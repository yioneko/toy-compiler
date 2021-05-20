#include "parser.hpp"
#include "gram.hpp"
#include "tokens.hpp"
#include <array>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <vector>
using std::unordered_set;
using std::vector;

struct Parser::ShiftAction : Action {
  const State &nextState;
  ShiftAction(const State &nextState,
              Action::ActionType type = ActionType::Shift)
      : Action(type), nextState(nextState) {}
};

struct Parser::ReduceAction : Action {
  const Gram &production;
  ReduceAction(const Gram &production,
               Action::ActionType type = ActionType::Reduce)
      : Action(type), production(production) {}
};

bool Parser::LALRItem::operator==(const LALRItem &rhs) const {
  return dot == rhs.dot && production == rhs.production /* &&
         lookAhead.get() == rhs.lookAhead.get() */
      ;
}

size_t Parser::LALRItem::Hash::operator()(const LALRItem &item) const {
  size_t ret = 0;
  using h = std::hash<unsigned>;
  for (auto &gram : *(item.production)) {
    ret = h()(ret * SYMBOLS_CNT + ret);
  }
  ret = h()(ret + item.dot);
  return ret;
}

// Parser::LALRItem::~LALRItem() { delete lookAhead; }

size_t Parser::State::Hash::operator()(const State &state) const {
  using h = LALRItem::Hash;
  using unsignedHash = std::hash<unsigned>;
  size_t ret = 0;
  for (auto &gram : state.kernel) {
    ret = unsignedHash()(ret + h()(gram));
  }

  for (auto &gram : state.trival) {
    ret = unsignedHash()(ret + h()(gram));
  }

  return ret;
};

bool Parser::isItemSetEql(const ItemSet &lhs, const ItemSet &rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }

  for (size_t i = 0; i < lhs.size(); ++i) {
    if (!(lhs[i] == rhs[i])) {
      return false;
    }
  }

  return true;
}

bool Parser::State::operator==(const State &rhs) const {
  return isItemSetEql(kernel, rhs.kernel) && isItemSetEql(trival, rhs.trival);
}

Parser::Parser(const std::vector<Token> &parsedTokens)
    : parsedTokens(parsedTokens), gramDef(this) {
  firstMemTable.resize(SYMBOLS_CNT);
  std::array<bool, SYMBOLS_CNT> firstVis;

  for (symbol_t sym = 0; sym < SYMBOLS_CNT; ++sym) {
    std::fill(firstVis.begin() + sym, firstVis.end(), false);
    firstMemTable[sym] = recurFirst(sym, firstVis);
  }

  auto closureRet =
      closure({LALRItem(gramDef.getProduction(Symbols::StartSymbol)[0], 1,
                        new unordered_set<symbol_t>{Tokens::EndInd})});
  actionTable.insert(actionTable.end(),
                     make_pair(closureRet.first,
                               vector<const Action *>(SYMBOLS_CNT, nullptr)));
  calcGoto(closureRet.first, closureRet.second);

  for (size_t i = 0; i < actionTable.size(); ++i) {
    for (auto &pir : actionTable) {
      auto &state = pir.first;
      for (size_t kernelIdx = 0; kernelIdx < state.kernel.size(); ++kernelIdx) {
        auto curLh = state.kernel[kernelIdx].lookAhead;
        for (auto &nxtLh : state.propagationLink->at(kernelIdx)) {
          for (auto &lh : *curLh) {
            if (!nxtLh->count(lh)) {
              nxtLh->insert(lh);
            }
          }
        }
      }
    }
  }

  for (auto &pir : actionTable) {
    auto &state = pir.first;
    for (size_t kernelIdx = 0; kernelIdx < state.kernel.size(); ++kernelIdx) {
      auto &item = state.kernel[kernelIdx];
      if (item.dot == item.production->size()) {
        for (auto &lh : *(item.lookAhead)) {
          if (lh != Symbols::Meaningless) {
            insertAction(state, lh, new ReduceAction(*(item.production)));
          }
        }
      }
    }
  }

  for (auto &pir : actionTable) {
    auto &state = pir.first;
    for (size_t kernelIdx = 0; kernelIdx < state.kernel.size(); ++kernelIdx) {
      auto &item = state.kernel[kernelIdx];
      for (size_t i = 0; i < item.production->size(); ++i) {
        if (item.dot == i) {
          printf("~ ");
        }
        printf("%d ", item.production->at(i));
      }

      printf("(");
      for (auto &lh : *(item.lookAhead)) {
        printf("%d ", lh);
      }
      printf(")");
      putchar('\n');
    }
    putchar('\n');
  }
  // reserved for program declaration
  code.push_back({});
}

const unordered_set<symbol_t> Parser::first(vector<symbol_t> &syms) const {
  unordered_set<symbol_t> ret;
  for (auto &sym : syms) {
    if (sym == Symbols::Meaningless) {
      ret.insert(sym);
      break;
    }
    ret.insert(firstMemTable[sym].begin(), firstMemTable[sym].end());
    if (!firstMemTable[sym].count(Symbols::None)) {
      break;
    }
  }
  return ret;
};

const unordered_set<symbol_t>
Parser::recurFirst(symbol_t sym, std::array<bool, SYMBOLS_CNT> &firstVis) {
  if (firstVis[sym]) {
    return firstMemTable[sym];
  }

  firstVis[sym] = true;
  if (sym < TOKEN_CNT) {
    return {sym};
  }

  unordered_set<symbol_t> ret;
  for (auto &prod : gramDef.getProduction(Symbols(sym))) {
    unordered_set<symbol_t> nxtRecurFirst = recurFirst(prod->at(1), firstVis);
    ret.insert(nxtRecurFirst.begin(), nxtRecurFirst.end());

    if (nxtRecurFirst.count(Symbols::None) && prod->size() > 2) {
      nxtRecurFirst = recurFirst(prod->at(2), firstVis);
      ret.insert(nxtRecurFirst.begin(), nxtRecurFirst.end());
    }
  }
  return ret;
};

void Parser::insertAction(const State &state, symbol_t symbol,
                          const Action *action) {
  if (actionTable[state].size() > symbol &&
      actionTable[state][symbol] == nullptr) {
    actionTable[state][symbol] = action;
  } else {
    throw new std::runtime_error("duplicate action");
    // TODO: throw error
  }
}

std::pair<Parser::State, Parser::SpontaneousSet>
Parser::closure(const ItemSet &kernel) const {
  SpontaneousSet spontaneousSet;
  auto propagationSet = new PropagationSet(kernel.size());

  using std::make_pair;
  std::queue<std::pair<LALRItem, symbol_t>> itemQueue;

  for (size_t kernelIdx = 0; kernelIdx < kernel.size(); ++kernelIdx) {
    auto &item = kernel[kernelIdx];
    itemQueue.push(
        make_pair(LALRItem(item.production, item.dot), Symbols::Meaningless));
    propagationSet->at(kernelIdx).insert(item);
    // nxtKernel[ItemWithHash{item}] = {Symbols::Meaningless};
  }
  unsigned kernelIdx = 0;

  ItemSet trival;
  while (!itemQueue.empty()) {
    const auto &item = itemQueue.front();
    itemQueue.pop();

    if (item.first.dot >= item.first.production->size()) {
      continue;
    }
    const symbol_t nxtSymbol = item.first.production->at(item.first.dot);
    if (nxtSymbol >= TOKEN_CNT) {
      for (auto &prod : gramDef.getProduction(Symbols(nxtSymbol))) {
        LALRItem newItem(prod);
        vector<symbol_t> trailingSymbols(item.first.production->begin() +
                                             item.first.dot,
                                         item.first.production->end());
        trailingSymbols.push_back(item.second);
        auto newFirst = first(trailingSymbols);
        auto visNewItemIter = spontaneousSet.find(LALRItem{newItem});

        for (auto &lookAhead : newFirst) {
          if (lookAhead == Symbols::Meaningless) {
            propagationSet->at(kernelIdx).insert(newItem);
          }
        }
        if (visNewItemIter == spontaneousSet.end()) {
          (spontaneousSet[LALRItem{newItem}] =
               unordered_set<symbol_t>(newFirst.begin(), newFirst.end()))
              .erase(Symbols::Meaningless);
          trival.push_back(newItem);
        } else {
          for (auto &lookAhead : newFirst) {
            if (!visNewItemIter->second.count(lookAhead)) {
              visNewItemIter->second.insert(lookAhead);
              itemQueue.push(make_pair(newItem, lookAhead));
            }
          }
        }
      }
    }
    ++kernelIdx;
  }

  return make_pair(
      State(kernel, trival, propagationSet, new PropagationLink(kernel.size())),
      spontaneousSet);
}

void Parser::calcGoto(const State &state, SpontaneousSet &spontaneousSet) {
  ItemSet allItems(state.kernel);
  allItems.insert(allItems.end(), state.trival.begin(), state.trival.end());

  for (symbol_t sym = 0; sym < SYMBOLS_CNT; ++sym) {
    ItemSet newKernel;

    for (auto &item : allItems) {
      if (item.dot >= item.production->size()) {
        continue;
      }

      if (item.production->at(item.dot) == sym) {
        LALRItem newItem(item.production, item.dot + 1);
        auto sponLhIter = spontaneousSet.find(item);
        if (sponLhIter != spontaneousSet.end()) {
          newItem.lookAhead = std::make_shared<unordered_set<symbol_t>>(
              unordered_set<symbol_t>(sponLhIter->second));
        } else {
          newItem.lookAhead =
              shared_ptr<unordered_set<symbol_t>>(new unordered_set<symbol_t>);
        }
        newKernel.push_back(newItem);
      }
    }

    if (!newKernel.empty()) {
      bool visited = true;

      auto closureRet = closure(newKernel);
      State &newState = closureRet.first;

      auto stateIter = actionTable.find(newState);
      if (stateIter == actionTable.end()) {
        visited = false;
        stateIter = actionTable.insert(
            actionTable.end(),
            make_pair(newState, vector<const Action *>(SYMBOLS_CNT, nullptr)));
      }

      for (size_t kernelIdx = 0; kernelIdx < state.kernel.size(); ++kernelIdx) {
        for (auto &newItem : stateIter->first.kernel) {
          if (state.propagationSet->at(kernelIdx).count(
                  LALRItem(newItem.production, newItem.dot - 1))) {
            state.propagationLink->at(kernelIdx).push_back(newItem.lookAhead);
          }
        }
      }

      insertAction(state, sym, new ShiftAction(newState));
      if (!visited)
        calcGoto(newState, closureRet.second);
    }
  }
}

void Parser::parse() {}

void Parser::throwSyntaxError() const {};

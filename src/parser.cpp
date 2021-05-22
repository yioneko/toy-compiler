#include "parser.hpp"
#include "gram.hpp"
#include "tokens.hpp"
#include <array>
#include <cstdio>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <vector>
// #define DEBUG
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

void Parser::State::print() const {
  printf("Kernel:\n");
  for (size_t kernelIdx = 0; kernelIdx < kernel.size(); ++kernelIdx) {
    auto &item = kernel[kernelIdx];
    for (size_t i = 0; i < item.production->size(); ++i) {
      printf("%d ", item.production->at(i));
      if (item.dot == i + 1) {
        printf("~ ");
      }
    }

    printf("(");
    bool fir = true;
    for (auto &lh : *(item.lookAhead)) {
      if (fir) {
        printf("%d", lh);
        fir = false;
      } else {
        printf(" %d", lh);
      }
    }
    printf(")");
    putchar('\n');
  }
  printf("Trival:\n");
  for (size_t trivalIdx = 0; trivalIdx < trival.size(); ++trivalIdx) {
    auto &item = trival[trivalIdx];
    for (size_t i = 0; i < item.production->size(); ++i) {
      printf("%d ", item.production->at(i));
      if (item.dot == i + 1) {
        printf("~ ");
      }
    }
    putchar('\n');
  }
  putchar('\n');
}

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
  return isItemSetEql(kernel,
                      rhs.kernel) /*  && isItemSetEql(trival, rhs.trival) */;
}

Parser::Parser(const std::vector<Token> &parsedTokens) : gramDef(this) {
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
          if (lh != Tokens::None) {
            insertAction(state, lh, new ReduceAction(*(item.production)));
          }
        }
      }
    }
  }

  for (auto &token : parsedTokens) {
    this->parsedTokens.emplace_back(new Terminal(token));
  }
  this->parsedTokens.emplace_back(
      new Terminal(Token(Tokens::EndInd, "", -1, -1)));

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
    if (!firstMemTable[sym].count(Tokens::None)) {
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

    if (nxtRecurFirst.count(Tokens::None) && prod->size() > 2) {
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
#ifdef DEBUG
    auto oldAction = actionTable[state][symbol];
    if (!(oldAction->type == Action::ActionType::Reduce &&
          action->type == Action::ActionType::Reduce &&
          static_cast<const ReduceAction *>(oldAction)->production ==
              static_cast<const ReduceAction *>(action)->production)) {
      state.print();
      for (auto &i : static_cast<const ReduceAction *>(action)->production) {
        printf("%d ", i);
      }
      putchar('\n');
      // throw std::runtime_error("duplicate action");
    }
#endif
    delete action;
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
                                             item.first.dot + 1,
                                         item.first.production->end());
        trailingSymbols.push_back(item.second);
        auto newFirst = first(trailingSymbols);
        auto visNewItemIter = spontaneousSet.find(newItem);

        for (auto &lookAhead : newFirst) {
          if (lookAhead == Symbols::Meaningless) {
            if (kernelIdx >= kernel.size()) {
              for (auto &progSet : *propagationSet) {
                if (progSet.count(item.first)) {
                  progSet.insert(newItem);
                }
              }
            } else {
              propagationSet->at(kernelIdx).insert(newItem);
            }
          }
        }
        if (visNewItemIter == spontaneousSet.end()) {
          visNewItemIter =
              spontaneousSet.insert(spontaneousSet.end(), {newItem, {}});
          trival.push_back(newItem);
        }
        for (auto &lookAhead : newFirst) {
          if (!visNewItemIter->second.count(lookAhead)) {
            visNewItemIter->second.insert(lookAhead);
            itemQueue.push(make_pair(newItem, lookAhead));
          }
        }
      }
    }
    ++kernelIdx;
  }

  for (auto &pir : spontaneousSet) {
    pir.second.erase(Symbols::Meaningless);
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
      } else {
        for (size_t kernelIdx = 0; kernelIdx < stateIter->first.kernel.size();
             ++kernelIdx) {
          auto &lh = stateIter->first.kernel[kernelIdx].lookAhead;
          lh->insert(newKernel[kernelIdx].lookAhead->begin(),
                     newKernel[kernelIdx].lookAhead->end());
        }
      }

      for (size_t kernelIdx = 0; kernelIdx < state.kernel.size(); ++kernelIdx) {
        for (auto &newItem : stateIter->first.kernel) {
          if (state.propagationSet->at(kernelIdx).count(
                  LALRItem(newItem.production, newItem.dot - 1))) {
            state.propagationLink->at(kernelIdx).push_back(newItem.lookAhead);
          }
        }
      }

      insertAction(state, sym, new ShiftAction(stateIter->first));
      if (!visited)
        calcGoto(newState, closureRet.second);
    }
  }
}

void Parser::parse() {
  stateStk.push(
      closure({LALRItem(gramDef.getProduction(Symbols::StartSymbol)[0], 1,
                        new unordered_set<symbol_t>{Tokens::EndInd})})
          .first);

  for (size_t tokenIdx = 0; tokenIdx < parsedTokens.size();) {
    auto &curState = stateStk.top();
    bool tryNone = true;
    auto nextAction = actionTable[curState][Tokens::None];
    shared_ptr<Symbol> nextSymbol(
        new Terminal(Token(Tokens::None, "", -1, -1)));
    if (nextAction == nullptr ||
        actionTable[static_cast<const ShiftAction *>(nextAction)->nextState]
                   [parsedTokens[tokenIdx]->token.type] == nullptr) {
      nextAction = actionTable[curState][parsedTokens[tokenIdx]->token.type];
      tryNone = false;
      nextSymbol = parsedTokens[tokenIdx];
    }

    if (nextAction == nullptr) {
      throwParserError("Syntax error", parsedTokens[tokenIdx]->token);
    }

    if (nextAction->type == Action::ActionType::Reduce) {
      auto &prod = static_cast<const ReduceAction *>(nextAction)->production;
      nextSymbol = gramDef.getSementicAction(prod)(vector<shared_ptr<Symbol>>(
          parsedSymbols.end() - prod.size() + 1, parsedSymbols.end()));
      for (size_t i = 0; i < prod.size() - 1; ++i) {
        parsedSymbols.pop_back();
        stateStk.pop();
      }

      // finish
      if (prod.at(0) == Symbols::StartSymbol) {
        return;
      }

      nextAction = actionTable[stateStk.top()][nextSymbol->getType()];
      tryNone = true;
    }

    if (nextAction == nullptr) {
      throwParserError("Syntax error", parsedTokens[tokenIdx]->token);
    }

    if (nextAction->type == Action::ActionType::Shift) {
      parsedSymbols.push_back(nextSymbol);
      stateStk.push(static_cast<const ShiftAction *>(nextAction)->nextState);
      if (!tryNone)
        ++tokenIdx;
    }
  }
}

void Parser::throwParserError(const std::string &errorType,
                              const Token &token) const {
  throw std::runtime_error(errorType + " near '" + token.lexeme + "' at line " +
                           std::to_string(token.line) + ", col " +
                           std::to_string(token.col) + ".");
};

void Parser::printCode() const {
  for (size_t lineNum = 0; lineNum < code.size(); ++lineNum) {
    auto &line = code[lineNum];
    printf("(%lu) (%s", lineNum, line[0].c_str());
    for (size_t i = 1; i < line.size(); ++i) {
      printf(", %s", line[i].c_str());
    }
    printf(")\n");
  }
}

Parser::~Parser() {
  for (auto &pir : actionTable) {
    for (auto &action : pir.second) {
      delete action;
    }
  }
}

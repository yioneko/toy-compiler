#ifndef PARSER_H
#define PARSER_H

#include "gram.hpp"
#include "symbols.hpp"
#include <array>
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Parser {
private:
  struct LALRItem {
    const Gram *production;
    unsigned dot;
    std::shared_ptr<std::unordered_set<symbol_t>> lookAhead;

    explicit LALRItem(const Gram *const &production, unsigned dot = 1,
                      std::unordered_set<symbol_t> *lookAhead = nullptr)
        : production(production), dot(dot), lookAhead(lookAhead) {}

    struct Hash {
      size_t operator()(const LALRItem &item) const;
    };
    bool operator==(const LALRItem &) const;
  };

  typedef std::vector<LALRItem> ItemSet;

  static bool isItemSetEql(const ItemSet &lhs, const ItemSet &rhs);
  std::vector<std::unordered_set<symbol_t>> firstMemTable;
  const std::unordered_set<symbol_t> first(std::vector<symbol_t> &) const;
  const std::unordered_set<symbol_t>
  recurFirst(symbol_t, std::array<bool, SYMBOLS_CNT> &firstVis);

  typedef std::unordered_map<LALRItem, std::unordered_set<symbol_t>,
                             LALRItem::Hash>
      SpontaneousSet;
  typedef std::vector<std::unordered_set<LALRItem, LALRItem::Hash>>
      PropagationSet;
  typedef std::vector<
      std::vector<std::shared_ptr<std::unordered_set<symbol_t>>>>
      PropagationLink;

  struct Action;

  struct State {
    ItemSet kernel;
    ItemSet trival;
    std::shared_ptr<PropagationSet> propagationSet;
    std::shared_ptr<PropagationLink> propagationLink;

    State(const ItemSet &kernel, const ItemSet &trival,
          PropagationSet *propagationSet, PropagationLink *propagationLink)
        : kernel(kernel), trival(trival), propagationSet(propagationSet),
          propagationLink(propagationLink) {}
    bool operator==(const State &) const;
    struct Hash {
      size_t operator()(const State &state) const;
    };

    // ~State() ;
  };

  struct Action {
    enum class ActionType { Shift, Reduce };
    const ActionType type;
    explicit Action(ActionType type) : type(type) {}
  };
  struct ShiftAction;
  struct ReduceAction;

  std::unordered_map<State, std::vector<const Action *>, State::Hash>
      actionTable;
  void insertAction(const State &state, symbol_t symbol, const Action *action);

  std::pair<State, SpontaneousSet> closure(const ItemSet &kernel) const;
  void calcGoto(const State &state, SpontaneousSet &spontaneousSet);

  const vector<Token> &parsedTokens;
  vector<shared_ptr<Symbol>> parsedSymbols;
  std::stack<State> stateStk;

  void throwSyntaxError() const;

  friend class GramDef;
  GramDef gramDef;

  typedef std::array<std::string, 4> QuaTuple;
  vector<QuaTuple> code;

public:
  explicit Parser(const std::vector<Token> &parsedTokens);
  void parse();
};

#endif

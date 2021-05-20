#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "tokens.hpp"
using std::string;
using std::vector;

enum Symbols : unsigned {
  Terminals = TOKEN_CNT - 1,
  Expression,
  ArithmeticExpression,
  BooleanExpression,
  CharExpression,
  Item,
  Factor,
  ArithmeticUnit,
  BooleanItem,
  BooleanFactor,
  BooleanUnit,
  RelationOperator,
  Statement,
  Assignment,
  IfStatement,
  WhileStatement,
  RepeatStatement,
  CompoundStatement,
  StatementTable,
  Program,
  VariablePreamble,
  VariableDeclaration,
  Type,
  IdentifierTable,
  StartSymbol,
  None,
  M,
  N,
  SYMBOLS_CNT,
  Meaningless,
};

typedef unsigned symbol_t;

struct Symbol {
  virtual Symbols getType() = 0;
};

struct Terminal : Symbol {
  Token *token;
  explicit Terminal(Token *const &token);
  const static Symbols symbolType = Symbols::Terminals;
  Symbols getType() override;
};

struct Expression : Symbol {
  const static Symbols symbolType = Symbols::Expression;
  string label;
  Symbols getType() override;
};

struct ArithmeticExpression : Symbol {
  const static Symbols symbolType = Symbols::ArithmeticExpression;
  string label;
  Symbols getType() override;
};

struct BooleanExpression : Symbol {
  const static Symbols symbolType = Symbols::BooleanExpression;
  vector<size_t> trueList;
  vector<size_t> falseList;
  Symbols getType() override;
};

struct CharExpression : Symbol {
  const static Symbols symbolType = Symbols::CharExpression;
  string label;
  Symbols getType() override;
};

struct Item : Symbol {
  const static Symbols symbolType = Symbols::Item;
  string label;
  Symbols getType() override;
};

struct Factor : Symbol {
  const static Symbols symbolType = Symbols::Factor;
  string label;
  Symbols getType() override;
};

struct ArithmeticUnit : Symbol {
  const static Symbols symbolType = Symbols::ArithmeticUnit;
  string label;
  Symbols getType() override;
};

struct BooleanItem : Symbol {
  const static Symbols symbolType = Symbols::BooleanItem;
  vector<size_t> trueList;
  vector<size_t> falseList;
  Symbols getType() override;
};

struct BooleanFactor : Symbol {
  const static Symbols symbolType = Symbols::BooleanFactor;
  vector<size_t> trueList;
  vector<size_t> falseList;
  Symbols getType() override;
};

struct BooleanUnit : Symbol {
  const static Symbols symbolType = Symbols::BooleanUnit;
  vector<size_t> trueList;
  vector<size_t> falseList;
  Symbols getType() override;
};

struct RelationOperator : Symbol {
  const static Symbols symbolType = Symbols::RelationOperator;
  string op;
  Symbols getType() override;
};

struct Statement : Symbol {
  const static Symbols symbolType = Symbols::Statement;
  vector<size_t> nextList;
  Symbols getType() override;
};

struct Assignment : Symbol {
  const static Symbols symbolType = Symbols::Assignment;
  vector<size_t> nextList;
  Symbols getType() override;
};

struct IfStatement : Symbol {
  const static Symbols symbolType = Symbols::IfStatement;
  vector<size_t> nextList;
  Symbols getType() override;
};

struct WhileStatement : Symbol {
  const static Symbols symbolType = Symbols::WhileStatement;
  vector<size_t> nextList;
  Symbols getType() override;
};

struct RepeatStatement : Symbol {
  const static Symbols symbolType = Symbols::RepeatStatement;
  vector<size_t> nextList;
  Symbols getType() override;
};

struct CompoundStatement : Symbol {
  const static Symbols symbolType = Symbols::CompoundStatement;
  const vector<size_t> nextList = {};
  Symbols getType() override;
};

struct StatementTable : Symbol {
  const static Symbols symbolType = Symbols::StatementTable;
  Symbols getType() override;
};

struct Program : Symbol {
  const static Symbols symbolType = Symbols::Program;
  Symbols getType() override;
};

struct VariablePreamble : Symbol {
  const static Symbols symbolType = Symbols::VariablePreamble;
  Symbols getType() override;
};

struct VariableDeclaration : Symbol {
  const static Symbols symbolType = Symbols::VariableDeclaration;
  Symbols getType() override;
};

struct Type : Symbol {
  const static Symbols symbolType = Symbols::Type;
  Symbols getType() override;
};

struct IdentifierTable : Symbol {
  const static Symbols symbolType = Symbols::IdentifierTable;
  Symbols getType() override;
};

struct M : Symbol {
  const static Symbols symbolType = Symbols::M;
  size_t instr;
  Symbols getType() override;
};

struct N : Symbol {
  const static Symbols symbolType = Symbols::M;
  size_t instr;
  Symbols getType() override;
};

struct StartSymbol : Symbol {
  const static Symbols symbolType = Symbols::StartSymbol;
  Symbols getType() override;
};
#endif

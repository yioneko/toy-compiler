#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "tokens.hpp"

enum Symbols {
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
  SYMBOLS_CNT
};

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
  Symbols getType() override;
};

struct ArithmeticExpression : Symbol {
  const static Symbols symbolType = Symbols::ArithmeticExpression;
  Symbols getType() override;
};

struct BooleanExpression : Symbol {
  const static Symbols symbolType = Symbols::BooleanExpression;
  Symbols getType() override;
};

struct CharExpression : Symbol {
  const static Symbols symbolType = Symbols::CharExpression;
  Symbols getType() override;
};

struct Item : Symbol {
  const static Symbols symbolType = Symbols::Item;
  Symbols getType() override;
};

struct Factor : Symbol {
  const static Symbols symbolType = Symbols::Factor;
  Symbols getType() override;
};

struct ArithmeticUnit : Symbol {
  const static Symbols symbolType = Symbols::ArithmeticUnit;
  Symbols getType() override;
};

struct BooleanItem : Symbol {
  const static Symbols symbolType = Symbols::BooleanItem;
  Symbols getType() override;
};

struct BooleanFactor : Symbol {
  const static Symbols symbolType = Symbols::BooleanFactor;
  Symbols getType() override;
};

struct BooleanUnit : Symbol {
  const static Symbols symbolType = Symbols::BooleanUnit;
  Symbols getType() override;
};

struct RelationOperator : Symbol {
  const static Symbols symbolType = Symbols::RelationOperator;
  Symbols getType() override;
};

struct Statement : Symbol {
  const static Symbols symbolType = Symbols::Statement;
  Symbols getType() override;
};

struct Assignment : Symbol {
  const static Symbols symbolType = Symbols::Assignment;
  Symbols getType() override;
};

struct IfStatement : Symbol {
  const static Symbols symbolType = Symbols::IfStatement;
  Symbols getType() override;
};

struct WhileStatement : Symbol {
  const static Symbols symbolType = Symbols::WhileStatement;
  Symbols getType() override;
};

struct RepeatStatement : Symbol {
  const static Symbols symbolType = Symbols::RepeatStatement;
  Symbols getType() override;
};

struct CompoundStatement : Symbol {
  const static Symbols symbolType = Symbols::CompoundStatement;
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

#endif

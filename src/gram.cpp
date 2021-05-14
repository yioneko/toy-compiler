#include "gram.hpp"
#include "symbols.hpp"
#include "tokens.hpp"
#include <unordered_map>

struct GramWithHash {
  Gram gram;
  GramWithHash(const Gram &gram) : gram(gram) {}

  bool operator==(const GramWithHash &rhs) const {
    if (this->gram.size() != rhs.gram.size())
      return false;
    for (int i = 0; i < gram.size(); ++i) {
      if (this->gram[i] != rhs.gram[i])
        return false;
    }
    return true;
  }

  struct Hash {
    size_t operator()(const GramWithHash &gram) const {
      using h = std::hash<unsigned>;
      size_t ret = 0;
      for (auto &sym : gram.gram) {
        ret = h()(ret * SYMBOLS_CNT + sym);
      }
      return ret;
    }
  };
};

const std::unordered_map<GramWithHash, SementicAction, GramWithHash::Hash>
    sementicActions({
        {Gram{Symbols::Expression, Symbols::ArithmeticExpression},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Expression, Symbols::BooleanExpression},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Expression, Symbols::CharExpression},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::ArithmeticExpression, Symbols::ArithmeticExpression,
              Tokens::Plus, Symbols::Item},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::ArithmeticExpression, Symbols::ArithmeticExpression,
              Tokens::Minus, Symbols::Item},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::ArithmeticExpression, Symbols::Item},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Item, Symbols::Item, Tokens::Asterisk, Symbols::Factor},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Item, Symbols::Item, Tokens::Slash, Symbols::Factor},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Item, Symbols::Factor},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Factor, Symbols::ArithmeticUnit},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Factor, Tokens::Minus, Symbols::Factor},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::ArithmeticUnit, Tokens::IntLIteral},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::ArithmeticUnit, Tokens::Identifier},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::ArithmeticUnit, Tokens::LeftBracket,
              Symbols::ArithmeticExpression, Tokens::RightBracket},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanExpression, Symbols::BooleanExpression,
              Tokens::OR, Symbols::BooleanItem},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanExpression, Symbols::BooleanItem},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanItem, Symbols::BooleanItem, Tokens::AND,
              Symbols::BooleanFactor},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanUnit, Tokens::TRUE},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanUnit, Tokens::FALSE},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanUnit, Tokens::Identifier},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanUnit, Tokens::LeftBracket,
              Symbols::BooleanExpression, Tokens::RightBracket},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanUnit, Tokens::Identifier,
              Symbols::RelationOperator, Tokens::Identifier},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::BooleanUnit, Symbols::ArithmeticExpression,
              Symbols::RelationOperator, Symbols::ArithmeticExpression},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::RelationOperator, Tokens::Less},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::RelationOperator, Tokens::NotEqual},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::RelationOperator, Tokens::LessEqual},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::RelationOperator, Tokens::LargerEqual},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::RelationOperator, Tokens::Larger},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::RelationOperator, Tokens::Equal},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::CharExpression, Tokens::CharLiteral},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::CharExpression, Tokens::Identifier},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Statement, Symbols::Assignment},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Statement, Symbols::IfStatement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Statement, Symbols::WhileStatement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Statement, Symbols::RepeatStatement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Statement, Symbols::CompoundStatement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Assignment, Tokens::Identifier, Tokens::Assign,
              Symbols::ArithmeticExpression},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::IfStatement, Tokens::IF, Symbols::BooleanExpression,
              Tokens::THEN, Symbols::Statement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::IfStatement, Tokens::IF, Symbols::BooleanExpression,
              Tokens::THEN, Symbols::Statement, Tokens::ELSE,
              Symbols::Statement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::WhileStatement, Tokens::WHILE,
              Symbols::BooleanExpression, Tokens::DO, Symbols::Statement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::RepeatStatement, Tokens::REPEAT, Symbols::Statement,
              Tokens::UNTIL, Symbols::BooleanExpression},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::CompoundStatement, Tokens::BEGIN,
              Symbols::StatementTable, Tokens::END},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::StatementTable, Symbols::Statement, Tokens::Semicolon,
              Symbols::StatementTable},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::StatementTable, Symbols::Statement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Program, Tokens::PROGRAM, Tokens::Semicolon,
              Symbols::VariablePreamble, Symbols::CompoundStatement},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::VariablePreamble, Tokens::VAR,
              Symbols::VariableDeclaration},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::VariableDeclaration, Symbols::IdentifierTable,
              Tokens::Colon, Symbols::Type, Tokens::Semicolon,
              Symbols::VariableDeclaration},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::VariableDeclaration, Symbols::IdentifierTable,
              Tokens::Colon, Symbols::Type, Tokens::Semicolon},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::VariablePreamble},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::IdentifierTable, Tokens::Identifier,
              Symbols::IdentifierTable},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::IdentifierTable, Tokens::Identifier},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Type, Tokens::INTEGER},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Type, Tokens::BOOL},
         [](const std::initializer_list<Symbol> &) {}},

        {Gram{Symbols::Type, Tokens::CHAR},
         [](const std::initializer_list<Symbol> &) {}},
    });

using std::vector;
typedef vector<vector<const Gram *>> Productions;
const Productions * const getProductionsFromMap() {
  Productions *ret = new Productions;
  ret->resize(SYMBOLS_CNT - TOKEN_CNT);
  for (auto &pir : sementicActions) {
    ret->at(pir.first.gram[0] - TOKEN_CNT).push_back(&pir.first.gram);
  }
  return ret;
}
const Productions * const productions = getProductionsFromMap();

SementicAction getSementicAction(const Gram &gram) {
  return sementicActions.find(gram)->second;
}

const vector<const Gram *> getProduction(Symbols sym) {
  return productions->at(sym - TOKEN_CNT);
}

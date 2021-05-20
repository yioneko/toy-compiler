#include "gram.hpp"
#include "parser.hpp"
#include "symbols.hpp"
#include "tokens.hpp"

using std::dynamic_pointer_cast;
using std::vector;
GramDef::GramDef(Parser *parser)
    : parser(parser), tempLabelCnt(0),
      sementicActions({
          {Gram{Symbols::Expression, Symbols::ArithmeticExpression},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Expression> Exp(new struct Expression);
             Exp->label =
                 dynamic_pointer_cast<struct ArithmeticExpression>(syms[0])
                     ->label;
             return dynamic_pointer_cast<Symbol>(Exp);
           }},
          {Gram{Symbols::Expression, Symbols::BooleanExpression},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Expression> Exp(new struct Expression);
             //  Exp->label =
             //      dynamic_pointer_cast<struct
             //      BooleanExpression>(syms[0])->label;
             return dynamic_pointer_cast<Symbol>(Exp);
           }},

          {Gram{Symbols::Expression, Symbols::CharExpression},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Expression> Exp(new struct Expression);
             Exp->label =
                 dynamic_pointer_cast<struct CharExpression>(syms[0])->label;
             return dynamic_pointer_cast<Symbol>(Exp);
           }},

          {Gram{Symbols::ArithmeticExpression, Symbols::ArithmeticExpression,
                Tokens::Plus, Symbols::Item},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct ArithmeticExpression> arith(
                 new struct ArithmeticExpression);
             arith->label = newTemp();
             emitCode(
                 {"+",
                  dynamic_pointer_cast<struct ArithmeticExpression>(syms[0])
                      ->label,
                  dynamic_pointer_cast<struct Item>(syms[2])->label,
                  arith->label});
             return dynamic_pointer_cast<Symbol>(arith);
           }},

          {Gram{Symbols::ArithmeticExpression, Symbols::ArithmeticExpression,
                Tokens::Minus, Symbols::Item},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct ArithmeticExpression> arith(
                 new struct ArithmeticExpression);
             arith->label = newTemp();
             emitCode(
                 {"-",
                  dynamic_pointer_cast<struct ArithmeticExpression>(syms[0])
                      ->label,
                  dynamic_pointer_cast<struct Item>(syms[2])->label,
                  arith->label});
             return dynamic_pointer_cast<Symbol>(arith);
           }},

          {Gram{Symbols::ArithmeticExpression, Symbols::Item},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct ArithmeticExpression> arith(
                 new struct ArithmeticExpression);
             arith->label = dynamic_pointer_cast<struct Item>(syms[0])->label;
             return dynamic_pointer_cast<Symbol>(arith);
           }},

          {Gram{Symbols::Item, Symbols::Item, Tokens::Asterisk,
                Symbols::Factor},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Item> item(new struct Item);
             item->label = newTemp();
             emitCode({"*", dynamic_pointer_cast<struct Item>(syms[0])->label,
                       dynamic_pointer_cast<struct Factor>(syms[2])->label,
                       item->label});
             return dynamic_pointer_cast<Symbol>(item);
           }},

          {Gram{Symbols::Item, Symbols::Item, Tokens::Slash, Symbols::Factor},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Item> item(new struct Item);
             item->label = newTemp();
             emitCode({"/", dynamic_pointer_cast<struct Item>(syms[0])->label,
                       dynamic_pointer_cast<struct Factor>(syms[2])->label,
                       item->label});
             return dynamic_pointer_cast<Symbol>(item);
           }},

          {Gram{Symbols::Item, Symbols::Factor},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Item> item(new struct Item);
             item->label = dynamic_pointer_cast<struct Factor>(syms[0])->label;
             return dynamic_pointer_cast<Symbol>(item);
           }},

          {Gram{Symbols::Factor, Symbols::ArithmeticUnit},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Factor> factor(new struct Factor);
             factor->label =
                 dynamic_pointer_cast<struct ArithmeticUnit>(syms[0])->label;
             return dynamic_pointer_cast<Symbol>(factor);
           }},

          {Gram{Symbols::Factor, Tokens::Minus, Symbols::Factor},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Factor> factor(new struct Factor);
             factor->label = newTemp();
             emitCode({"-", "0",
                       dynamic_pointer_cast<struct Factor>(syms[0])->label,
                       factor->label});
             return dynamic_pointer_cast<Symbol>(factor);
           }},

          {Gram{Symbols::ArithmeticUnit, Tokens::IntLIteral},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct ArithmeticUnit> arith(new struct ArithmeticUnit);
             arith->label = newTemp();
             emitCode(
                 {":=",
                  dynamic_pointer_cast<struct Terminal>(syms[0])->token->lexeme,
                  "-", arith->label});
             return dynamic_pointer_cast<Symbol>(arith);
           }},

          {Gram{Symbols::ArithmeticUnit, Tokens::Identifier},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct ArithmeticUnit> arith(new struct ArithmeticUnit);
             arith->label =
                 dynamic_pointer_cast<struct Terminal>(syms[0])->token->lexeme;
             return dynamic_pointer_cast<Symbol>(arith);
           }},

          {Gram{Symbols::ArithmeticUnit, Tokens::LeftBracket,
                Symbols::ArithmeticExpression, Tokens::RightBracket},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct ArithmeticUnit> arith(new struct ArithmeticUnit);
             arith->label =
                 dynamic_pointer_cast<struct ArithmeticExpression>(syms[1])
                     ->label;
             return dynamic_pointer_cast<Symbol>(arith);
           }},

          {Gram{Symbols::M, Symbols::None},
           [this](const vector<shared_ptr<Symbol>> &) {
             shared_ptr<struct M> m(new struct M);
             m->instr = nextInstr();
             return dynamic_pointer_cast<Symbol>(m);
           }},

          {Gram{Symbols::BooleanExpression, Symbols::BooleanExpression,
                Symbols::M, Tokens::OR, Symbols::BooleanItem},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct BooleanExpression> exp(
                 new struct BooleanExpression);
             auto b1 = dynamic_pointer_cast<struct BooleanExpression>(syms[0]);
             auto b2 = dynamic_pointer_cast<struct BooleanItem>(syms[3]);
             backPatch(b1->falseList,
                       dynamic_pointer_cast<struct M>(syms[1])->instr);
             exp->trueList = b1->trueList;
             exp->trueList.insert(exp->trueList.end(), b2->trueList.begin(),
                                  b2->trueList.end());
             exp->falseList = b2->falseList;
             return dynamic_pointer_cast<Symbol>(exp);
           }},

          {Gram{Symbols::BooleanExpression, Symbols::BooleanItem},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct BooleanExpression> exp(
                 new struct BooleanExpression);
             auto b1 = dynamic_pointer_cast<struct BooleanExpression>(syms[0]);
             exp->trueList = b1->trueList;
             exp->falseList = b1->falseList;
             return dynamic_pointer_cast<Symbol>(exp);
           }},

          {Gram{Symbols::BooleanItem, Symbols::BooleanItem, Symbols::M,
                Tokens::AND, Symbols::BooleanFactor},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct BooleanItem> item(new struct BooleanItem);
             auto b1 = dynamic_pointer_cast<struct BooleanItem>(syms[0]);
             auto b2 = dynamic_pointer_cast<struct BooleanFactor>(syms[3]);
             backPatch(b1->trueList,
                       dynamic_pointer_cast<struct M>(syms[1])->instr);
             item->trueList = b2->trueList;
             item->falseList = b1->falseList;
             item->falseList.insert(item->trueList.end(), b2->falseList.begin(),
                                    b2->falseList.end());
             return dynamic_pointer_cast<Symbol>(item);
           }},

          {Gram{Symbols::BooleanUnit, Tokens::TRUE},
           [this](const vector<shared_ptr<Symbol>> &) {
             shared_ptr<struct BooleanUnit> unit(new struct BooleanUnit);
             unit->trueList = {nextInstr()};
             emitCode({"j", "-", "-", "-"});
             return dynamic_pointer_cast<Symbol>(unit);
           }},

          {Gram{Symbols::BooleanUnit, Tokens::FALSE},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct BooleanUnit> unit(new struct BooleanUnit);
             unit->falseList = {nextInstr()};
             emitCode({"j", "-", "-", "-"});
             return dynamic_pointer_cast<Symbol>(unit);
           }},

          {Gram{Symbols::BooleanUnit, Tokens::Identifier},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct BooleanUnit> unit(new struct BooleanUnit);
             const string &id =
                 dynamic_pointer_cast<struct Terminal>(syms[0])->token->lexeme;
             unit->trueList = {nextInstr()};
             emitCode({"j<>", id, 0, "-"});

             unit->falseList = {nextInstr()};
             emitCode({"j", "-", "-", "-"});
             return dynamic_pointer_cast<Symbol>(unit);
           }},

          {Gram{Symbols::BooleanUnit, Tokens::LeftBracket,
                Symbols::BooleanExpression, Tokens::RightBracket},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct BooleanUnit> unit(new struct BooleanUnit);
             auto b1 = dynamic_pointer_cast<struct BooleanExpression>(syms[1]);
             unit->trueList = b1->trueList;
             unit->falseList = b1->falseList;
             return dynamic_pointer_cast<Symbol>(unit);
           }},

          {Gram{Symbols::BooleanUnit, Tokens::Identifier,
                Symbols::RelationOperator, Tokens::Identifier},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct BooleanUnit> unit(new struct BooleanUnit);
             const string &id1 =
                 dynamic_pointer_cast<struct Terminal>(syms[0])->token->lexeme;
             const string &id2 =
                 dynamic_pointer_cast<struct Terminal>(syms[2])->token->lexeme;
             auto op =
                 dynamic_pointer_cast<struct RelationOperator>(syms[1])->op;

             unit->trueList = {nextInstr()};
             emitCode({"j" + op, id1, id2, "-"});

             unit->falseList = {nextInstr()};
             emitCode({"j", "-", "-", "-"});
             return dynamic_pointer_cast<Symbol>(unit);
           }},

          {Gram{Symbols::BooleanUnit, Symbols::ArithmeticExpression,
                Symbols::RelationOperator, Symbols::ArithmeticExpression},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct BooleanUnit> unit(new struct BooleanUnit);
             const string &label1 =
                 dynamic_pointer_cast<struct ArithmeticExpression>(syms[0])
                     ->label;
             const string &label2 =
                 dynamic_pointer_cast<struct ArithmeticExpression>(syms[2])
                     ->label;
             auto op =
                 dynamic_pointer_cast<struct RelationOperator>(syms[1])->op;

             unit->trueList = {nextInstr()};
             emitCode({"j" + op, label1, label2, "-"});

             unit->falseList = {nextInstr()};
             emitCode({"j", "-", "-", "-"});
             return dynamic_pointer_cast<Symbol>(unit);
           }},

          {Gram{Symbols::RelationOperator, Tokens::Less},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct RelationOperator> rel(
                 new struct RelationOperator);
             rel->op = "<";
             return dynamic_pointer_cast<Symbol>(rel);
           }},

          {Gram{Symbols::RelationOperator, Tokens::NotEqual},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct RelationOperator> rel(
                 new struct RelationOperator);
             rel->op = "<>";
             return dynamic_pointer_cast<Symbol>(rel);
           }},

          {Gram{Symbols::RelationOperator, Tokens::LessEqual},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct RelationOperator> rel(
                 new struct RelationOperator);
             rel->op = "<=";
             return dynamic_pointer_cast<Symbol>(rel);
           }},

          {Gram{Symbols::RelationOperator, Tokens::LargerEqual},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct RelationOperator> rel(
                 new struct RelationOperator);
             rel->op = ">=";
             return dynamic_pointer_cast<Symbol>(rel);
           }},

          {Gram{Symbols::RelationOperator, Tokens::Larger},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct RelationOperator> rel(
                 new struct RelationOperator);
             rel->op = ">";
             return dynamic_pointer_cast<Symbol>(rel);
           }},

          {Gram{Symbols::RelationOperator, Tokens::Equal},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct RelationOperator> rel(
                 new struct RelationOperator);
             rel->op = "=";
             return dynamic_pointer_cast<Symbol>(rel);
           }},

          {Gram{Symbols::CharExpression, Tokens::CharLiteral},
           [](const vector<shared_ptr<Symbol>> &) {
             shared_ptr<struct CharExpression> exp(new struct CharExpression);
             return dynamic_pointer_cast<Symbol>(exp);
           }},

          {Gram{Symbols::CharExpression, Tokens::Identifier},
           [](const vector<shared_ptr<Symbol>> &) {
             shared_ptr<struct CharExpression> exp(new struct CharExpression);
             return dynamic_pointer_cast<Symbol>(exp);
           }},

          {Gram{Symbols::Statement, Symbols::Assignment},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Statement> st(new struct Statement);
             st->nextList =
                 dynamic_pointer_cast<struct Assignment>(syms[0])->nextList;
             return dynamic_pointer_cast<Symbol>(st);
           }},

          {Gram{Symbols::Statement, Symbols::IfStatement},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Statement> st(new struct Statement);
             st->nextList =
                 dynamic_pointer_cast<struct IfStatement>(syms[0])->nextList;
             return dynamic_pointer_cast<Symbol>(st);
           }},

          {Gram{Symbols::Statement, Symbols::WhileStatement},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Statement> st(new struct Statement);
             st->nextList =
                 dynamic_pointer_cast<struct WhileStatement>(syms[0])->nextList;
             return dynamic_pointer_cast<Symbol>(st);
           }},

          {Gram{Symbols::Statement, Symbols::RepeatStatement},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Statement> st(new struct Statement);
             st->nextList =
                 dynamic_pointer_cast<struct RepeatStatement>(syms[0])
                     ->nextList;
             return dynamic_pointer_cast<Symbol>(st);
           }},

          {Gram{Symbols::Statement, Symbols::CompoundStatement},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Statement> st(new struct Statement);
             st->nextList =
                 dynamic_pointer_cast<struct CompoundStatement>(syms[0])
                     ->nextList;
             return dynamic_pointer_cast<Symbol>(st);
           }},

          {Gram{Symbols::Assignment, Tokens::Identifier, Tokens::Assign,
                Symbols::ArithmeticExpression},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Assignment> assign(new struct Assignment);
             emitCode(
                 {":=",
                  dynamic_pointer_cast<struct Terminal>(syms[0])->token->lexeme,
                  "-",
                  dynamic_pointer_cast<struct ArithmeticExpression>(syms[2])
                      ->label});
             return dynamic_pointer_cast<Symbol>(assign);
           }},

          {Gram{Symbols::IfStatement, Tokens::IF, Symbols::BooleanExpression,
                Symbols::M, Tokens::THEN, Symbols::Statement},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct IfStatement> ifSt(new struct IfStatement);
             backPatch(dynamic_pointer_cast<struct BooleanExpression>(syms[1])
                           ->trueList,
                       dynamic_pointer_cast<struct M>(syms[2])->instr);
             ifSt->nextList =
                 dynamic_pointer_cast<struct BooleanExpression>(syms[1])
                     ->falseList;
             auto stNextList =
                 dynamic_pointer_cast<struct Statement>(syms[4])->nextList;
             ifSt->nextList.insert(ifSt->nextList.end(), stNextList.begin(),
                                   stNextList.end());
             return dynamic_pointer_cast<Symbol>(ifSt);
           }},

          {Gram{Symbols::N, Symbols::None},
           [this](const vector<shared_ptr<Symbol>> &) {
             shared_ptr<struct N> n(new struct N);
             n->instr = nextInstr();
             emitCode({"j", "-", "-", "-"});
             return dynamic_pointer_cast<Symbol>(n);
           }},

          {Gram{Symbols::IfStatement, Tokens::IF, Symbols::BooleanExpression,
                Symbols::M, Tokens::THEN, Symbols::Statement, Symbols::N,
                Tokens::ELSE, Symbols::Statement},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct IfStatement> ifSt(new struct IfStatement);
             auto b = dynamic_pointer_cast<struct BooleanExpression>(syms[1]);
             backPatch(b->trueList,
                       dynamic_pointer_cast<struct M>(syms[2])->instr);
             backPatch(b->falseList,
                       dynamic_pointer_cast<struct N>(syms[5])->instr);

             auto stNext1 =
                 dynamic_pointer_cast<struct Statement>(syms[4])->nextList;
             auto stNext2 =
                 dynamic_pointer_cast<struct Statement>(syms[7])->nextList;
             ifSt->nextList = stNext1;
             ifSt->nextList.push_back(
                 dynamic_pointer_cast<struct N>(syms[5])->instr);
             ifSt->nextList.insert(ifSt->nextList.end(), stNext2.begin(),
                                   stNext2.end());
             return dynamic_pointer_cast<Symbol>(ifSt);
           }},

          {Gram{Symbols::WhileStatement, Symbols::M, Tokens::WHILE,
                Symbols::BooleanExpression, Symbols::M, Tokens::DO,
                Symbols::Statement},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct WhileStatement> whSt(new struct WhileStatement);
             auto m1Addr = dynamic_pointer_cast<struct M>(syms[0])->instr;
             auto b = dynamic_pointer_cast<struct BooleanExpression>(syms[2]);
             backPatch(
                 dynamic_pointer_cast<struct Statement>(syms[5])->nextList,
                 m1Addr);
             backPatch(b->trueList,
                       dynamic_pointer_cast<struct M>(syms[3])->instr);

             whSt->nextList = b->falseList;
             emitCode({"j", "-", "-", std::to_string(m1Addr)});
             return dynamic_pointer_cast<Symbol>(whSt);
           }},

          {Gram{Symbols::RepeatStatement, Symbols::M, Tokens::REPEAT,
                Symbols::Statement, Symbols::M, Tokens::UNTIL,
                Symbols::BooleanExpression},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct RepeatStatement> reap(
                 new struct RepeatStatement);
             auto b = dynamic_pointer_cast<struct BooleanExpression>(syms[5]);
             backPatch(
                 dynamic_pointer_cast<struct Statement>(syms[2])->nextList,
                 dynamic_pointer_cast<struct M>(syms[3])->instr);
             backPatch(b->falseList,
                       dynamic_pointer_cast<struct M>(syms[0])->instr);
             reap->nextList = b->trueList;
             return dynamic_pointer_cast<Symbol>(reap);
           }},

          {Gram{Symbols::CompoundStatement, Tokens::BEGIN,
                Symbols::StatementTable, Tokens::END},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct CompoundStatement> st(
                 new struct CompoundStatement);
             return dynamic_pointer_cast<Symbol>(st);
           }},

          {Gram{Symbols::StatementTable, Symbols::Statement, Symbols::M,
                Tokens::Semicolon, Symbols::StatementTable},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct StatementTable> st(new struct StatementTable);
             backPatch(
                 dynamic_pointer_cast<struct Statement>(syms[0])->nextList,
                 dynamic_pointer_cast<struct M>(syms[1])->instr);
             return dynamic_pointer_cast<Symbol>(st);
           }},

          {Gram{Symbols::StatementTable, Symbols::Statement},
           [this](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct StatementTable> st(new struct StatementTable);
             backPatch(
                 dynamic_pointer_cast<struct Statement>(syms[0])->nextList,
                 nextInstr());
             return dynamic_pointer_cast<Symbol>(st);
           }},

          {Gram{Symbols::Program, Tokens::PROGRAM, Tokens::Identifier,
                Tokens::Semicolon, Symbols::VariablePreamble,
                Symbols::CompoundStatement},
           [this, parser](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Program> prog(new struct Program);
             parser->code[0] = {
                 "program",
                 dynamic_pointer_cast<struct Terminal>(syms[1])->token->lexeme,
                 "-", "-"};
             emitCode({"sys", "-", "-", "-"});
             return dynamic_pointer_cast<Symbol>(prog);
           }},

          {Gram{Symbols::VariablePreamble, Tokens::VAR,
                Symbols::VariableDeclaration},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct VariablePreamble> preamble(
                 new struct VariablePreamble);
             return dynamic_pointer_cast<Symbol>(preamble);
           }},

          {Gram{Symbols::VariableDeclaration, Symbols::IdentifierTable,
                Tokens::Colon, Symbols::Type, Tokens::Semicolon,
                Symbols::VariableDeclaration},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct VariableDeclaration> declar(
                 new struct VariableDeclaration);
             return dynamic_pointer_cast<Symbol>(declar);
           }},

          {Gram{Symbols::VariableDeclaration, Symbols::IdentifierTable,
                Tokens::Colon, Symbols::Type, Tokens::Semicolon},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct VariableDeclaration> declar(
                 new struct VariableDeclaration);
             return dynamic_pointer_cast<Symbol>(declar);
           }},

          {Gram{Symbols::VariablePreamble, Symbols::None},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct VariablePreamble> preamble(
                 new struct VariablePreamble);
             return dynamic_pointer_cast<Symbol>(preamble);
           }},

          {Gram{Symbols::IdentifierTable, Tokens::Identifier,
                Symbols::IdentifierTable},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct IdentifierTable> table(
                 new struct IdentifierTable);
             return dynamic_pointer_cast<Symbol>(table);
           }},

          {Gram{Symbols::IdentifierTable, Tokens::Identifier},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct IdentifierTable> table(
                 new struct IdentifierTable);
             return dynamic_pointer_cast<Symbol>(table);
           }},

          {Gram{Symbols::Type, Tokens::INTEGER},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Type> tp(new struct Type);
             return dynamic_pointer_cast<Symbol>(tp);
           }},

          {Gram{Symbols::Type, Tokens::BOOL},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Type> tp(new struct Type);
             return dynamic_pointer_cast<Symbol>(tp);
           }},

          {Gram{Symbols::Type, Tokens::CHAR},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct Type> tp(new struct Type);
             return dynamic_pointer_cast<Symbol>(tp);
           }},

          {Gram{Symbols::StartSymbol, Symbols::Program},
           [](const vector<shared_ptr<Symbol>> &syms) {
             shared_ptr<struct StartSymbol> start(new struct StartSymbol);
             return dynamic_pointer_cast<Symbol>(start);
           }},
      }) {

  productions.resize(SYMBOLS_CNT);
  for (auto &pir : sementicActions) {
    productions.at(pir.first.gram[0] - TOKEN_CNT).push_back(&pir.first.gram);
  }
}

const vector<const Gram *> &GramDef::getProduction(Symbols sym) const {
  return productions.at(sym - TOKEN_CNT);
}

SementicAction GramDef::getSementicAction(const Gram &gram) const {
  return sementicActions.find(gram)->second;
}

void GramDef::emitCode(std::array<std::string, 4> code) {
  parser->code.push_back(code);
}

size_t GramDef::nextInstr() const { return parser->code.size(); }

void GramDef::backPatch(vector<size_t> &list, size_t instr) {
  for (auto tar : list) {
    parser->code[tar][3] = std::to_string(instr);
  }
}

string GramDef::newTemp() { return "T" + std::to_string(++tempLabelCnt); }

bool operator==(const Gram &lhs, const Gram &rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }

  for (size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }

  return true;
}

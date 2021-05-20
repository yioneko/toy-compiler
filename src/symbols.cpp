#include "symbols.hpp"

Terminal::Terminal(Token *const &token) : token(token) {}

// Too ugly! Perhaps someone who could provide
// elegant scheme to override static field in the
// superclass.
Symbols Terminal::getType() { return symbolType; };
Symbols Expression::getType() { return symbolType; };
Symbols ArithmeticExpression::getType() { return symbolType; };
Symbols BooleanExpression::getType() { return symbolType; };
Symbols CharExpression::getType() { return symbolType; };
Symbols Item::getType() { return symbolType; };
Symbols Factor::getType() { return symbolType; };
Symbols ArithmeticUnit::getType() { return symbolType; };
Symbols BooleanItem::getType() { return symbolType; };
Symbols BooleanFactor::getType() { return symbolType; };
Symbols BooleanUnit::getType() { return symbolType; };
Symbols RelationOperator::getType() { return symbolType; };
Symbols Statement::getType() { return symbolType; };
Symbols Assignment::getType() { return symbolType; };
Symbols IfStatement::getType() { return symbolType; };
Symbols WhileStatement::getType() { return symbolType; };
Symbols RepeatStatement::getType() { return symbolType; };
Symbols CompoundStatement::getType() { return symbolType; };
Symbols StatementTable::getType() { return symbolType; };
Symbols Program::getType() { return symbolType; };
Symbols VariablePreamble::getType() { return symbolType; };
Symbols VariableDeclaration::getType() { return symbolType; };
Symbols Type::getType() { return symbolType; };
Symbols IdentifierTable::getType() { return symbolType; };
Symbols M::getType() { return symbolType; };
Symbols N::getType() { return symbolType; };
Symbols StartSymbol::getType() { return symbolType; };

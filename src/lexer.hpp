#ifndef LEXER_H
#define LEXER_H
#include "tokens.hpp"
#include <map>
#include <vector>

class Lexer {
private:
  std::map<std::string, unsigned> symbolTable;
  std::string::const_iterator peek;
  const std::string &progText;
  unsigned curLine;
  unsigned curCol;

  void skipComment();
  void skipSpace(bool skipEol);
  void throwLexerError(const std::string &errorType,
                       const std::string::const_iterator &pos, unsigned line,
                       unsigned col) const;

  typedef std::pair<std::smatch, Tokens> matchResult;
  matchResult tryMatchToken();

public:
  explicit Lexer(const std::string &progText);

  int getSymbolId(const std::string &symbol) const;
  void lexer(std::vector<Token> &parsedTokens);
};

#endif

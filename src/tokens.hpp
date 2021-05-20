#ifndef TOKENS_H
#define TOKENS_H
#include <regex>
#include <string>
#include <vector>

enum Tokens : unsigned {
  AND,
  ARRAY,
  BEGIN,
  BOOL,
  CALL,
  CASE,
  CHAR,
  CONSTANT,
  DIM,
  DO,
  ELSE,
  END,
  FALSE,
  FOR,
  IF,
  INPUT,
  INTEGER,
  NOT,
  OF,
  OR,
  OUTPUT,
  PROCEDURE,
  PROGRAM,
  READ,
  REAL,
  REPEAT,
  SET,
  STOP,
  THEN,
  TO,
  TRUE,
  UNTIL,
  VAR,
  WHILE,
  WRITE,
  Identifier,
  IntLIteral,
  CharLiteral,
  LeftBracket,
  RightBracket,
  Asterisk,
  RightBlockComment,
  Plus,
  Comma,
  Minus,
  Dot,
  DoubleDots,
  Slash,
  LeftBlockComment,
  Colon,
  Assign,
  Semicolon,
  Less,
  LessEqual,
  NotEqual,
  Equal,
  Larger,
  LargerEqual,
  LeftSquare,
  RightSquare,
  EndInd,
  TOKEN_CNT
};

extern const std::vector<std::string> tokenRegexps;
extern const std::string legalCharsRegexp;

struct Token {
  Tokens type;
  std::string lexeme;
  unsigned line;
  unsigned col;
  Token(Tokens type, const std::string &lexeme, const unsigned line,
        const unsigned col);
};

#endif

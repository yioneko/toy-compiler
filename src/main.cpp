#include "lexer.hpp"
#include <cstdio>

void readProgtext(std::string &progText) {
  char c;
  while ((c = getchar()) != EOF) {
    progText += c;
  }
}

void printTokens(Lexer &lexer, const std::vector<Token> &TokensVec) {
  const unsigned tokenColumns = 5;
  const unsigned maxColumnWidth = 12;

  unsigned curColumn = 0;
  char curToken[maxColumnWidth + 1];
  for (auto &token : TokensVec) {
    const int symbolId = lexer.getSymbolId(token.lexeme);
    if (symbolId == -1) {
      sprintf(curToken, "(%d, -)  ", token.type + 1);
    } else {
      sprintf(curToken, "(%d, %d)  ", token.type + 1, symbolId + 1);
    }

    printf("%-*s", maxColumnWidth, curToken);
    if ((++curColumn) % tokenColumns == 0) {
      putchar('\n');
    }
  }
}

void compile() {
  std::vector<Token> parsedTokens;
  std::string progText;

  readProgtext(progText);

  Lexer lexer(progText);
  lexer.lexer(parsedTokens);
  printf("Parsed tokens: \n");
  printTokens(lexer, parsedTokens);

  exit(0);
}

int main() {
  printf("============Welcome to toy compiler for SAMPLE============\n");
  compile();
}

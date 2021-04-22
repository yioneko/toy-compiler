#include "lexer.hpp"
#include <cstdio>

void readProgtext(std::string &progText) {
  char c;
  while ((c = getchar()) != EOF) {
    progText += c;
  }
}

void printTokens(const Lexer &lexer, const std::vector<Token> &TokensVec) {
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
  printf("Program file name(path): ");

  // length of file name can be at most 999
  char fileName[1000];
  scanf("%999s", fileName);
  while (!freopen(fileName, "r", stdin)) {
    printf("Failed to read file! Input correct file name again: ");
  }

  compile();
}

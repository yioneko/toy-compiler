#include "lexer.hpp"
#include "parser.hpp"
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <streambuf>

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
  putchar('\n');
}

void compile(const std::string &progText) {
  std::vector<Token> parsedTokens;

  Lexer lexer(progText);
  lexer.lexer(parsedTokens);
  printf("Parsed tokens: \n");
  printTokens(lexer, parsedTokens);
  putchar('\n');

  Parser parser(parsedTokens);
  try {
    parser.parse();
  } catch (const std::exception &e) {
    puts("Error occurred during syntax analysis: ");
    printf("%s\n", e.what());
  }
  printf("Generated intermediate code: \n");
  parser.printCode();
}

int main() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  system("chcp 65001 >nul");
#endif
  printf("============Welcome to toy compiler for SAMPLE============\n");
  printf("Program file name(path): ");

  // length of file name can be at most 999
  char fileName[1000];
  scanf("%999s", fileName);
  // freopen("../tmp", "w", stdout);
  if (!fopen(fileName, "r")) {
    printf("Failed to read file!");
    return 1;
  }
  std::ifstream file(fileName);
  std::string progText((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

  compile(progText);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  system("pause");
#endif
}

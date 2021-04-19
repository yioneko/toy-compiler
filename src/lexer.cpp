#include "lexer.hpp"
#include <cctype>
#include <cstdio>
#include <stdexcept>

Lexer::Lexer(const std::string &progText) : progText(progText) {
  this->peek = this->progText.begin();
  this->curLine = 1;
  this->curCol = 1;
}

void Lexer::skipSpace() {
  while (isspace(*peek)) {
    if (*peek == '\n') {
      ++curLine;
      curCol = 1;
    } else {
      ++curCol;
    }
    ++peek;
  }
}

void Lexer::skipComment() {
  std::regex rightCommentRegex(tokenRegexps[Tokens::RightBlockComment]);

  while (!std::regex_search(peek, progText.end(), rightCommentRegex)) {
    skipSpace();
    if (peek == progText.end() || (++peek == progText.end())) {
      return;
    }
  }

  // length of right block comment sign
  peek += 2;
  curCol += 2;
}

Lexer::matchResult Lexer::tryMatchToken() {
  std::smatch candidateMatch;
  Tokens candidateToken;
  for (unsigned tokenIndex = 0; tokenIndex < Tokens::TOKEN_CNT; ++tokenIndex) {
    const std::string &regStr = tokenRegexps[tokenIndex];

    std::smatch match;
    std::regex_search(peek, progText.end(), match, std::regex(regStr));
    if (candidateMatch.empty() ||
        match[0].length() > candidateMatch[0].length()) {
      candidateMatch = match;
      candidateToken = Tokens(tokenIndex);
    }
  }

  // error
  if (candidateMatch.empty()) {
    const unsigned maxExtraChars = 5;
    unsigned actualBackwardChars;
    auto bacIter = peek, forIter = peek + 1;

    // search backward
    for (actualBackwardChars = 0; actualBackwardChars <= maxExtraChars;
         ++actualBackwardChars, --bacIter) {
      if (bacIter == progText.begin() || isspace(*(bacIter - 1)))
        break;
    }

    // search forward
    for (unsigned forwardChars = 0;
         forIter != progText.end() && forwardChars <= maxExtraChars &&
         !isspace(*forIter);
         ++forwardChars, ++forIter)
      ;

    const std::string strBeforeWrongText = "No candidate token match for ";
    throw std::runtime_error(
        strBeforeWrongText + std::string(bacIter, forIter) + "\n" +
        std::string(strBeforeWrongText.length() + actualBackwardChars, ' ') +
        "^ at line " + std::to_string(curLine) + ", col " +
        std::to_string(curCol) + '.');
  }
  return std::make_pair(candidateMatch, candidateToken);
}

void Lexer::lexer(std::vector<Token> &parsedTokens) {
  // skip space
  bool errorOccurred = false;
  while (peek != progText.end()) {
    skipSpace();

    if (peek == progText.end())
      break;

    matchResult ret;
    try {
      ret = tryMatchToken();
    } catch (const std::exception &e) {
      if (!errorOccurred) {
        errorOccurred = true;
        printf("Error occurred during lexical analysis: \n");
      }
      printf("%s\n", e.what());
      ++peek;
      ++curCol;
      continue;
    }

    std::smatch candidateMatch = ret.first;
    Tokens candidateToken = ret.second;

    if (candidateToken == Tokens::LeftBlockComment) {
      skipComment();
      continue;
    }

    const std::string lexeme = std::string(peek, candidateMatch[0].second);
    if (candidateToken == Tokens::Identifier ||
        candidateToken == Tokens::IntLIteral ||
        candidateToken == Tokens::CharLiteral) {
      if (!symbolTable.count(lexeme)) {
        symbolTable[lexeme] = symbolTable.size();
      }
    }

    parsedTokens.emplace_back(candidateToken, lexeme, curLine, curCol);
    curCol += candidateMatch[0].length();
    peek = candidateMatch[0].second;
  }
}

int Lexer::getSymbolId(const std::string &symbol) const {
  if (!symbolTable.count(symbol)) {
    return -1;
  }

  return symbolTable.find(symbol)->second;
}

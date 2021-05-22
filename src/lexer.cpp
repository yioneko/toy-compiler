#include "lexer.hpp"
#include <cctype>
#include <cstdio>
#include <stdexcept>

Lexer::Lexer(const std::string &progText)
    : progText(progText), curLine(1), curCol(1) {
  this->peek = this->progText.begin();
}

void Lexer::skipSpace(bool skipEol = true) {
  while (isspace(*peek)) {
    if (*peek == '\n') {
      if (!skipEol) {
        return;
      }

      ++curLine;
      curCol = 1;
    } else {
      ++curCol;
    }
    if (++peek == progText.end())
      return;
  }
}

void Lexer::skipComment() {
  const std::string::const_iterator leftCommentIter = peek;
  const unsigned leftCommentCol = curCol;
  peek += 2;
  curCol += 2;
  std::regex rightCommentRegex(tokenRegexps[RightBlockComment]);

  while (!std::regex_search(peek, progText.end(), rightCommentRegex,
                            std::regex_constants::match_continuous)) {
    // do not skip '\n'
    const auto prePeek = peek;
    skipSpace(false);
    if ((*peek) == '\n' || peek == progText.end() ||
        (prePeek == peek && ++peek == progText.end())) {
      throwLexerError("Unexpected unclosed comment", leftCommentIter, curLine,
                      leftCommentCol);
    }
  }

  // length of right block comment sign
  peek += 2;
  curCol += 2;
}

void Lexer::throwLexerError(const std::string &errorType,
                            const std::string::const_iterator &pos,
                            unsigned line, unsigned col) const {
  const unsigned maxExtraChars = 6;
  unsigned actualBackwardChars;
  auto bacIter = pos, forIter = pos + 1;

  // search backward
  for (actualBackwardChars = 0;
       actualBackwardChars <= maxExtraChars && bacIter != progText.begin() &&
       *(bacIter - 1) != '\n';
       ++actualBackwardChars, --bacIter)
    ;

  // search forward
  for (unsigned forwardChars = 0;
       forIter != progText.end() && forwardChars <= maxExtraChars &&
       (*forIter) != '\n' && (*forIter) != '\r'; // compatible for windows
       ++forwardChars, ++forIter)
    ;

  throw std::runtime_error(
      errorType + ": " + std::string(bacIter, forIter) + "\n" +
      std::string(errorType.length() + actualBackwardChars + 2, ' ') +
      "^ at line " + std::to_string(line) + ", col " + std::to_string(col) +
      '.');
};

Lexer::matchResult Lexer::tryMatchToken() {
  if (!std::regex_search(peek, progText.end(), std::regex(legalCharsRegexp),
                         std::regex_constants::match_continuous)) {
    throwLexerError("Detect illegal character", peek++, curLine, curCol++);
  }

  std::smatch candidateMatch;
  Tokens candidateToken;
  for (unsigned tokenIndex = 0; tokenIndex < TOKEN_CNT - 2; ++tokenIndex) {
    const std::string &regStr = tokenRegexps[tokenIndex];

    std::smatch match;
    std::regex_search(peek, progText.end(), match, std::regex(regStr),
                      std::regex_constants::match_continuous);
    if (candidateMatch.empty() ||
        match[0].length() > candidateMatch[0].length()) {
      candidateMatch = match;
      candidateToken = Tokens(tokenIndex);
    }
  }

  // error
  if (candidateMatch.empty()) {
    if ((*(peek - 1)) == '\'') {
      throwLexerError("Unexpected unclosed string literal", peek++, curLine,
                      curCol++);
    }

    throwLexerError("No candidate token match", peek++, curLine, curCol++);
  }
  return std::make_pair(candidateMatch, candidateToken);
}

void Lexer::lexer(std::vector<Token> &parsedTokens) {
  bool errorOccurred = false;
  while (peek != progText.end()) {
    skipSpace();

    if (peek == progText.end())
      break;

    std::smatch candidateMatch;
    Tokens candidateToken;
    try {
      matchResult ret = tryMatchToken();

      candidateMatch = ret.first;
      candidateToken = ret.second;

      if (candidateToken == LeftBlockComment) {
        skipComment();
        continue;
      }
    } catch (const std::exception &e) {
      if (!errorOccurred) {
        errorOccurred = true;
        printf("Error occurred during lexical analysis: \n");
      }
      printf("%s\n", e.what());
      continue;
    }

    const std::string lexeme = std::string(peek, candidateMatch[0].second);
    if (candidateToken == Identifier || candidateToken == IntLIteral ||
        candidateToken == CharLiteral) {
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

#ifndef GRAM_H
#define GRAM_H

#include "symbols.hpp"
#include <array>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

using std::shared_ptr;
using std::vector;

typedef vector<symbol_t> Gram;
bool operator==(const Gram &lhs, const Gram &rhs);

typedef vector<vector<const Gram *>> Productions;
typedef std::function<shared_ptr<Symbol>(const vector<shared_ptr<Symbol>> &)>
    SementicAction;

class Parser;

class GramDef {
private:
  struct GramWithHash {
    Gram gram;
    GramWithHash(const Gram &gram) : gram(gram) {}

    bool operator==(const GramWithHash &rhs) const { return gram == rhs.gram; }

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

  std::unordered_map<GramWithHash, SementicAction, GramWithHash::Hash>
      sementicActions;

  Productions productions;
  Parser *parser;

  void emitCode(std::array<std::string, 4> code);
  size_t nextInstr() const;
  void backPatch(vector<size_t> &list, size_t instr);

  unsigned tempLabelCnt;
  string newTemp();

public:
  GramDef(Parser *parser);
  SementicAction getSementicAction(const Gram &gram) const;
  const std::vector<const Gram *> &getProduction(Symbols sym) const;
};

#endif

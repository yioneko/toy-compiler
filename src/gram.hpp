#ifndef GRAM_H
#define GRAM_H

#include "symbols.hpp"
#include <vector>

typedef std::vector<unsigned> Gram;
typedef void (*SementicAction)(const std::initializer_list<Symbol> &);

SementicAction getSementicAction(const Gram &gram);
const std::vector<const Gram *> getProduction(Symbols sym);

#endif

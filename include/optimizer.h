#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "tac.h"

class Optimizer {
public:
    TACProgram optimize(const TACProgram& input);
};

#endif

#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include "ast.h"
#include "tac.h"

class TACGenerator {
public:
    TACProgram generate(const ASTNode& program);
};

#endif

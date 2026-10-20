#ifndef CODEGEN_H
#define CODEGEN_H

#include <string>
#include "tac.h"

class CCodeGenerator {
public:
    std::string generate(const TACProgram& program);
};

#endif

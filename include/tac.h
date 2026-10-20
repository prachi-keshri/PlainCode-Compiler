#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>

enum class TACOp {
    Assign,
    Add,
    Subtract,
    Multiply,
    Divide,
    Print
};

struct TACInstruction {
    TACOp op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

using TACProgram = std::vector<TACInstruction>;

#endif

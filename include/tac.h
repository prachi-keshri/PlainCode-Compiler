#ifndef TAC_H
#define TAC_H
#include <string>
#include <utility>
#include <vector>
enum class TACOp {
    Assign,
    Add,
    Subtract,
    Multiply,
    Divide,
    Print,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    Equal,
    NotEqual,
    Label,
    Jump,
    JumpIfZero,
    FunctionBegin,
    FunctionEnd,
    Return,
    Call
};
struct TACInstruction {
    TACOp op;
    std::string arg1;
    std::string arg2;
    std::string result;
    std::vector<std::string> args;
    TACInstruction(TACOp operation, std::string first = "", std::string second = "",
                   std::string destination = "")
        : op(operation), arg1(std::move(first)), arg2(std::move(second)),
          result(std::move(destination)) {}
};
using TACProgram = std::vector<TACInstruction>;
#endif


#include "codegen.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    TACProgram program = {
        {TACOp::Assign, "10", "", "a"},
        {TACOp::Assign, "20", "", "b"},
        {TACOp::Multiply, "b", "2", "__pc_tmp1"},
        {TACOp::Add, "a", "__pc_tmp1", "result"},
        {TACOp::Print, "result", "", ""}
    };

    CCodeGenerator generator;
    std::string code = generator.generate(program);

    assert(code.find("#include <stdio.h>") != std::string::npos);
    assert(code.find("a = 10;") != std::string::npos);
    assert(code.find("__pc_tmp1 = b * 2;") != std::string::npos);
    assert(code.find("result = a + __pc_tmp1;") != std::string::npos);
    assert(code.find("printf(\"%d\\n\", result);") != std::string::npos);

    TACProgram divisionProgram = {
        {TACOp::Divide, "10", "2", "answer"},
        {TACOp::Print, "answer", "", ""}
    };

    const std::string divisionCode =
        generator.generate(divisionProgram);

    assert(divisionCode.find("if (2 == 0)") != std::string::npos);

    bool rejected = false;

    try {
        TACProgram invalidProgram = {
            {TACOp::Assign, "10", "", "bad-name"}
        };
        generator.generate(invalidProgram);
    } catch (const std::runtime_error&) {
        rejected = true;
    }

    assert(rejected);

    std::cout << "C code generator tests passed!\n";
    return 0;
}

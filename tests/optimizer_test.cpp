#include "optimizer.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

void testConstantFolding() {
    TACProgram input = {
        {TACOp::Add, "5", "3", "__pc_tmp1"},
        {TACOp::Print, "__pc_tmp1", "", ""}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output.size() == 1);
    assert(output[0].op == TACOp::Print);
    assert(output[0].arg1 == "8");

    std::cout << "[PASS] Constant folding\n";
}

void testConstantPropagation() {
    TACProgram input = {
        {TACOp::Assign, "10", "", "a"},
        {TACOp::Add, "a", "5", "b"},
        {TACOp::Print, "b", "", ""}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output.size() == 3);
    assert(output[1].op == TACOp::Assign);
    assert(output[1].arg1 == "15");
    assert(output[2].arg1 == "15");

    std::cout << "[PASS] Constant propagation\n";
}

void testReassignment() {
    TACProgram input = {
        {TACOp::Assign, "10", "", "a"},
        {TACOp::Assign, "20", "", "a"},
        {TACOp::Print, "a", "", ""}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output[2].arg1 == "20");

    std::cout << "[PASS] Reassignment\n";
}

void testDivisionByZero() {
    TACProgram input = {
        {TACOp::Divide, "10", "0", "a"}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output.size() == 1);
    assert(output[0].op == TACOp::Divide);
    assert(output[0].arg1 == "10");
    assert(output[0].arg2 == "0");

    std::cout << "[PASS] Division by zero not folded\n";
}

void testOverflow() {
    const std::string maxValue =
        std::to_string(std::numeric_limits<std::int64_t>::max());

    TACProgram input = {
        {TACOp::Add, maxValue, "1", "a"}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output.size() == 1);
    assert(output[0].op == TACOp::Add);

    std::cout << "[PASS] Overflow not folded\n";
}

void testNonConstantExpression() {
    TACProgram input = {
        {TACOp::Add, "x", "5", "a"}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output.size() == 1);
    assert(output[0].op == TACOp::Add);
    assert(output[0].arg1 == "x");
    assert(output[0].arg2 == "5");

    std::cout << "[PASS] Non-constant expression preserved\n";
}

void testSubtractionAndMultiplication() {
    TACProgram input = {
        {TACOp::Subtract, "10", "4", "a"},
        {TACOp::Multiply, "a", "3", "b"}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output[0].op == TACOp::Assign);
    assert(output[0].arg1 == "6");
    assert(output[1].op == TACOp::Assign);
    assert(output[1].arg1 == "18");

    std::cout << "[PASS] Subtraction and multiplication\n";
}

void testDeadTemporaryElimination() {
    TACProgram input = {
        {TACOp::Add, "5", "3", "__pc_tmp1"},
        {TACOp::Multiply, "2", "4", "__pc_tmp2"},
        {TACOp::Print, "__pc_tmp2", "", ""}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    // Both temporary assignments become unnecessary
    // after constant propagation.
    assert(output.size() == 1);
    assert(output[0].op == TACOp::Print);
    assert(output[0].arg1 == "8");

    std::cout << "[PASS] Dead temporary elimination\n";
}

void testUnusedDivisionPreserved() {
    TACProgram input = {
        {TACOp::Divide, "10", "0", "__pc_tmp1"},
        {TACOp::Print, "5", "", ""}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output.size() == 2);
    assert(output[0].op == TACOp::Divide);
    assert(output[0].arg1 == "10");
    assert(output[0].arg2 == "0");
    assert(output[1].op == TACOp::Print);

    std::cout << "[PASS] Unused division preserved\n";
}

void testVariableReassignmentAndMultipleUses() {
    TACProgram input = {
        {TACOp::Assign, "10", "", "a"},
        {TACOp::Print, "a", "", ""},
        {TACOp::Print, "a", "", ""},
        {TACOp::Assign, "20", "", "a"},
        {TACOp::Print, "a", "", ""}
    };

    Optimizer optimizer;
    TACProgram output = optimizer.optimize(input);

    assert(output.size() == 5);
    assert(output[1].op == TACOp::Print);
    assert(output[1].arg1 == "10");
    assert(output[2].arg1 == "10");
    assert(output[4].arg1 == "20");

    std::cout << "[PASS] Reassignment and multiple uses\n";
}

int main() {
    testConstantFolding();
    testConstantPropagation();
    testReassignment();
    testDivisionByZero();
    testOverflow();
    testNonConstantExpression();
    testSubtractionAndMultiplication();
    testDeadTemporaryElimination();
    testUnusedDivisionPreserved();
    testVariableReassignmentAndMultipleUses();

    std::cout << "\nAll 10 optimizer tests passed!\n";
    return 0;
}

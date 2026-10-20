#include "ast.h"
#include "tac_generator.h"

#include <cassert>
#include <iostream>
#include <memory>

int main() {
    ASTNode program("Program");

    // SET a TO 10
    auto assignmentA = std::make_unique<ASTNode>("Assignment", "a");
    assignmentA->children.push_back(
        std::make_unique<ASTNode>("Number", "10"));
    program.children.push_back(std::move(assignmentA));

    // SET b TO 20
    auto assignmentB = std::make_unique<ASTNode>("Assignment", "b");
    assignmentB->children.push_back(
        std::make_unique<ASTNode>("Number", "20"));
    program.children.push_back(std::move(assignmentB));

    // SET result TO a + b * 2
    auto assignmentResult =
        std::make_unique<ASTNode>("Assignment", "result");

    auto add = std::make_unique<ASTNode>("Add");
    add->children.push_back(
        std::make_unique<ASTNode>("Identifier", "a"));

    auto multiply = std::make_unique<ASTNode>("Multiply");
    multiply->children.push_back(
        std::make_unique<ASTNode>("Identifier", "b"));
    multiply->children.push_back(
        std::make_unique<ASTNode>("Number", "2"));

    add->children.push_back(std::move(multiply));
    assignmentResult->children.push_back(std::move(add));
    program.children.push_back(std::move(assignmentResult));

    // DISPLAY result
    auto display = std::make_unique<ASTNode>("Display");
    display->children.push_back(
        std::make_unique<ASTNode>("Identifier", "result"));
    program.children.push_back(std::move(display));

    TACGenerator generator;
    TACProgram tac = generator.generate(program);

    assert(tac.size() == 6);
    assert(tac[0].op == TACOp::Assign);
    assert(tac[1].op == TACOp::Assign);
    assert(tac[2].op == TACOp::Multiply);
    assert(tac[3].op == TACOp::Add);
    assert(tac[4].op == TACOp::Assign);
    assert(tac[5].op == TACOp::Print);

    std::cout << "TAC generator tests passed!\n";
    return 0;
}
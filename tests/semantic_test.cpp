#include "semantic.h"

#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

using Node = std::unique_ptr<ASTNode>;

Node makeNode(const std::string& type,
              const std::string& value = "") {
    return std::make_unique<ASTNode>(type, value);
}

Node makeAssignment(const std::string& name, Node expression) {
    auto node = makeNode("Assignment", name);
    node->children.push_back(std::move(expression));
    return node;
}

Node makeDisplay(Node expression) {
    auto node = makeNode("Display");
    node->children.push_back(std::move(expression));
    return node;
}

Node makeBinary(const std::string& op, Node left, Node right) {
    auto node = makeNode(op);
    node->children.push_back(std::move(left));
    node->children.push_back(std::move(right));
    return node;
}

void expectValid(const std::string& name,
                 const std::function<void(ASTNode&)>& build) {
    ASTNode program("Program");
    build(program);

    SemanticAnalyzer analyzer;
    analyzer.analyze(program);

    std::cout << "[PASS] " << name << '\n';
}

void expectError(const std::string& name,
                 const std::string& expected,
                 const std::function<void(ASTNode&)>& build) {
    ASTNode program("Program");
    build(program);

    SemanticAnalyzer analyzer;

    try {
        analyzer.analyze(program);
    } catch (const std::runtime_error& error) {
        if (std::string(error.what()).find(expected) !=
            std::string::npos) {
            std::cout << "[PASS] " << name << '\n';
            return;
        }
        throw;
    }

    throw std::runtime_error(
        "Expected semantic error for: " + name);
}

int main() {
    try {
        expectValid("Simple assignment and display", [](ASTNode& p) {
            p.children.push_back(
                makeAssignment("a", makeNode("Number", "10")));
            p.children.push_back(
                makeDisplay(makeNode("Identifier", "a")));
        });

        expectValid("Reassignment", [](ASTNode& p) {
            p.children.push_back(
                makeAssignment("a", makeNode("Number", "10")));
            p.children.push_back(
                makeAssignment(
                    "a",
                    makeBinary("Add",
                        makeNode("Identifier", "a"),
                        makeNode("Number", "1"))));
        });

        expectValid("Nested arithmetic", [](ASTNode& p) {
            p.children.push_back(
                makeAssignment("a", makeNode("Number", "5")));
            p.children.push_back(
                makeDisplay(
                    makeBinary("Multiply",
                        makeBinary("Add",
                            makeNode("Identifier", "a"),
                            makeNode("Number", "2")),
                        makeNode("Number", "3"))));
        });

        expectError("Undefined variable",
                    "undefined variable 'x'", [](ASTNode& p) {
            p.children.push_back(
                makeDisplay(makeNode("Identifier", "x")));
        });

        expectError("Self-reference before assignment",
                    "undefined variable 'a'", [](ASTNode& p) {
            p.children.push_back(
                makeAssignment(
                    "a",
                    makeBinary("Add",
                        makeNode("Identifier", "a"),
                        makeNode("Number", "1"))));
        });

        expectError("Undefined variable in arithmetic",
                    "undefined variable 'b'", [](ASTNode& p) {
            p.children.push_back(
                makeAssignment(
                    "a",
                    makeBinary("Add",
                        makeNode("Number", "10"),
                        makeNode("Identifier", "b"))));
        });

        expectError("Reserved assignment name",
                    "reserved identifier", [](ASTNode& p) {
            p.children.push_back(
                makeAssignment(
                    "__pc_tmp1", makeNode("Number", "5")));
        });

        expectError("Reserved identifier in expression",
                    "reserved identifier", [](ASTNode& p) {
            p.children.push_back(
                makeDisplay(makeNode("Identifier", "__pc_tmp1")));
        });

        std::cout << "\nAll 8 semantic tests passed!\n";
        return 0;

    } catch (const std::exception& error) {
        std::cerr << "[FAIL] " << error.what() << '\n';
        return 1;
    }
}

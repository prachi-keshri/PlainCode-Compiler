#include "semantic.h"

#include <stdexcept>
#include <string>
#include <unordered_set>

namespace {

void checkExpression(
    const ASTNode& node,
    const std::unordered_set<std::string>& symbols) {

    if (node.type == "Number") {
        return;
    }

    if (node.type == "Identifier") {
        if (node.value.rfind("__pc_", 0) == 0) {
            throw std::runtime_error(
                "Semantic error: reserved identifier '" +
                node.value + "'");
        }

        if (symbols.find(node.value) == symbols.end()) {
            throw std::runtime_error(
                "Semantic error: undefined variable '" +
                node.value + "'");
        }
        return;
    }

    if (node.type == "Add" ||
        node.type == "Subtract" ||
        node.type == "Multiply" ||
        node.type == "Divide") {

        if (node.children.size() != 2) {
            throw std::runtime_error(
                "Semantic error: invalid binary expression");
        }

        checkExpression(*node.children[0], symbols);
        checkExpression(*node.children[1], symbols);
        return;
    }

    throw std::runtime_error(
        "Semantic error: unexpected expression node '" +
        node.type + "'");
}

} // namespace

void SemanticAnalyzer::analyze(const ASTNode& program) {
    if (program.type != "Program") {
        throw std::runtime_error(
            "Semantic error: expected Program node");
    }

    std::unordered_set<std::string> symbols;

    for (const auto& statement : program.children) {
        if (statement->type == "Assignment") {
            const std::string& name = statement->value;

            if (name.rfind("__pc_", 0) == 0) {
                throw std::runtime_error(
                    "Semantic error: reserved identifier '" +
                    name + "'");
            }

            if (statement->children.size() != 1) {
                throw std::runtime_error(
                    "Semantic error: invalid assignment");
            }

            // Check RHS before defining the variable.
            checkExpression(*statement->children[0], symbols);
            symbols.insert(name);
        }
        else if (statement->type == "Display") {
            if (statement->children.size() != 1) {
                throw std::runtime_error(
                    "Semantic error: invalid DISPLAY statement");
            }

            checkExpression(*statement->children[0], symbols);
        }
        else {
            throw std::runtime_error(
                "Semantic error: unexpected statement node '" +
                statement->type + "'");
        }
    }
}

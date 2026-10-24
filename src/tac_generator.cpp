
#include "tac_generator.h"

#include <stdexcept>
#include <string>

namespace {

class Generator {
public:
    TACProgram instructions;

    TACProgram run(const ASTNode& program) {
        if (program.type != "Program") {
            throw std::runtime_error(
                "TAC generation error: expected Program node");
        }

        for (const auto& statement : program.children) {
            if (!statement) {
                throw std::runtime_error(
                    "TAC generation error: null statement");
            }
            generateStatement(*statement);
        }

        return instructions;
    }

private:
    unsigned int tempCounter = 0;

    std::string newTemp() {
        return "__pc_tmp" + std::to_string(++tempCounter);
    }

    void requireChildren(const ASTNode& node, std::size_t count) {
        if (node.children.size() != count) {
            throw std::runtime_error(
                "TAC generation error: malformed " + node.type + " node");
        }

        for (const auto& child : node.children) {
            if (!child) {
                throw std::runtime_error(
                    "TAC generation error: null child in " + node.type);
            }
        }
    }

    void generateStatement(const ASTNode& node) {
        if (node.type == "Assignment") {
            requireChildren(node, 1);

            if (node.value.empty()) {
                throw std::runtime_error(
                    "TAC generation error: assignment has no target");
            }

            std::string value = generateExpression(*node.children[0]);
            instructions.push_back({TACOp::Assign, value, "", node.value});
        } else if (node.type == "Display") {
            requireChildren(node, 1);
            std::string value = generateExpression(*node.children[0]);
            instructions.push_back({TACOp::Print, value, "", ""});
        } else {
            throw std::runtime_error(
                "TAC generation error: unsupported statement '" +
                node.type + "'");
        }
    }

    std::string generateExpression(const ASTNode& node) {
        if (node.type == "Number" || node.type == "Identifier") {
            if (!node.children.empty() || node.value.empty()) {
                throw std::runtime_error(
                    "TAC generation error: malformed " + node.type + " node");
            }
            return node.value;
        }

        TACOp op;
        if (node.type == "Add") {
            op = TACOp::Add;
        } else if (node.type == "Subtract") {
            op = TACOp::Subtract;
        } else if (node.type == "Multiply") {
            op = TACOp::Multiply;
        } else if (node.type == "Divide") {
            op = TACOp::Divide;
        } else {
            throw std::runtime_error(
                "TAC generation error: unsupported expression '" +
                node.type + "'");
        }

        requireChildren(node, 2);

        std::string left = generateExpression(*node.children[0]);
        std::string right = generateExpression(*node.children[1]);
        std::string result = newTemp();

        instructions.push_back({op, left, right, result});
        return result;
    }
};

} // namespace

TACProgram TACGenerator::generate(const ASTNode& program) {
    Generator generator;
    return generator.run(program);
}

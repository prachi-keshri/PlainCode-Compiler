
#include "codegen.h"

#include <cctype>
#include <set>
#include <stdexcept>
#include <string>

namespace {

bool isIntegerLiteral(const std::string& value) {
    if (value.empty()) {
        return false;
    }

    std::size_t i = (value[0] == '-') ? 1 : 0;

    if (i == value.size()) {
        return false;
    }

    for (; i < value.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(value[i]))) {
            return false;
        }
    }

    return true;
}

bool isValidIdentifier(const std::string& name) {
    if (name.empty()) {
        return false;
    }

    const unsigned char first =
        static_cast<unsigned char>(name[0]);

    if (!(std::isalpha(first) || name[0] == '_')) {
        return false;
    }

    for (char ch : name) {
        const unsigned char c = static_cast<unsigned char>(ch);
        if (!(std::isalnum(c) || ch == '_')) {
            return false;
        }
    }

    return true;
}

void addVariable(std::set<std::string>& variables,
                 const std::string& name) {
    if (name.empty()) {
        throw std::runtime_error(
            "C code generation error: missing operand or result");
    }

    if (!isIntegerLiteral(name)) {
        if (!isValidIdentifier(name)) {
            throw std::runtime_error(
                "C code generation error: invalid identifier '" +
                name + "'");
        }

        variables.insert(name);
    }
}

std::string operationSymbol(TACOp op) {
    switch (op) {
        case TACOp::Add:      return "+";
        case TACOp::Subtract: return "-";
        case TACOp::Multiply: return "*";
        case TACOp::Divide:   return "/";
        default:
            throw std::runtime_error(
                "C code generation error: invalid arithmetic operation");
    }
}

} // namespace

std::string CCodeGenerator::generate(const TACProgram& program) {
    std::set<std::string> variables;

    // First pass: collect every variable and temporary.
    for (const auto& instruction : program) {
        switch (instruction.op) {
            case TACOp::Assign:
                addVariable(variables, instruction.result);
                addVariable(variables, instruction.arg1);
                if (!instruction.arg2.empty()) {
                    throw std::runtime_error(
                        "C code generation error: Assign has an extra operand");
                }
                break;

            case TACOp::Add:
            case TACOp::Subtract:
            case TACOp::Multiply:
            case TACOp::Divide:
                addVariable(variables, instruction.arg1);
                addVariable(variables, instruction.arg2);
                addVariable(variables, instruction.result);
                break;

            case TACOp::Print:
                addVariable(variables, instruction.arg1);
                if (!instruction.arg2.empty() ||
                    !instruction.result.empty()) {
                    throw std::runtime_error(
                        "C code generation error: malformed Print instruction");
                }
                break;

            default:
                throw std::runtime_error(
                    "C code generation error: unknown TAC operation");
        }
    }

    std::string output;
    output += "#include <stdio.h>\n\n";
    output += "int main(void) {\n";

    for (const auto& variable : variables) {
        output += "    int " + variable + " = 0;\n";
    }

    if (!variables.empty()) {
        output += "\n";
    }

    // Second pass: emit the instructions in order.
    for (const auto& instruction : program) {
        switch (instruction.op) {
            case TACOp::Assign:
                output += "    " + instruction.result + " = " +
                          instruction.arg1 + ";\n";
                break;

            case TACOp::Add:
            case TACOp::Subtract:
            case TACOp::Multiply:
            case TACOp::Divide:
                if (instruction.op == TACOp::Divide) {
                    output += "    if (" + instruction.arg2 +
                              " == 0) {\n";
                    output +=
                        "        fprintf(stderr, \"Division by zero\\n\");\n";
                    output += "        return 1;\n";
                    output += "    }\n";
                }

                output += "    " + instruction.result + " = " +
                          instruction.arg1 + " " +
                          operationSymbol(instruction.op) + " " +
                          instruction.arg2 + ";\n";
                break;

            case TACOp::Print:
                output += "    printf(\"%d\\n\", " +
                          instruction.arg1 + ");\n";
                break;

            default:
                throw std::runtime_error(
                    "C code generation error: unknown TAC operation");
        }
    }

    output += "    return 0;\n";
    output += "}\n";

    return output;
}

#include "optimizer.h"

#include <charconv>
#include <cstdint>
#include <limits>
#include <string>
#include <system_error>
#include <unordered_map>
#include <unordered_set>

namespace {

bool parseInteger(const std::string& text, std::int64_t& value) {
    if (text.empty()) {
        return false;
    }

    const char* begin = text.data();
    const char* end = begin + text.size();

    auto result = std::from_chars(begin, end, value);

    return result.ec == std::errc{} && result.ptr == end;
}

std::string resolveOperand(
    const std::string& operand,
    const std::unordered_map<std::string, std::string>& constants) {

    auto it = constants.find(operand);

    if (it != constants.end()) {
        return it->second;
    }

    return operand;
}

bool foldArithmetic(
    TACOp op,
    const std::string& left,
    const std::string& right,
    std::string& result) {

    std::int64_t a = 0;
    std::int64_t b = 0;

    if (!parseInteger(left, a) || !parseInteger(right, b)) {
        return false;
    }

    const auto min = std::numeric_limits<std::int64_t>::min();
    const auto max = std::numeric_limits<std::int64_t>::max();

    std::int64_t value = 0;

    switch (op) {
        case TACOp::Add:
            if ((b > 0 && a > max - b) ||
                (b < 0 && a < min - b)) {
                return false;
            }
            value = a + b;
            break;

        case TACOp::Subtract:
            if ((b < 0 && a > max + b) ||
                (b > 0 && a < min + b)) {
                return false;
            }
            value = a - b;
            break;

        case TACOp::Multiply:
            if (a == 0 || b == 0) {
                value = 0;
                break;
            }

            if ((a == -1 && b == min) ||
                (b == -1 && a == min)) {
                return false;
            }

            if (a > 0) {
                if ((b > 0 && a > max / b) ||
                    (b < 0 && b < min / a)) {
                    return false;
                }
            } else {
                if ((b > 0 && a < min / b) ||
                    (b < 0 && a < max / b)) {
                    return false;
                }
            }

            value = a * b;
            break;

        case TACOp::Divide:
            if (b == 0 || (a == min && b == -1)) {
                return false;
            }
            value = a / b;
            break;

        default:
            return false;
    }

    result = std::to_string(value);
    return true;
}

} // namespace

bool isTemporary(const std::string& name) {
    return name.rfind("__pc_tmp", 0) == 0;
}

TACProgram eliminateDeadTemporaries(const TACProgram& input) {
    std::unordered_set<std::string> live;
    TACProgram reversed;

    for (auto it = input.rbegin(); it != input.rend(); ++it) {
        const TACInstruction& instruction = *it;

        const bool writesResult =
            instruction.op != TACOp::Print;

        const bool deadTemporary =
            writesResult &&
            isTemporary(instruction.result) &&
            live.find(instruction.result) == live.end();

        // Preserve division because it may fail at runtime.
        const bool canRemove =
            deadTemporary &&
            instruction.op != TACOp::Divide;

        if (canRemove) {
            continue;
        }

        reversed.push_back(instruction);

        if (writesResult) {
            live.erase(instruction.result);
        }

        auto markUsed = [&live](const std::string& operand) {
            if (operand.empty()) {
                return;
            }

            std::int64_t number = 0;
            const char* begin = operand.data();
            const char* end = begin + operand.size();
            auto parsed = std::from_chars(begin, end, number);

            if (parsed.ec != std::errc{} || parsed.ptr != end) {
                live.insert(operand);
            }
        };

        markUsed(instruction.arg1);

        if (instruction.op == TACOp::Add ||
            instruction.op == TACOp::Subtract ||
            instruction.op == TACOp::Multiply ||
            instruction.op == TACOp::Divide) {
            markUsed(instruction.arg2);
        }
    }

    return TACProgram(reversed.rbegin(), reversed.rend());
}

TACProgram Optimizer::optimize(const TACProgram& input) {
    TACProgram output;

    std::unordered_map<std::string, std::string> constants;

    for (const auto& instruction : input) {
        TACInstruction current = instruction;

        switch (current.op) {
            case TACOp::Assign: {
                current.arg1 =
                    resolveOperand(current.arg1, constants);

                constants.erase(current.result);

                std::int64_t value = 0;

                if (parseInteger(current.arg1, value)) {
                    constants[current.result] = current.arg1;
                }

                break;
            }

            case TACOp::Add:
            case TACOp::Subtract:
            case TACOp::Multiply:
            case TACOp::Divide: {
                current.arg1 =
                    resolveOperand(current.arg1, constants);

                current.arg2 =
                    resolveOperand(current.arg2, constants);

                constants.erase(current.result);

                std::string foldedResult;

                if (foldArithmetic(
                        current.op,
                        current.arg1,
                        current.arg2,
                        foldedResult)) {

                    current.op = TACOp::Assign;
                    current.arg1 = foldedResult;
                    current.arg2.clear();

                    constants[current.result] = foldedResult;
                }

                break;
            }

            case TACOp::Print:
                current.arg1 =
                    resolveOperand(current.arg1, constants);
                break;
        }

        output.push_back(current);
    }

    return eliminateDeadTemporaries(output);
}

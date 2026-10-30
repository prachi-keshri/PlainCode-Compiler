
#include "codegen.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool isNumber(const std::string &s) {
    if (s.empty()) {
        return false;
    }

    try {
        std::size_t n = 0;
        std::stod(s, &n);
        return n == s.size();
    } catch (...) {
        return false;
    }
}

bool isString(const std::string &s) {
    return s.size() >= 2 && s.front() == '"' && s.back() == '"';
}

bool isIdentifier(const std::string &s) {
    if (s.empty() || !(std::isalpha(static_cast<unsigned char>(s[0])) || s[0] == '_')) {
        return false;
    }

    for (char c : s) {
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_')) {
            return false;
        }
    }

    return true;
}

std::string sym(TACOp op) {
    switch (op) {
    case TACOp::Add:
        return "+";
    case TACOp::Subtract:
        return "-";
    case TACOp::Multiply:
        return "*";
    case TACOp::Divide:
        return "/";
    case TACOp::Less:
        return "<";
    case TACOp::Greater:
        return ">";
    case TACOp::LessEqual:
        return "<=";
    case TACOp::GreaterEqual:
        return ">=";
    case TACOp::Equal:
        return "==";
    case TACOp::NotEqual:
        return "!=";
    default:
        throw std::runtime_error("invalid operator");
    }
}

using Types = std::map<std::string, std::string>;

void collectName(Types &t, const std::string &s, const std::string &type = "int") {

    if (s.empty() || isNumber(s) || isString(s)) {
        return;
    }

    if (!isIdentifier(s)) {
        throw std::runtime_error("invalid generated identifier '" + s + "'");
    }

    if (t.count(s) && t[s] != type) {
        if (t[s] == "int" && type == "double") {
            t[s] = "double";
        } else if (!(t[s] == "double" && type == "int")) {
            throw std::runtime_error("variable used with incompatible types: " + s);
        }
    } else {
        t[s] = type;
    }
}

} // namespace

std::string CCodeGenerator::generate(const TACProgram &p) {
    std::string out = "#include <stdio.h>\n\n";

    std::vector<std::pair<std::string, std::string>> funcs;

    // Generate function prototypes.
    for (const auto &i : p) {
        if (i.op == TACOp::FunctionBegin) {
            std::vector<std::string> ps;
            std::size_t st = 0;

            while (st < i.arg1.size()) {
                auto e = i.arg1.find(',', st);

                ps.push_back(i.arg1.substr(st, e == std::string::npos ? e : e - st));

                if (e == std::string::npos) {
                    break;
                }

                st = e + 1;
            }

            std::string sig = "double " + i.result + "(";

            for (std::size_t x = 0; x < ps.size(); ++x) {
                if (x) {
                    sig += ", ";
                }

                sig += "double " + ps[x];
            }

            if (ps.empty()) {
                sig += "void";
            }

            sig += ")";

            funcs.push_back({i.result, sig});
            out += sig + ";\n";
        }
    }

    out += "\n";

    std::size_t index = 0;

    // Handle programs where a function appears first.
    if (!p.empty() && p[0].op == TACOp::FunctionBegin) {
        out += "int main(void) {\n";
        out += "    return 0;\n";
        out += "}\n\n";
    }

    while (index < p.size()) {
        bool fn = p[index].op == TACOp::FunctionBegin;
        std::size_t end = index;

        if (fn) {
            while (end < p.size() && p[end].op != TACOp::FunctionEnd) {
                ++end;
            }

            if (end == p.size()) {
                throw std::runtime_error("unterminated function in TAC");
            }
        } else {
            end = p.size();

            for (std::size_t k = index; k < end; ++k) {
                if (p[k].op == TACOp::FunctionBegin) {
                    end = k;
                    break;
                }
            }
        }

        Types vars;
        std::string functionName;
        std::vector<std::string> params;

        // Collect function parameters.
        if (fn) {
            const auto &b = p[index];

            functionName = b.result;
            std::size_t st = 0;

            while (st < b.arg1.size()) {
                auto e = b.arg1.find(',', st);

                params.push_back(b.arg1.substr(st, e == std::string::npos ? e : e - st));

                if (e == std::string::npos) {
                    break;
                }

                st = e + 1;
            }

            for (auto &a : params) {
                collectName(vars, a, "double");
            }
        }

        // First pass: determine variable types.
        for (std::size_t k = index + (fn ? 1 : 0); k < end; ++k) {

            const auto &i = p[k];

            switch (i.op) {
            case TACOp::Assign: {
                std::string sourceType =
                    isString(i.arg1)
                        ? "const char*"
                        : (vars.count(i.arg1)
                               ? vars[i.arg1]
                               : (i.arg1.find('.') != std::string::npos ? "double" : "int"));

                collectName(vars, i.result, sourceType);
                collectName(vars, i.arg1, sourceType);
                break;
            }

            case TACOp::Add:
            case TACOp::Subtract:
            case TACOp::Multiply:
            case TACOp::Divide:
            case TACOp::Less:
            case TACOp::Greater:
            case TACOp::LessEqual:
            case TACOp::GreaterEqual:
            case TACOp::Equal:
            case TACOp::NotEqual:
                collectName(vars, i.arg1, i.arg1.find('.') != std::string::npos ? "double" : "int");

                collectName(vars, i.arg2, i.arg2.find('.') != std::string::npos ? "double" : "int");

                collectName(vars, i.result,
                            (vars.count(i.arg1) && vars[i.arg1] == "double") ||
                                    (vars.count(i.arg2) && vars[i.arg2] == "double")
                                ? "double"
                                : "int");

                break;

            case TACOp::Print:
                if (!vars.count(i.arg1)) {
                    collectName(vars, i.arg1);
                }
                break;

            case TACOp::JumpIfZero:
                collectName(vars, i.arg1);
                break;

            case TACOp::Call:
                for (const auto &a : i.args) {
                    collectName(vars, a);
                }

                collectName(vars, i.result, "double");
                break;

            case TACOp::Return:
                collectName(vars, i.arg1);
                break;

            default:
                break;
            }
        }

        // Generate the function or main signature.
        if (fn) {
            out += "double " + functionName + "(";
        } else {
            out += "int main(void";
        }

        if (fn) {
            for (std::size_t x = 0; x < params.size(); ++x) {
                if (x) {
                    out += ", ";
                }

                out += "double " + params[x];
            }

            if (params.empty()) {
                out += "void";
            }

            out += ")";
        } else {
            out += ")";
        }

        out += " {\n";

        // Declare local variables.
        for (const auto &v : vars) {
            if (std::find(params.begin(), params.end(), v.first) == params.end()) {

                out += "    " + v.second + " " + v.first + " = " +
                       (v.second == "const char*" ? "\"\"" : "0") + ";\n";
            }
        }

        // Second pass: generate C statements.
        for (std::size_t k = index + (fn ? 1 : 0); k < end; ++k) {

            const auto &i = p[k];

            switch (i.op) {
            case TACOp::Assign:
                out += "    " + i.result + " = " + i.arg1 + ";\n";
                break;

            case TACOp::Add:
            case TACOp::Subtract:
            case TACOp::Multiply:
            case TACOp::Divide:
                if (i.op == TACOp::Divide) {
                    out += "    if (" + i.arg2 +
                           " == 0) { fprintf(stderr, "
                           "\"Division by zero\\n\"); return " +
                           (fn ? std::string("0.0") : std::string("1")) + "; }\n";
                }

                out += "    " + i.result + " = " + i.arg1 + " " + sym(i.op) + " " + i.arg2 + ";\n";
                break;

            case TACOp::Less:
            case TACOp::Greater:
            case TACOp::LessEqual:
            case TACOp::GreaterEqual:
            case TACOp::Equal:
            case TACOp::NotEqual:
                out +=
                    "    " + i.result + " = (" + i.arg1 + " " + sym(i.op) + " " + i.arg2 + ");\n";
                break;

            case TACOp::JumpIfZero:
                out += "    if (!(" + i.arg1 + ")) goto " + i.result + ";\n";
                break;

            case TACOp::Jump:
                out += "    goto " + i.result + ";\n";
                break;

            case TACOp::Label:
                out += i.result + ": ;\n";
                break;

            case TACOp::Print:
                out += std::string("    printf(") +
                       ((isString(i.arg1) || (vars.count(i.arg1) && vars[i.arg1] == "const char*"))
                            ? "\"%s\\n\""
                            : ((vars.count(i.arg1) && vars[i.arg1] == "double") ||
                                       i.arg1.find('.') != std::string::npos
                                   ? "\"%g\\n\""
                                   : "\"%d\\n\"")) +
                       ", " + i.arg1 + ");\n";
                break;

            case TACOp::Return:
                out += "    return " + i.arg1 + ";\n";
                break;

            case TACOp::Call:
                out += "    " + i.result + " = " + i.arg1 + "(";

                for (std::size_t a = 0; a < i.args.size(); ++a) {
                    if (a) {
                        out += ", ";
                    }

                    out += i.args[a];
                }

                out += ");\n";
                break;

            default:
                break;
            }
        }

        // Add a default return only if the function has no explicit return.
        if (fn) {
            bool hasReturn = false;

            for (std::size_t k = index + 1; k < end; ++k) {
                if (p[k].op == TACOp::Return) {
                    hasReturn = true;
                    break;
                }
            }

            if (!hasReturn) {
                out += "    return 0.0;\n";
            }
        } else {
            out += "    return 0;\n";
        }

        // IMPORTANT: close every generated function or main block.
        out += "}\n\n";

        index = fn ? end + 1 : end;
    }

    return out;
}

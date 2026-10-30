#include "semantic.h"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
namespace {
using Types = std::unordered_map<std::string, std::string>;
using Functions = std::unordered_map<std::string, std::size_t>;
[[noreturn]] void errorAt(const ASTNode &n, const std::string &m) {
    throw std::runtime_error("Semantic error at line " + std::to_string(n.line) + ", column " +
                             std::to_string(n.column) + ": " + m);
}
bool numeric(const std::string &t) {
    return t == "number";
}
std::string exprType(const ASTNode &n, const Types &vars, const Functions &funcs) {
    if (n.type == "Number")
        return "number";
    if (n.type == "String")
        return "string";
    if (n.type == "Identifier") {
        if (n.value.rfind("__pc_", 0) == 0)
            errorAt(n, "reserved identifier '" + n.value + "'");
        auto i = vars.find(n.value);
        if (i == vars.end())
            errorAt(n, "undefined variable '" + n.value + "'");
        return i->second;
    }
    if (n.type == "Call") {
        auto f = funcs.find(n.value);
        if (f == funcs.end())
            errorAt(n, "call to undefined function '" + n.value + "'");
        if (f->second != n.children.size())
            errorAt(n, "function '" + n.value + "' expects " + std::to_string(f->second) +
                           " argument(s)");
        for (const auto &a : n.children)
            if (!numeric(exprType(*a, vars, funcs)))
                errorAt(*a, "function arguments must be numeric");
        return "number";
    }
    static const std::unordered_set<std::string> binary = {
        "Add",     "Subtract",  "Multiply",     "Divide", "Less",
        "Greater", "LessEqual", "GreaterEqual", "Equal",  "NotEqual"};
    if (binary.count(n.type)) {
        if (n.children.size() != 2)
            errorAt(n, "malformed binary expression");
        auto a = exprType(*n.children[0], vars, funcs), b = exprType(*n.children[1], vars, funcs);
        if (!numeric(a) || !numeric(b))
            errorAt(n, "arithmetic and comparison operators require numeric operands");
        return n.type == "Add" || n.type == "Subtract" || n.type == "Multiply" || n.type == "Divide"
                   ? "number"
                   : "number";
    }
    errorAt(n, "unexpected expression node '" + n.type + "'");
}
Types block(const ASTNode &b, Types vars, const Functions &fs, bool inFunction, bool &hasReturn) {
    for (const auto &p : b.children) {
        const ASTNode &s = *p;
        if (s.type == "Assignment") {
            if (s.value.rfind("__pc_", 0) == 0)
                errorAt(s, "reserved identifier '" + s.value + "'");
            if (s.children.size() != 1)
                errorAt(s, "malformed assignment");
            auto assignedType = exprType(*s.children[0], vars, fs);
            if (vars.count(s.value) && vars[s.value] != assignedType)
                errorAt(s, "cannot change variable type for '" + s.value + "'");
            vars[s.value] = assignedType;
        } else if (s.type == "Display") {
            if (s.children.size() != 1)
                errorAt(s, "malformed DISPLAY");
            (void)exprType(*s.children[0], vars, fs);
        } else if (s.type == "Return") {
            if (!inFunction)
                errorAt(s, "RETURN is only valid inside a FUNCTION");
            if (s.children.size() != 1)
                errorAt(s, "malformed RETURN");
            if (!numeric(exprType(*s.children[0], vars, fs)))
                errorAt(s, "functions currently return numeric values");
            hasReturn = true;
        } else if (s.type == "If") {
            if (s.children.size() < 2 || s.children.size() > 3)
                errorAt(s, "malformed IF");
            if (!numeric(exprType(*s.children[0], vars, fs)))
                errorAt(s, "IF condition must be numeric");
            bool r1 = false, r2 = false;
            Types a = block(*s.children[1], vars, fs, inFunction, r1),
                  c = s.children.size() == 3 ? block(*s.children[2], vars, fs, inFunction, r2)
                                             : vars;
            Types merged;
            for (const auto &x : a)
                if (c.count(x.first)) {
                    if (a[x.first] != c[x.first])
                        errorAt(s, "variable '" + x.first +
                                       "' has inconsistent types across IF branches");
                    merged[x.first] = x.second;
                }
            vars = std::move(merged);
            hasReturn = hasReturn || (r1 && r2);
        } else if (s.type == "While") {
            if (s.children.size() != 2)
                errorAt(s, "malformed WHILE");
            if (!numeric(exprType(*s.children[0], vars, fs)))
                errorAt(s, "WHILE condition must be numeric");
            bool ignored = false;
            (void)block(*s.children[1], vars, fs, inFunction, ignored);
        } else
            errorAt(s, "unexpected statement node '" + s.type + "'");
    }
    return vars;
}
} // namespace
void SemanticAnalyzer::analyze(const ASTNode &p) {
    if (p.type != "Program")
        throw std::runtime_error("Semantic error: expected Program node");
    Functions fs;
    for (const auto &x : p.children)
        if (x->type == "Function") {
            if (x->value.rfind("__pc_", 0) == 0 || x->value == "main" || x->value == "printf" ||
                x->value == "fprintf")
                errorAt(*x, "reserved function name '" + x->value + "'");
            if (fs.count(x->value))
                errorAt(*x, "duplicate function '" + x->value + "'");
            if (x->children.size() != 2)
                errorAt(*x, "malformed function");
            fs[x->value] = x->children[0]->children.size();
        }
    for (const auto &x : p.children)
        if (x->type == "Function") {
            Types vars;
            for (const auto &q : x->children[0]->children) {
                if (q->value.rfind("__pc_", 0) == 0)
                    errorAt(*q, "reserved identifier '" + q->value + "'");
                if (vars.count(q->value))
                    errorAt(*q, "duplicate parameter '" + q->value + "'");
                vars[q->value] = "number";
            }
            bool ret = false;
            (void)block(*x->children[1], vars, fs, true, ret);
            if (!ret)
                errorAt(*x, "function '" + x->value + "' must contain a RETURN statement");
        }
    Types globals;
    for (const auto &ptr : p.children) {
        const ASTNode &s = *ptr;
        if (s.type == "Function")
            continue;
        if (s.type == "Assignment") {
            if (s.value.rfind("__pc_", 0) == 0)
                errorAt(s, "reserved identifier '" + s.value + "'");
            if (s.children.size() != 1)
                errorAt(s, "malformed assignment");
            auto assignedType = exprType(*s.children[0], globals, fs);
            if (globals.count(s.value) && globals[s.value] != assignedType)
                errorAt(s, "cannot change variable type for '" + s.value + "'");
            globals[s.value] = assignedType;
        } else if (s.type == "Display") {
            if (s.children.size() != 1)
                errorAt(s, "malformed DISPLAY");
            (void)exprType(*s.children[0], globals, fs);
        } else if (s.type == "If") {
            if (s.children.size() < 2 || s.children.size() > 3)
                errorAt(s, "malformed IF");
            if (!numeric(exprType(*s.children[0], globals, fs)))
                errorAt(s, "IF condition must be numeric");
            bool r1 = false, r2 = false;
            Types a = block(*s.children[1], globals, fs, false, r1),
                  b = s.children.size() == 3 ? block(*s.children[2], globals, fs, false, r2)
                                             : globals;
            Types merged;
            for (const auto &v : a)
                if (b.count(v.first)) {
                    if (a[v.first] != b[v.first])
                        errorAt(s, "variable '" + v.first +
                                       "' has inconsistent types across IF branches");
                    merged[v.first] = v.second;
                }
            globals = std::move(merged);
        } else if (s.type == "While") {
            if (s.children.size() != 2)
                errorAt(s, "malformed WHILE");
            if (!numeric(exprType(*s.children[0], globals, fs)))
                errorAt(s, "WHILE condition must be numeric");
            bool ignored = false;
            (void)block(*s.children[1], globals, fs, false, ignored);
        } else if (s.type == "Return")
            errorAt(s, "RETURN is only valid inside a FUNCTION");
        else
            errorAt(s, "unexpected top-level declaration");
    }
}

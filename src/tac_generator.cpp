#include "tac_generator.h"
#include <stdexcept>
#include <string>
namespace {
class Generator {
  public:
    TACProgram instructions;
    TACProgram run(const ASTNode &p) {
        if (p.type != "Program")
            throw std::runtime_error("TAC generation error: expected Program node");
        for (const auto &s : p.children)
            if (s->type != "Function")
                statement(*s);
        for (const auto &s : p.children)
            if (s->type == "Function")
                function(*s);
        return instructions;
    }

  private:
    unsigned temp = 0, label = 0;
    std::string newTemp() {
        return "__pc_tmp" + std::to_string(++temp);
    }
    std::string newLabel() {
        return "__pc_label" + std::to_string(++label);
    }
    void require(const ASTNode &n, size_t count) {
        if (n.children.size() != count)
            throw std::runtime_error("TAC generation error: malformed " + n.type + " node");
    }
    std::string stringLiteral(const std::string &value) {
        std::string q = "\"";
        for (std::size_t i = 0; i < value.size(); ++i) {
            char c = value[i];
            if (c == '\\' && i + 1 < value.size() && (value[i + 1] == 'n' || value[i + 1] == 't')) {
                q += '\\';
                q += value[++i];
            } else {
                if (c == '\\' || c == '\"')
                    q += '\\';
                q += c;
            }
        }
        return q + "\"";
    }
    std::string expr(const ASTNode &n) {
        if (n.type == "Number")
            return n.value;
        if (n.type == "String")
            return stringLiteral(n.value);
        if (n.type == "Identifier")
            return n.value;
        if (n.type == "Call") {
            std::vector<std::string> a;
            for (const auto &c : n.children)
                a.push_back(expr(*c));
            std::string r = newTemp();
            TACInstruction i{TACOp::Call, n.value, "", r};
            i.args = std::move(a);
            instructions.push_back(std::move(i));
            return r;
        }
        require(n, 2);
        std::string a = expr(*n.children[0]), b = expr(*n.children[1]), r = newTemp();
        TACOp op;
        if (n.type == "Add")
            op = TACOp::Add;
        else if (n.type == "Subtract")
            op = TACOp::Subtract;
        else if (n.type == "Multiply")
            op = TACOp::Multiply;
        else if (n.type == "Divide")
            op = TACOp::Divide;
        else if (n.type == "Less")
            op = TACOp::Less;
        else if (n.type == "Greater")
            op = TACOp::Greater;
        else if (n.type == "LessEqual")
            op = TACOp::LessEqual;
        else if (n.type == "GreaterEqual")
            op = TACOp::GreaterEqual;
        else if (n.type == "Equal")
            op = TACOp::Equal;
        else if (n.type == "NotEqual")
            op = TACOp::NotEqual;
        else
            throw std::runtime_error("TAC generation error: unsupported expression");
        instructions.push_back({op, a, b, r});
        return r;
    }
    void block(const ASTNode &n) {
        for (const auto &s : n.children)
            statement(*s);
    }
    void statement(const ASTNode &n) {
        if (n.type == "Assignment") {
            require(n, 1);
            instructions.push_back({TACOp::Assign, expr(*n.children[0]), "", n.value});
        } else if (n.type == "Display") {
            require(n, 1);
            instructions.push_back({TACOp::Print, expr(*n.children[0]), "", ""});
        } else if (n.type == "Return") {
            require(n, 1);
            instructions.push_back({TACOp::Return, expr(*n.children[0]), "", ""});
        } else if (n.type == "If") {
            if (n.children.size() < 2 || n.children.size() > 3)
                throw std::runtime_error("malformed IF");
            std::string c = expr(*n.children[0]), otherwise = newLabel(), end = newLabel();
            instructions.push_back({TACOp::JumpIfZero, c, "", otherwise});
            block(*n.children[1]);
            if (n.children.size() == 3) {
                instructions.push_back({TACOp::Jump, "", "", end});
                instructions.push_back({TACOp::Label, "", "", otherwise});
                block(*n.children[2]);
                instructions.push_back({TACOp::Label, "", "", end});
            } else
                instructions.push_back({TACOp::Label, "", "", otherwise});
        } else if (n.type == "While") {
            require(n, 2);
            std::string start = newLabel(), end = newLabel();
            instructions.push_back({TACOp::Label, "", "", start});
            std::string c = expr(*n.children[0]);
            instructions.push_back({TACOp::JumpIfZero, c, "", end});
            block(*n.children[1]);
            instructions.push_back({TACOp::Jump, "", "", start});
            instructions.push_back({TACOp::Label, "", "", end});
        } else
            throw std::runtime_error("unsupported statement '" + n.type + "'");
    }
    void function(const ASTNode &n) {
        require(n, 2);
        std::string params;
        for (const auto &p : n.children[0]->children) {
            if (!params.empty())
                params += ",";
            params += p->value;
        }
        instructions.push_back({TACOp::FunctionBegin, params, "", n.value});
        block(*n.children[1]);
        instructions.push_back({TACOp::FunctionEnd, "", "", n.value});
    }
};
} // namespace
TACProgram TACGenerator::generate(const ASTNode &p) {
    Generator g;
    return g.run(p);
}

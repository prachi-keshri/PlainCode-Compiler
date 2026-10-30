#include "parser.h"
#include <algorithm>
#include <sstream>
Parser::Parser(const std::vector<Token> &input) : tokens(input) {}
const Token &Parser::current() const {
    if (pos >= tokens.size())
        throw std::runtime_error("Syntax error: unexpected end of token stream");
    return tokens[pos];
}
bool Parser::match(TokenType t) {
    if (pos < tokens.size() && tokens[pos].type == t) {
        ++pos;
        return true;
    }
    return false;
}
[[noreturn]] void Parser::syntaxError(const std::string &expected) const {
    const Token &t = current();
    std::ostringstream s;
    s << "Syntax error at line " << t.line << ", column " << t.column << ": expected " << expected
      << ", found '" << (t.value.empty() ? "end of file" : t.value) << "'";
    throw std::runtime_error(s.str());
}
Token Parser::consume(TokenType t) {
    if (match(t))
        return tokens[pos - 1];
    std::string e = "token";
    switch (t) {
    case TokenType::SET:
        e = "SET";
        break;
    case TokenType::TO:
        e = "TO";
        break;
    case TokenType::DISPLAY:
        e = "DISPLAY";
        break;
    case TokenType::IF:
        e = "IF";
        break;
    case TokenType::THEN:
        e = "THEN";
        break;
    case TokenType::ELSE:
        e = "ELSE";
        break;
    case TokenType::ENDIF:
        e = "ENDIF";
        break;
    case TokenType::WHILE:
        e = "WHILE";
        break;
    case TokenType::DO:
        e = "DO";
        break;
    case TokenType::ENDWHILE:
        e = "ENDWHILE";
        break;
    case TokenType::FUNCTION:
        e = "FUNCTION";
        break;
    case TokenType::ENDFUNCTION:
        e = "ENDFUNCTION";
        break;
    case TokenType::RETURN:
        e = "RETURN";
        break;
    case TokenType::IDENTIFIER:
        e = "identifier";
        break;
    case TokenType::LPAREN:
        e = "'('";
        break;
    case TokenType::RPAREN:
        e = "')'";
        break;
    case TokenType::COMMA:
        e = "','";
        break;
    default:
        break;
    }
    syntaxError(e);
}
std::unique_ptr<ASTNode> Parser::parse() {
    auto root = std::make_unique<ASTNode>("Program");
    while (current().type != TokenType::END_OF_FILE) {
        if (match(TokenType::NEWLINE))
            continue;
        root->children.push_back(parseStatement());
        if (current().type != TokenType::END_OF_FILE && !match(TokenType::NEWLINE))
            syntaxError("end of line");
    }
    consume(TokenType::END_OF_FILE);
    return root;
}
std::vector<std::unique_ptr<ASTNode>> Parser::parseBlock(const std::vector<TokenType> &stops) {
    std::vector<std::unique_ptr<ASTNode>> b;
    while (current().type != TokenType::END_OF_FILE &&
           std::find(stops.begin(), stops.end(), current().type) == stops.end()) {
        if (match(TokenType::NEWLINE))
            continue;
        b.push_back(parseStatement());
        if (current().type != TokenType::END_OF_FILE &&
            std::find(stops.begin(), stops.end(), current().type) == stops.end() &&
            !match(TokenType::NEWLINE))
            syntaxError("end of line");
    }
    return b;
}
std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::SET)) {
        Token id = consume(TokenType::IDENTIFIER);
        consume(TokenType::TO);
        auto n = std::make_unique<ASTNode>("Assignment", id.value, id.line, id.column);
        n->children.push_back(parseExpression());
        return n;
    }
    if (match(TokenType::DISPLAY)) {
        Token t = tokens[pos - 1];
        auto n = std::make_unique<ASTNode>("Display", "", t.line, t.column);
        n->children.push_back(parseExpression());
        return n;
    }
    if (match(TokenType::RETURN)) {
        Token t = tokens[pos - 1];
        auto n = std::make_unique<ASTNode>("Return", "", t.line, t.column);
        n->children.push_back(parseExpression());
        return n;
    }
    if (match(TokenType::FUNCTION)) {
        Token t = tokens[pos - 1], id = consume(TokenType::IDENTIFIER);
        auto n = std::make_unique<ASTNode>("Function", id.value, t.line, t.column);
        consume(TokenType::LPAREN);
        auto params = std::make_unique<ASTNode>("Parameters");
        if (current().type != TokenType::RPAREN) {
            do {
                Token p = consume(TokenType::IDENTIFIER);
                params->children.push_back(
                    std::make_unique<ASTNode>("Parameter", p.value, p.line, p.column));
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RPAREN);
        n->children.push_back(std::move(params));
        while (match(TokenType::NEWLINE)) {
        };
        auto body = parseBlock({TokenType::ENDFUNCTION});
        auto b = std::make_unique<ASTNode>("Block");
        for (auto &x : body)
            b->children.push_back(std::move(x));
        n->children.push_back(std::move(b));
        consume(TokenType::ENDFUNCTION);
        return n;
    }
    if (match(TokenType::IF)) {
        Token t = tokens[pos - 1];
        auto n = std::make_unique<ASTNode>("If", "", t.line, t.column);
        n->children.push_back(parseComparison());
        consume(TokenType::THEN);
        while (match(TokenType::NEWLINE)) {
        };
        auto tb = parseBlock({TokenType::ELSE, TokenType::ENDIF});
        auto b = std::make_unique<ASTNode>("Block");
        for (auto &x : tb)
            b->children.push_back(std::move(x));
        n->children.push_back(std::move(b));
        if (match(TokenType::ELSE)) {
            while (match(TokenType::NEWLINE)) {
            };
            auto eb = parseBlock({TokenType::ENDIF});
            auto e = std::make_unique<ASTNode>("Block");
            for (auto &x : eb)
                e->children.push_back(std::move(x));
            n->children.push_back(std::move(e));
        }
        consume(TokenType::ENDIF);
        return n;
    }
    if (match(TokenType::WHILE)) {
        Token t = tokens[pos - 1];
        auto n = std::make_unique<ASTNode>("While", "", t.line, t.column);
        n->children.push_back(parseComparison());
        consume(TokenType::DO);
        while (match(TokenType::NEWLINE)) {
        };
        auto wb = parseBlock({TokenType::ENDWHILE});
        auto b = std::make_unique<ASTNode>("Block");
        for (auto &x : wb)
            b->children.push_back(std::move(x));
        n->children.push_back(std::move(b));
        consume(TokenType::ENDWHILE);
        return n;
    }
    syntaxError("SET, DISPLAY, IF, WHILE, FUNCTION, or RETURN");
}
std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto l = parseExpression();
    TokenType t = current().type;
    std::string op;
    switch (t) {
    case TokenType::LESS:
        op = "Less";
        break;
    case TokenType::GREATER:
        op = "Greater";
        break;
    case TokenType::LESS_EQUAL:
        op = "LessEqual";
        break;
    case TokenType::GREATER_EQUAL:
        op = "GreaterEqual";
        break;
    case TokenType::EQUAL:
        op = "Equal";
        break;
    case TokenType::NOT_EQUAL:
        op = "NotEqual";
        break;
    default:
        syntaxError("comparison operator");
    }
    ++pos;
    auto r = parseExpression();
    auto n = std::make_unique<ASTNode>(op);
    n->children.push_back(std::move(l));
    n->children.push_back(std::move(r));
    return n;
}
std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto l = parseTerm();
    while (current().type == TokenType::PLUS || current().type == TokenType::MINUS) {
        auto t = current().type;
        ++pos;
        auto r = parseTerm();
        auto n = std::make_unique<ASTNode>(t == TokenType::PLUS ? "Add" : "Subtract");
        n->children.push_back(std::move(l));
        n->children.push_back(std::move(r));
        l = std::move(n);
    }
    return l;
}
std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto l = parseFactor();
    while (current().type == TokenType::MULTIPLY || current().type == TokenType::DIVIDE) {
        auto t = current().type;
        ++pos;
        auto r = parseFactor();
        auto n = std::make_unique<ASTNode>(t == TokenType::MULTIPLY ? "Multiply" : "Divide");
        n->children.push_back(std::move(l));
        n->children.push_back(std::move(r));
        l = std::move(n);
    }
    return l;
}
std::unique_ptr<ASTNode> Parser::parseFactor() {
    if (match(TokenType::NUMBER))
        return std::make_unique<ASTNode>("Number", tokens[pos - 1].value, tokens[pos - 1].line,
                                         tokens[pos - 1].column);
    if (match(TokenType::STRING))
        return std::make_unique<ASTNode>("String", tokens[pos - 1].value, tokens[pos - 1].line,
                                         tokens[pos - 1].column);
    if (match(TokenType::IDENTIFIER)) {
        Token id = tokens[pos - 1];
        if (match(TokenType::LPAREN)) {
            auto call = std::make_unique<ASTNode>("Call", id.value, id.line, id.column);
            if (current().type != TokenType::RPAREN) {
                do {
                    call->children.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN);
            return call;
        }
        return std::make_unique<ASTNode>("Identifier", id.value, id.line, id.column);
    }
    if (match(TokenType::LPAREN)) {
        auto e = parseExpression();
        consume(TokenType::RPAREN);
        return e;
    }
    syntaxError("number, string, identifier, function call, or '('");
}

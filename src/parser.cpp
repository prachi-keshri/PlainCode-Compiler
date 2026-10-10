
#include "parser.h"

#include <stdexcept>
#include <string>
#include <utility>

Parser::Parser(const std::vector<Token>& inputTokens)
    : tokens(inputTokens) {}

const Token& Parser::current() const {
    if (pos >= tokens.size()) {
        throw std::runtime_error("Unexpected end of token stream");
    }
    return tokens[pos];
}

bool Parser::match(TokenType type) {
    if (pos < tokens.size() && tokens[pos].type == type) {
        ++pos;
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type) {
    if (pos >= tokens.size() || tokens[pos].type != type) {
        throw std::runtime_error("Syntax error: unexpected token");
    }
    return tokens[pos++];
}

std::unique_ptr<ASTNode> Parser::parse() {
    auto root = std::make_unique<ASTNode>("Program");

    while (pos < tokens.size() &&
           tokens[pos].type != TokenType::END_OF_FILE) {
        if (match(TokenType::NEWLINE)) {
            continue;
        }

        root->children.push_back(parseStatement());

        if (pos < tokens.size() &&
            tokens[pos].type != TokenType::END_OF_FILE &&
            !match(TokenType::NEWLINE)) {
            throw std::runtime_error(
                "Syntax error: expected end of line");
        }
    }

    consume(TokenType::END_OF_FILE);
    return root;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::SET)) {
        Token identifier = consume(TokenType::IDENTIFIER);
        consume(TokenType::TO);

        auto assignment =
            std::make_unique<ASTNode>("Assignment", identifier.value);

        assignment->children.push_back(parseExpression());
        return assignment;
    }

    if (match(TokenType::DISPLAY)) {
        auto display = std::make_unique<ASTNode>("Display");
        display->children.push_back(parseExpression());
        return display;
    }

    throw std::runtime_error(
        "Syntax error: expected SET or DISPLAY");
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseTerm();

    while (pos < tokens.size()) {
        TokenType op = current().type;

        if (op != TokenType::PLUS && op != TokenType::MINUS) {
            break;
        }

        ++pos;
        auto right = parseTerm();

        auto parent = std::make_unique<ASTNode>(
            op == TokenType::PLUS ? "Add" : "Subtract");

        parent->children.push_back(std::move(left));
        parent->children.push_back(std::move(right));
        left = std::move(parent);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();

    while (pos < tokens.size()) {
        TokenType op = current().type;

        if (op != TokenType::MULTIPLY &&
            op != TokenType::DIVIDE) {
            break;
        }

        ++pos;
        auto right = parseFactor();

        auto parent = std::make_unique<ASTNode>(
            op == TokenType::MULTIPLY ? "Multiply" : "Divide");

        parent->children.push_back(std::move(left));
        parent->children.push_back(std::move(right));
        left = std::move(parent);
    }

    return left;
}

std::unique_ptr<ASTNode> Parser::parseFactor() {
    if (match(TokenType::NUMBER)) {
        return std::make_unique<ASTNode>(
            "Number", tokens[pos - 1].value);
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<ASTNode>(
            "Identifier", tokens[pos - 1].value);
    }

    if (match(TokenType::LPAREN)) {
        auto expression = parseExpression();
        consume(TokenType::RPAREN);
        return expression;
    }

    throw std::runtime_error(
        "Syntax error: expected number, identifier, or '('");
}

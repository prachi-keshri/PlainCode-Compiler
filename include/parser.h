
#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "token.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ASTNode> parse();

private:
    std::vector<Token> tokens;
    std::size_t pos = 0;

    const Token& current() const;
    bool match(TokenType type);
    Token consume(TokenType type);
    [[noreturn]] void syntaxError(
        const std::string& expected) const;

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
};

#endif


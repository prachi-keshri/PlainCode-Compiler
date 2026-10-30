#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "token.h"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
class Parser {
  public:
    explicit Parser(const std::vector<Token> &tokens);
    std::unique_ptr<ASTNode> parse();

  private:
    std::vector<Token> tokens;
    std::size_t pos = 0;
    const Token &current() const;
    bool match(TokenType type);
    Token consume(TokenType type);
    [[noreturn]] void syntaxError(const std::string &expected) const;
    std::unique_ptr<ASTNode> parseStatement();
    std::vector<std::unique_ptr<ASTNode>> parseBlock(const std::vector<TokenType> &stops);
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
};
#endif

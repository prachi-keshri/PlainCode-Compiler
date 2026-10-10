
#include "parser.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

void printAST(const ASTNode* node, int depth = 0) {
    if (!node) return;

    std::cout << std::string(depth * 2, ' ')
              << node->type;

    if (!node->value.empty()) {
        std::cout << ": " << node->value;
    }

    std::cout << '\n';

    for (const auto& child : node->children) {
        printAST(child.get(), depth + 1);
    }
}

int main() {
    std::vector<Token> tokens = {
        {TokenType::SET, "SET", 1, 1},
        {TokenType::IDENTIFIER, "result", 1, 5},
        {TokenType::TO, "TO", 1, 12},
        {TokenType::NUMBER, "5", 1, 15},
        {TokenType::PLUS, "+", 1, 17},
        {TokenType::NUMBER, "3", 1, 19},
        {TokenType::MULTIPLY, "*", 1, 21},
        {TokenType::NUMBER, "2", 1, 23},
        {TokenType::NEWLINE, "\\n", 1, 24},
        {TokenType::END_OF_FILE, "", 2, 1}
    };

    try {
        Parser parser(tokens);
        auto ast = parser.parse();

        std::cout << "Parsing successful!\\n";
        printAST(ast.get());
    } catch (const std::exception& error) {
        std::cerr << "Parsing failed: "
                  << error.what() << '\n';
        return 1;
    }

    return 0;
}

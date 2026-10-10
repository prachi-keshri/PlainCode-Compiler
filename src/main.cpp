
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "lexer.h"
#include "parser.h"
#include "ast.h"

void printAST(const ASTNode* node, int depth = 0) {
    if (node == nullptr) {
        return;
    }

    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }

    std::cout << node->type;

    if (!node->value.empty()) {
        std::cout << ": " << node->value;
    }

    std::cout << '\n';

    for (const auto& child : node->children) {
        printAST(child.get(), depth + 1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: plaincode <source-file.plain>\n";
        return 1;
    }

    std::ifstream input(argv[1]);

    if (!input) {
        std::cerr << "Error: could not open file '"
                  << argv[1] << "'\n";
        return 1;
    }

    const std::string source(
        (std::istreambuf_iterator<char>(input)),
        std::istreambuf_iterator<char>()
    );

    try {
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<ASTNode> ast = parser.parse();

        std::cout << "Parsing successful!\n";
        std::cout << "Abstract Syntax Tree:\n";
        printAST(ast.get());

    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}

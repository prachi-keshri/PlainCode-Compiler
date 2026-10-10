
#include "parser.h"

#include <cassert>
#include <iostream>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<Token> makeTokens(
    std::initializer_list<Token> input) {
    std::vector<Token> tokens(input);

    tokens.push_back(
        {TokenType::END_OF_FILE, "", 2, 1});

    return tokens;
}

std::unique_ptr<ASTNode> parseTokens(
    const std::vector<Token>& tokens) {
    Parser parser(tokens);
    return parser.parse();
}

void testOperatorPrecedence() {
    auto tokens = makeTokens({
        {TokenType::SET, "SET", 1, 1},
        {TokenType::IDENTIFIER, "result", 1, 5},
        {TokenType::TO, "TO", 1, 12},
        {TokenType::NUMBER, "5", 1, 15},
        {TokenType::PLUS, "+", 1, 17},
        {TokenType::NUMBER, "3", 1, 19},
        {TokenType::MULTIPLY, "*", 1, 21},
        {TokenType::NUMBER, "2", 1, 23}
    });

    auto ast = parseTokens(tokens);

    assert(ast->type == "Program");
    assert(ast->children.size() == 1);

    const ASTNode* assignment = ast->children[0].get();
    assert(assignment->type == "Assignment");
    assert(assignment->value == "result");

    const ASTNode* add = assignment->children[0].get();
    assert(add->type == "Add");
    assert(add->children[0]->value == "5");

    const ASTNode* multiply = add->children[1].get();
    assert(multiply->type == "Multiply");
    assert(multiply->children[0]->value == "3");
    assert(multiply->children[1]->value == "2");
}

void testParentheses() {
    auto tokens = makeTokens({
        {TokenType::SET, "SET", 1, 1},
        {TokenType::IDENTIFIER, "result", 1, 5},
        {TokenType::TO, "TO", 1, 12},
        {TokenType::LPAREN, "(", 1, 15},
        {TokenType::NUMBER, "5", 1, 16},
        {TokenType::PLUS, "+", 1, 18},
        {TokenType::NUMBER, "3", 1, 20},
        {TokenType::RPAREN, ")", 1, 21},
        {TokenType::MULTIPLY, "*", 1, 23},
        {TokenType::NUMBER, "2", 1, 25}
    });

    auto ast = parseTokens(tokens);
    const ASTNode* assignment = ast->children[0].get();
    const ASTNode* multiply = assignment->children[0].get();

    assert(multiply->type == "Multiply");
    assert(multiply->children[0]->type == "Add");
}

void testLeftAssociativity() {
    auto tokens = makeTokens({
        {TokenType::SET, "SET", 1, 1},
        {TokenType::IDENTIFIER, "result", 1, 5},
        {TokenType::TO, "TO", 1, 12},
        {TokenType::NUMBER, "10", 1, 15},
        {TokenType::MINUS, "-", 1, 18},
        {TokenType::NUMBER, "3", 1, 20},
        {TokenType::MINUS, "-", 1, 22},
        {TokenType::NUMBER, "2", 1, 24}
    });

    auto ast = parseTokens(tokens);
    const ASTNode* assignment = ast->children[0].get();
    const ASTNode* outerSubtract = assignment->children[0].get();

    assert(outerSubtract->type == "Subtract");
    assert(outerSubtract->children[0]->type == "Subtract");
}

void testDisplayStatement() {
    auto tokens = makeTokens({
        {TokenType::DISPLAY, "DISPLAY", 1, 1},
        {TokenType::NUMBER, "42", 1, 9}
    });

    auto ast = parseTokens(tokens);

    assert(ast->children.size() == 1);
    assert(ast->children[0]->type == "Display");
    assert(ast->children[0]->children[0]->value == "42");
}


void expectSyntaxError(
    const std::vector<Token>& tokens,
    const std::string& testName) {
    try {
        auto ast = parseTokens(tokens);
        (void)ast;

        std::cerr << "FAILED: " << testName
                  << " was accepted\n";
        assert(false);
    } catch (const std::runtime_error& error) {
        const std::string message = error.what();

        if (message.find("line ") == std::string::npos ||
            message.find("column ") == std::string::npos) {
            std::cerr << "FAILED: " << testName
                      << " has no line/column information: "
                      << message << '\n';
            assert(false);
        }

        std::cout << "PASSED: " << testName
                  << " rejected with location information\n";
    }
}


void testInvalidSyntax() {
    expectSyntaxError(makeTokens({
        {TokenType::SET, "SET", 1, 1},
        {TokenType::IDENTIFIER, "x", 1, 5},
        {TokenType::NUMBER, "5", 1, 10}
    }), "Missing TO");

    expectSyntaxError(makeTokens({
        {TokenType::SET, "SET", 1, 1},
        {TokenType::IDENTIFIER, "x", 1, 5},
        {TokenType::TO, "TO", 1, 7},
        {TokenType::NUMBER, "5", 1, 10},
        {TokenType::PLUS, "+", 1, 12}
    }), "Missing right operand");

    expectSyntaxError(makeTokens({
        {TokenType::SET, "SET", 1, 1},
        {TokenType::IDENTIFIER, "x", 1, 5},
        {TokenType::TO, "TO", 1, 7},
        {TokenType::LPAREN, "(", 1, 10},
        {TokenType::NUMBER, "5", 1, 11},
        {TokenType::PLUS, "+", 1, 13},
        {TokenType::NUMBER, "3", 1, 15}
    }), "Missing closing parenthesis");

    expectSyntaxError(makeTokens({
        {TokenType::IDENTIFIER, "x", 1, 1}
    }), "Invalid statement");
}

int main() {
    testOperatorPrecedence();
    testParentheses();
    testLeftAssociativity();
    testDisplayStatement();
    testInvalidSyntax();

    std::cout << "\nAll parser tests passed!\n";
    return 0;
}

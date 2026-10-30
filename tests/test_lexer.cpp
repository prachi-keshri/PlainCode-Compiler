
#include "lexer.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

string tokenName(TokenType type) {
    switch (type) {
    case TokenType::SET:
        return "SET";
    case TokenType::TO:
        return "TO";
    case TokenType::DISPLAY:
        return "DISPLAY";
    case TokenType::IF:
        return "IF";
    case TokenType::THEN:
        return "THEN";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::ENDIF:
        return "ENDIF";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::DO:
        return "DO";
    case TokenType::ENDWHILE:
        return "ENDWHILE";
    case TokenType::FUNCTION:
        return "FUNCTION";
    case TokenType::ENDFUNCTION:
        return "ENDFUNCTION";
    case TokenType::RETURN:
        return "RETURN";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::STRING:
        return "STRING";
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::MULTIPLY:
        return "MULTIPLY";
    case TokenType::DIVIDE:
        return "DIVIDE";
    case TokenType::LESS:
        return "LESS";
    case TokenType::GREATER:
        return "GREATER";
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
    case TokenType::GREATER_EQUAL:
        return "GREATER_EQUAL";
    case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::NOT_EQUAL:
        return "NOT_EQUAL";
    case TokenType::LPAREN:
        return "LPAREN";
    case TokenType::RPAREN:
        return "RPAREN";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::NEWLINE:
        return "NEWLINE";
    case TokenType::END_OF_FILE:
        return "EOF";
    }
    return "UNKNOWN";
}

int main() {
    ifstream file("examples/sample.plain");

    if (!file) {
        cerr << "Error: Cannot open sample.plain\n";
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();

    try {
        Lexer lexer(buffer.str());
        vector<Token> tokens = lexer.tokenize();

        cout << "TOKEN\t\tVALUE\t\tLINE\tCOLUMN\n";
        cout << "------------------------------------------------\n";

        for (const Token &t : tokens) {
            cout << tokenName(t.type) << "\t\t" << t.value << "\t\t" << t.line << "\t" << t.column
                 << "\n";
        }
    } catch (const exception &e) {
        cerr << "Lexer Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

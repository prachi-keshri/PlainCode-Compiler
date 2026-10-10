
#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"

using namespace std;

string tokenName(TokenType type) {
    switch (type) {
        case TokenType::SET: return "SET";
        case TokenType::TO: return "TO";
        case TokenType::DISPLAY: return "DISPLAY";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::END_OF_FILE: return "EOF";
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

        for (const Token& t : tokens) {
            cout << tokenName(t.type) << "\t\t"
                 << t.value << "\t\t"
                 << t.line << "\t"
                 << t.column << "\n";
        }
    }
    catch (const exception& e) {
        cerr << "Lexer Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

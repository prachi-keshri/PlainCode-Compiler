
#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    SET,
    TO,
    DISPLAY,

    IDENTIFIER,
    NUMBER,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,

    LPAREN,
    RPAREN,

    NEWLINE,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

#endif

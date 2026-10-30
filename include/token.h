#ifndef TOKEN_H
#define TOKEN_H
#include <string>
enum class TokenType {
    SET,
    TO,
    DISPLAY,
    IF,
    THEN,
    ELSE,
    ENDIF,
    WHILE,
    DO,
    ENDWHILE,
    FUNCTION,
    ENDFUNCTION,
    RETURN,
    IDENTIFIER,
    NUMBER,
    STRING,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    EQUAL,
    NOT_EQUAL,
    LPAREN,
    RPAREN,
    COMMA,
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


#include "lexer.h"
#include <cctype>
#include <stdexcept>

using namespace std;

Lexer::Lexer(const string& source) : source(source) {}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;

    while (pos < source.size()) {
        char c = source[pos];

        // Skip spaces and tabs
        if (c == ' ' || c == '\t') {
            pos++;
            column++;
            continue;
        }

        // Handle Windows and Unix newlines
        if (c == '\r' || c == '\n') {
            int startCol = column;

            if (c == '\r' && pos + 1 < source.size()
                && source[pos + 1] == '\n') {
                pos += 2;
            } else {
                pos++;
            }

            tokens.push_back({
                TokenType::NEWLINE, "\\n", line, startCol
            });

            line++;
            column = 1;
            continue;
        }

        // Keywords and identifiers
        if (isalpha((unsigned char)c) || c == '_') {
            size_t start = pos;
            int startCol = column;

            while (pos < source.size() &&
                  (isalnum((unsigned char)source[pos]) ||
                   source[pos] == '_')) {
                pos++;
                column++;
            }

            string word = source.substr(start, pos - start);
            TokenType type = TokenType::IDENTIFIER;

            if (word == "SET") type = TokenType::SET;
            else if (word == "TO") type = TokenType::TO;
            else if (word == "DISPLAY") type = TokenType::DISPLAY;

            tokens.push_back({type, word, line, startCol});
            continue;
        }

        // Numbers
        if (isdigit((unsigned char)c)) {
            size_t start = pos;
            int startCol = column;

            while (pos < source.size() &&
                   isdigit((unsigned char)source[pos])) {
                pos++;
                column++;
            }

            tokens.push_back({
                TokenType::NUMBER,
                source.substr(start, pos - start),
                line,
                startCol
            });
            continue;
        }

        // Operators and parentheses
        TokenType type;

        switch (c) {
            case '+': type = TokenType::PLUS; break;
            case '-': type = TokenType::MINUS; break;
            case '*': type = TokenType::MULTIPLY; break;
            case '/': type = TokenType::DIVIDE; break;
            case '(': type = TokenType::LPAREN; break;
            case ')': type = TokenType::RPAREN; break;

            default:
                throw runtime_error(
                    "Invalid character '" + string(1, c) +
                    "' at line " + to_string(line) +
                    ", column " + to_string(column)
                );
        }

        tokens.push_back({
            type, string(1, c), line, column
        });

        pos++;
        column++;
    }

    // Add EOF token
    tokens.push_back({
        TokenType::END_OF_FILE, "", line, column
    });

    return tokens;
}

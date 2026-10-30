#include "lexer.h"
#include <cctype>
#include <stdexcept>
using namespace std;
Lexer::Lexer(const string &input) : source(input) {}
vector<Token> Lexer::tokenize() {
    vector<Token> out;
    while (pos < source.size()) {
        char c = source[pos];
        if (c == ' ' || c == '\t') {
            ++pos;
            ++column;
            continue;
        }
        if (c == '\r' || c == '\n') {
            int col = column;
            if (c == '\r' && pos + 1 < source.size() && source[pos + 1] == '\n')
                pos += 2;
            else
                ++pos;
            out.push_back({TokenType::NEWLINE, "\\n", line++, col});
            column = 1;
            continue;
        }
        if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t start = pos;
            int col = column;
            while (pos < source.size() &&
                   (isalnum(static_cast<unsigned char>(source[pos])) || source[pos] == '_')) {
                ++pos;
                ++column;
            }
            string word = source.substr(start, pos - start);
            TokenType t = TokenType::IDENTIFIER;
            if (word == "SET")
                t = TokenType::SET;
            else if (word == "TO")
                t = TokenType::TO;
            else if (word == "DISPLAY")
                t = TokenType::DISPLAY;
            else if (word == "IF")
                t = TokenType::IF;
            else if (word == "THEN")
                t = TokenType::THEN;
            else if (word == "ELSE")
                t = TokenType::ELSE;
            else if (word == "ENDIF")
                t = TokenType::ENDIF;
            else if (word == "WHILE")
                t = TokenType::WHILE;
            else if (word == "DO")
                t = TokenType::DO;
            else if (word == "ENDWHILE")
                t = TokenType::ENDWHILE;
            else if (word == "FUNCTION")
                t = TokenType::FUNCTION;
            else if (word == "ENDFUNCTION")
                t = TokenType::ENDFUNCTION;
            else if (word == "RETURN")
                t = TokenType::RETURN;
            out.push_back({t, word, line, col});
            continue;
        }
        if (isdigit(static_cast<unsigned char>(c)) ||
            (c == '.' && pos + 1 < source.size() &&
             isdigit(static_cast<unsigned char>(source[pos + 1])))) {
            size_t start = pos;
            int col = column;
            bool dot = false;
            while (pos < source.size() && (isdigit(static_cast<unsigned char>(source[pos])) ||
                                           (source[pos] == '.' && !dot))) {
                if (source[pos] == '.')
                    dot = true;
                ++pos;
                ++column;
            }
            out.push_back({TokenType::NUMBER, source.substr(start, pos - start), line, col});
            continue;
        }
        if (c == '"') {
            int col = column;
            ++pos;
            ++column;
            string value;
            bool closed = false;
            while (pos < source.size()) {
                char ch = source[pos++];
                ++column;
                if (ch == '"') {
                    closed = true;
                    break;
                }
                if (ch == '\\' && pos < source.size()) {
                    char e = source[pos++];
                    ++column;
                    if (e == 'n')
                        value += '\\', value += 'n';
                    else if (e == 't')
                        value += '\\', value += 't';
                    else if (e == '"' || e == '\\')
                        value += e;
                    else
                        throw runtime_error("Lexical error at line " + to_string(line) +
                                            ", column " + to_string(column - 1) +
                                            ": invalid string escape");
                } else {
                    if (ch == '\n' || ch == '\r')
                        throw runtime_error("Lexical error at line " + to_string(line) +
                                            ", column " + to_string(col) + ": unterminated string");
                    value += ch;
                }
            }
            if (!closed)
                throw runtime_error("Lexical error at line " + to_string(line) + ", column " +
                                    to_string(col) + ": unterminated string");
            out.push_back({TokenType::STRING, value, line, col});
            continue;
        }
        int col = column;
        TokenType t;
        std::string spelling(1, c);
        switch (c) {
        case '+':
            t = TokenType::PLUS;
            break;
        case '-':
            t = TokenType::MINUS;
            break;
        case '*':
            t = TokenType::MULTIPLY;
            break;
        case '/':
            t = TokenType::DIVIDE;
            break;
        case '(':
            t = TokenType::LPAREN;
            break;
        case ')':
            t = TokenType::RPAREN;
            break;
        case ',':
            t = TokenType::COMMA;
            break;
        case '<':
            if (pos + 1 < source.size() && source[pos + 1] == '=') {
                t = TokenType::LESS_EQUAL;
                spelling = "<=";
                ++pos;
                ++column;
            } else if (pos + 1 < source.size() && source[pos + 1] == '>') {
                t = TokenType::NOT_EQUAL;
                spelling = "<>";
                ++pos;
                ++column;
            } else
                t = TokenType::LESS;
            break;
        case '>':
            if (pos + 1 < source.size() && source[pos + 1] == '=') {
                t = TokenType::GREATER_EQUAL;
                spelling = ">=";
                ++pos;
                ++column;
            } else
                t = TokenType::GREATER;
            break;
        case '=':
            if (pos + 1 < source.size() && source[pos + 1] == '=') {
                ++pos;
                ++column;
                spelling = "==";
            }
            t = TokenType::EQUAL;
            break;
        case '!':
            if (pos + 1 < source.size() && source[pos + 1] == '=') {
                t = TokenType::NOT_EQUAL;
                spelling = "!=";
                ++pos;
                ++column;
            } else
                throw runtime_error("Lexical error at line " + to_string(line) + ", column " +
                                    to_string(col) + ": expected '=' after '!'");
            break;
        default:
            throw runtime_error("Lexical error at line " + to_string(line) + ", column " +
                                to_string(col) + ": invalid character '" + string(1, c) + "'");
        }
        out.push_back({t, spelling, line, col});
        ++pos;
        ++column;
    }
    out.push_back({TokenType::END_OF_FILE, "", line, column});
    return out;
}

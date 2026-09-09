//
// Created by Admin on 09.09.2026.
//

#include "../index/Lexer.h"
#include <cctype>

namespace {

bool isDigit(char c) { return std::isdigit(static_cast<unsigned char>(c)) != 0; }
bool isSpace(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }

}  // namespace

std::vector<Token> Lexer::tokenize(const std::string &text) {
    std::vector<Token> tokens;

    double fracScale = 0;   // 0 - точки ещё не было; иначе делитель для следующей цифры

    for (std::size_t i = 0; i < text.length(); ++i) {
        if (isSpace(text[i])) { continue; }

        if (isDigit(text[i])) {
            if (                                                // Digit continues current num
                !tokens.empty()
                &&
                tokens.back().type == TokenType::NUMBER
                &&
                tokens.back().end == i
                ) {
                tokens.back().end++;
                if (fracScale > 0) {                            // digit after the dot
                    tokens.back().value += (text[i]-'0') / fracScale;
                    fracScale *= 10;
                } else {
                    tokens.back().value = tokens.back().value * 10 + text[i]-'0';
                }
            } else {                                            // New num token
                tokens.push_back(Token(TokenType::NUMBER, text[i]-'0', i, i+1));
                fracScale = 0;
            }
        }

        else if (text[i] == '.') {
            const bool continuesNumber =
                !tokens.empty()
                &&
                tokens.back().type == TokenType::NUMBER
                &&
                tokens.back().end == i;

            if (!continuesNumber) {                             // ".5", "1 .5"
                throw LexError("digit expected before '.'", i);
            }
            if (fracScale != 0) {                               // "1.2.3"
                throw LexError("second '.' in number", i);
            }
            if (i+1 >= text.length() || !isDigit(text[i+1])) {  // "5.", "5.+"
                throw LexError("digit expected after '.'", i);
            }
            tokens.back().end++;                                // dot belongs to the lexeme
            fracScale = 10;
        }

        else {
            TokenType curTokenType{};
            switch (text[i]) {
                case '(': curTokenType = TokenType::LPAREN; break;
                case ')': curTokenType = TokenType::RPAREN; break;
                case '*': curTokenType = TokenType::STAR; break;
                case '/': curTokenType = TokenType::SLASH; break;
                case '+': curTokenType = TokenType::PLUS; break;
                case '-': curTokenType = TokenType::MINUS; break;
                default:
                    throw LexError(std::string("unexpected character '") + text[i] + "'", i);
            }
            tokens.push_back(Token(curTokenType, 0, i, i+1));
            fracScale = 0;
        }
    }

    // Поток всегда заканчивается пустым END: [n, n).
    tokens.push_back(Token(TokenType::END, 0, text.length(), text.length()));

    return tokens;
}

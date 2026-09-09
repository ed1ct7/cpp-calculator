#ifndef CPPLCCLIMB_TOKEN_H
#define CPPLCCLIMB_TOKEN_H

#include <cstddef>

enum class TokenType {
    NUMBER,
    MINUS,
    PLUS,
    STAR,
    SLASH,
    LPAREN,
    RPAREN,
    END
};

// [start, end) - полуоткрытый диапазон в ИСХОДНОЙ строке.
// Длина лексемы = end - start. У END диапазон пуст: start == end == длина строки.
struct Token {
    TokenType   type;
    double      value;
    std::size_t start;
    std::size_t end;

    Token(TokenType t, double v, std::size_t s, std::size_t e)
        : type(t), value(v), start(s), end(e) {}
};
#endif //CPPLCCLIMB_TOKEN_H

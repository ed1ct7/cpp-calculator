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

struct Token {
    TokenType type;
    int value;
    std::size_t start;
    std::size_t end;
};

#endif //CPPLCCLIMB_TOKEN_H

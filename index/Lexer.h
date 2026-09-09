#ifndef CPPLCCLIMB_LEXER_H
#define CPPLCCLIMB_LEXER_H

#include "Token.h"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

// Лексическая ошибка с точной позицией в исходной строке.
// position() - индекс символа (0-based); для печати столбца это position() + 1.
class LexError : public std::runtime_error {
public:
    LexError(const std::string &message, std::size_t position)
        : std::runtime_error(message), pos(position) {}

    std::size_t position() const noexcept { return pos; }

private:
    std::size_t pos;
};

class Lexer {
public:
    // Разбирает строку в поток токенов. Последний токен всегда END.
    // Входная строка не меняется, позиции указывают в неё.
    // Бросает LexError на первом же недопустимом символе.
    static std::vector<Token> tokenize(const std::string &text);
};

#endif //CPPLCCLIMB_LEXER_H

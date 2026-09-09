#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "Lexer.h"
#include "Token.h"

namespace {

const char *typeName(TokenType t) {
    switch (t) {
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::MINUS:  return "MINUS";
        case TokenType::PLUS:   return "PLUS";
        case TokenType::STAR:   return "STAR";
        case TokenType::SLASH:  return "SLASH";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::END:    return "END";
    }
    return "???";
}

// Печатает всё, что вернул лексер, независимо от того, верно это или нет.
void dump(const std::string &src, const std::vector<Token> &tokens) {
    std::cout << "tokenize(\"" << src << "\") -> " << tokens.size() << " tokens\n";
    for (std::size_t k = 0; k < tokens.size(); ++k) {
        const Token &t = tokens[k];
        std::cout << "  [" << k << "] " << typeName(t.type)
                  << "  value=" << t.value
                  << "  start=" << t.start
                  << "  end=" << t.end << '\n';
    }
    std::cout.flush();
}

}  // namespace

// "143" - это одно число, а не три отдельные цифры.
// Лексер должен склеить подряд идущие цифры в один токен NUMBER.
TEST(LexerTest, MergesConsecutiveDigitsIntoSingleNumberToken) {
    // Arrange
    const std::string input = "143";

    // Act
    const std::vector<Token> tokens = Lexer::tokenize(input);

    // Assert
    dump(input, tokens);

    ASSERT_EQ(tokens.size(), 1u) << "ожидался ровно один токен на \"143\"";

    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(tokens[0].value, 143);

    // Границы токена включительные: "143" -> start = 0, end = 2.
    EXPECT_EQ(tokens[0].start, 0u);
    EXPECT_EQ(tokens[0].end, 2u);
}

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
                  << "  [" << t.start << ", " << t.end << ")\n";
    }
    std::cout.flush();
}

// Ожидаемый токен. start/end - полуоткрытый диапазон в ИСХОДНОЙ строке:
// end указывает на символ ЗА лексемой, длина = end - start.
// value - double, потому что числа могут быть дробными.
struct Expected {
    TokenType   type;
    double      value;
    std::size_t start;
    std::size_t end;
};

// Прогоняет лексер и сверяет результат потокенно.
// Замыкающий END в таблицах не пишем: он есть всегда и проверяется отдельно.
void expectTokens(const std::string &input, const std::vector<Expected> &expected) {
    SCOPED_TRACE("input = \"" + input + "\"");

    std::vector<Token> tokens;
    ASSERT_NO_THROW(tokens = Lexer::tokenize(input));

    if (tokens.size() != expected.size() + 1) {
        dump(input, tokens);
    }
    ASSERT_EQ(tokens.size(), expected.size() + 1);

    // Контракт потока: последний токен - пустой END на конце строки.
    const Token &last = tokens.back();
    EXPECT_EQ(last.type, TokenType::END) << "got " << typeName(last.type);
    EXPECT_EQ(last.start, input.size());
    EXPECT_EQ(last.end,   input.size());

    for (std::size_t i = 0; i < expected.size(); ++i) {
        SCOPED_TRACE("token index " + std::to_string(i));
        EXPECT_EQ(tokens[i].type, expected[i].type)
            << "got " << typeName(tokens[i].type)
            << ", expected " << typeName(expected[i].type);
        EXPECT_DOUBLE_EQ(tokens[i].value, expected[i].value);
        EXPECT_EQ(tokens[i].start, expected[i].start);
        EXPECT_EQ(tokens[i].end,   expected[i].end);
    }
}

// Ждёт отказа: LexError с точным сообщением и точной позицией.
void expectLexError(const std::string &input,
                    std::size_t position,
                    const std::string &message) {
    SCOPED_TRACE("input = \"" + input + "\"");
    try {
        const std::vector<Token> tokens = Lexer::tokenize(input);
        dump(input, tokens);
        ADD_FAILURE() << "expected LexError, but tokenize() succeeded";
    } catch (const LexError &e) {
        EXPECT_EQ(e.position(), position);
        EXPECT_EQ(std::string(e.what()), message);
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Числа
// ---------------------------------------------------------------------------

// Подряд идущие цифры склеиваются в один токен NUMBER,
// но пробел между ними разрывает число.
TEST(LexerTest, SpaceSeparatedNumbersAreSeparateTokens) {
    expectTokens("143 645 654", {
        {TokenType::NUMBER, 143, 0,  3},
        {TokenType::NUMBER, 645, 4,  7},
        {TokenType::NUMBER, 654, 8, 11},
    });
}

TEST(LexerTest, EmptyInputProducesOnlyEndToken) {
    expectTokens("", {});
}

TEST(LexerTest, OnlySpacesProduceOnlyEndToken) {
    expectTokens("     ", {});
}

TEST(LexerTest, SingleDigit) {
    expectTokens("7", {{TokenType::NUMBER, 7, 0, 1}});
}

TEST(LexerTest, MultiDigitNumberSpansItsWholeRange) {
    expectTokens("1234567890", {{TokenType::NUMBER, 1234567890, 0, 10}});
}

TEST(LexerTest, LeadingZerosDoNotChangeValueButCountInRange) {
    expectTokens("007", {{TokenType::NUMBER, 7, 0, 3}});
}

TEST(LexerTest, ZeroIsANumberToken) {
    expectTokens("0", {{TokenType::NUMBER, 0, 0, 1}});
}

TEST(LexerTest, NumberAfterOperatorStartsNewToken) {
    expectTokens("1+2", {
        {TokenType::NUMBER, 1, 0, 1},
        {TokenType::PLUS,   0, 1, 2},
        {TokenType::NUMBER, 2, 2, 3},
    });
}

TEST(LexerTest, NumberNotAtStartOfInputGetsCorrectPositions) {
    expectTokens("+42", {
        {TokenType::PLUS,   0,  0, 1},
        {TokenType::NUMBER, 42, 1, 3},
    });
}

TEST(LexerTest, DigitsSeparatedBySpaceAreTwoNumbers) {
    expectTokens("1 2", {
        {TokenType::NUMBER, 1, 0, 1},
        {TokenType::NUMBER, 2, 2, 3},
    });
}

// ---------------------------------------------------------------------------
// Дробные числа
// ---------------------------------------------------------------------------

// Точка входит в лексему числа, поэтому диапазон покрывает обе части.
TEST(LexerTest, FractionalNumberSpansDotAndBothParts) {
    expectTokens("12.5", {{TokenType::NUMBER, 12.5, 0, 4}});
}

TEST(LexerTest, FractionWithSeveralDecimals) {
    expectTokens("0.125", {{TokenType::NUMBER, 0.125, 0, 5}});
}

// ---------------------------------------------------------------------------
// Отказы: формат числа
// ---------------------------------------------------------------------------

// "5." - незаконченное число, а не число 5 с довеском.
TEST(LexerTest, TrailingDotIsRejected) {
    expectLexError("12.", 2, "digit expected after '.'");
}

// Точка в конце числа не спасается и следующим оператором.
TEST(LexerTest, DotFollowedByOperatorIsRejected) {
    expectLexError("12.+3", 2, "digit expected after '.'");
}

// ".5" - тоже не число: цифра слева обязательна.
TEST(LexerTest, LeadingDotIsRejected) {
    expectLexError(".5", 0, "digit expected before '.'");
}

// Пробел разрывает число, поэтому точка после него уже ни к чему не примыкает.
TEST(LexerTest, DotAfterSpaceIsRejected) {
    expectLexError("1 .5", 2, "digit expected before '.'");
}

TEST(LexerTest, SecondDotInNumberIsRejected) {
    expectLexError("1.2.3", 3, "second '.' in number");
}

// ---------------------------------------------------------------------------
// Отказы: посторонние символы
// ---------------------------------------------------------------------------

TEST(LexerTest, UnknownCharacterIsRejected) {
    expectLexError("$", 0, "unexpected character '$'");
}

TEST(LexerTest, LetterIsRejected) {
    expectLexError("ab", 0, "unexpected character 'a'");
}

// Выход из калькулятора - дело REPL, для лексера 'e' обычная посторонняя буква.
TEST(LexerTest, LetterEIsRejectedLikeAnyOtherLetter) {
    expectLexError("e", 0, "unexpected character 'e'");
}

// Экспоненциальная запись форматом не поддерживается.
TEST(LexerTest, ExponentNotationIsRejected) {
    expectLexError("1e3", 1, "unexpected character 'e'");
}

// Позиция указывает ровно на плохой символ, а не на начало строки.
TEST(LexerTest, ErrorPositionPointsAtTheOffendingCharacter) {
    expectLexError("12 + $", 5, "unexpected character '$'");
}

// Отказ происходит на первой же ошибке, дальше строка не разбирается.
TEST(LexerTest, FirstErrorWins) {
    expectLexError("1 $ %", 2, "unexpected character '$'");
}

// ---------------------------------------------------------------------------
// Операторы и скобки
// ---------------------------------------------------------------------------

TEST(LexerTest, PlusToken) {
    expectTokens("+", {{TokenType::PLUS, 0, 0, 1}});
}

TEST(LexerTest, MinusToken) {
    expectTokens("-", {{TokenType::MINUS, 0, 0, 1}});
}

TEST(LexerTest, StarToken) {
    expectTokens("*", {{TokenType::STAR, 0, 0, 1}});
}

TEST(LexerTest, SlashToken) {
    expectTokens("/", {{TokenType::SLASH, 0, 0, 1}});
}

TEST(LexerTest, ParenTokens) {
    expectTokens("()", {
        {TokenType::LPAREN, 0, 0, 1},
        {TokenType::RPAREN, 0, 1, 2},
    });
}

TEST(LexerTest, AllOperatorsInARow) {
    expectTokens("+-*/()", {
        {TokenType::PLUS,   0, 0, 1},
        {TokenType::MINUS,  0, 1, 2},
        {TokenType::STAR,   0, 2, 3},
        {TokenType::SLASH,  0, 3, 4},
        {TokenType::LPAREN, 0, 4, 5},
        {TokenType::RPAREN, 0, 5, 6},
    });
}

// ---------------------------------------------------------------------------
// Контракт диапазонов и END
// ---------------------------------------------------------------------------

// END всегда последний и всегда пуст: [n, n).
TEST(LexerTest, EndTokenIsLastAndEmpty) {
    const std::string input = "1+2";

    const std::vector<Token> tokens = Lexer::tokenize(input);

    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(tokens.back().type,  TokenType::END);
    EXPECT_EQ(tokens.back().start, input.size());
    EXPECT_EQ(tokens.back().end,   input.size());

    // END ровно один, и он не встречается в середине потока.
    for (std::size_t i = 0; i + 1 < tokens.size(); ++i) {
        EXPECT_NE(tokens[i].type, TokenType::END) << "at index " << i;
    }
}

TEST(LexerTest, EmptyInputStillEndsWithEndToken) {
    const std::string input;

    const std::vector<Token> tokens = Lexer::tokenize(input);

    ASSERT_EQ(tokens.size(), 1u);
    EXPECT_EQ(tokens[0].type,  TokenType::END);
    EXPECT_EQ(tokens[0].start, 0u);
    EXPECT_EQ(tokens[0].end,   0u);
}

// Длина лексемы = end - start, и подстрока по этому диапазону - сам текст токена.
TEST(LexerTest, RangeLengthMatchesLexemeText) {
    const std::string input = "12.5 + (340 - 6)";

    const std::vector<Token> tokens = Lexer::tokenize(input);

    const std::vector<std::string> lexemes = {
        "12.5", "+", "(", "340", "-", "6", ")", ""
    };
    ASSERT_EQ(tokens.size(), lexemes.size());

    for (std::size_t i = 0; i < tokens.size(); ++i) {
        SCOPED_TRACE("token index " + std::to_string(i));
        ASSERT_LE(tokens[i].start, tokens[i].end);
        ASSERT_LE(tokens[i].end, input.size());
        EXPECT_EQ(tokens[i].end - tokens[i].start, lexemes[i].size());
        EXPECT_EQ(input.substr(tokens[i].start, tokens[i].end - tokens[i].start),
                  lexemes[i]);
    }
}

// ---------------------------------------------------------------------------
// Пробелы и входная строка
// ---------------------------------------------------------------------------

// Пропускается любой пробельный символ, а не только ' '.
TEST(LexerTest, TabIsSkippedLikeSpace) {
    expectTokens("1\t2", {
        {TokenType::NUMBER, 1, 0, 1},
        {TokenType::NUMBER, 2, 2, 3},
    });
}

TEST(LexerTest, NewlineIsSkippedLikeSpace) {
    expectTokens("1\n+\n2", {
        {TokenType::NUMBER, 1, 0, 1},
        {TokenType::PLUS,   0, 2, 3},
        {TokenType::NUMBER, 2, 4, 5},
    });
}

// tokenize() принимает const-ссылку и строку вызывающего не трогает.
TEST(LexerTest, TokenizeDoesNotModifyCallerString) {
    std::string input = "1 + 2";

    Lexer::tokenize(input);

    EXPECT_EQ(input, "1 + 2");
}

// Пробелы пропускаются, но не удаляются: позиции указывают в исходную строку.
TEST(LexerTest, PositionsReferToOriginalString) {
    expectTokens(" 1 +  2 ", {
        {TokenType::NUMBER, 1, 1, 2},
        {TokenType::PLUS,   0, 3, 4},
        {TokenType::NUMBER, 2, 6, 7},
    });
}

// ---------------------------------------------------------------------------
// Выражения целиком
// ---------------------------------------------------------------------------

TEST(LexerTest, FullExpressionWithParens) {
    expectTokens("12+34*(5-6)/7", {
        {TokenType::NUMBER, 12,  0,  2},
        {TokenType::PLUS,    0,  2,  3},
        {TokenType::NUMBER, 34,  3,  5},
        {TokenType::STAR,    0,  5,  6},
        {TokenType::LPAREN,  0,  6,  7},
        {TokenType::NUMBER,  5,  7,  8},
        {TokenType::MINUS,   0,  8,  9},
        {TokenType::NUMBER,  6,  9, 10},
        {TokenType::RPAREN,  0, 10, 11},
        {TokenType::SLASH,   0, 11, 12},
        {TokenType::NUMBER,  7, 12, 13},
    });
}

TEST(LexerTest, FullExpressionWithFractions) {
    expectTokens("1.5*(2.25-0.5)", {
        {TokenType::NUMBER, 1.5,   0,  3},
        {TokenType::STAR,   0,     3,  4},
        {TokenType::LPAREN, 0,     4,  5},
        {TokenType::NUMBER, 2.25,  5,  9},
        {TokenType::MINUS,  0,     9, 10},
        {TokenType::NUMBER, 0.5,  10, 13},
        {TokenType::RPAREN, 0,    13, 14},
    });
}

TEST(LexerTest, NestedParens) {
    expectTokens("((1))", {
        {TokenType::LPAREN, 0, 0, 1},
        {TokenType::LPAREN, 0, 1, 2},
        {TokenType::NUMBER, 1, 2, 3},
        {TokenType::RPAREN, 0, 3, 4},
        {TokenType::RPAREN, 0, 4, 5},
    });
}

// Лексер не проверяет синтаксис: набор из допустимых символов разбирается
// на токены, несогласованность скобок и операторов ловит парсер.
TEST(LexerTest, MalformedExpressionStillTokenizes) {
    expectTokens(")+*(", {
        {TokenType::RPAREN, 0, 0, 1},
        {TokenType::PLUS,   0, 1, 2},
        {TokenType::STAR,   0, 2, 3},
        {TokenType::LPAREN, 0, 3, 4},
    });
}

// Лексер не имеет состояния между вызовами: повторный прогон той же строки
// даёт тот же результат.
TEST(LexerTest, TokenizeIsRepeatable) {
    const std::string input = "10 - 2";

    const std::vector<Token> first  = Lexer::tokenize(input);
    const std::vector<Token> second = Lexer::tokenize(input);

    ASSERT_EQ(first.size(), second.size());
    for (std::size_t i = 0; i < first.size(); ++i) {
        SCOPED_TRACE("token index " + std::to_string(i));
        EXPECT_EQ(first[i].type,  second[i].type);
        EXPECT_DOUBLE_EQ(first[i].value, second[i].value);
        EXPECT_EQ(first[i].start, second[i].start);
        EXPECT_EQ(first[i].end,   second[i].end);
    }
}

// После отказа лексер тоже остаётся пригодным: следующая строка разбирается.
TEST(LexerTest, LexerIsUsableAfterError) {
    EXPECT_THROW(Lexer::tokenize("1 $ 2"), LexError);

    expectTokens("1+2", {
        {TokenType::NUMBER, 1, 0, 1},
        {TokenType::PLUS,   0, 1, 2},
        {TokenType::NUMBER, 2, 2, 3},
    });
}

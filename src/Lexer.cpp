//
// Created by Admin on 09.09.2026.
//

#include "../index/Lexer.h"
#include <cctype>

std::vector<Token> Lexer::tokenize(const std::string &text) {
    std::vector<Token> tokens;

    int cursor = 0;

    for (auto i = 0; i < text.length(); ++i) {

        if (tokens.empty()) {
            Token curToken{};
            TokenType curTokenType{};
            if (std::isdigit(text[i])) {
                curTokenType = TokenType::NUMBER;
                curToken = Token(curTokenType, text[0]-'0', 0, 0);
                tokens.push_back(curToken);
                continue;
            }
            if (text[i] == '(') curTokenType = TokenType::LPAREN;
            if (text[i] == ')') curTokenType = TokenType::RPAREN;
            if (text[i] == 'e') curTokenType = TokenType::END;
            if (text[i] == '-') curTokenType = TokenType::MINUS;
            if (text[i] == '+') curTokenType = TokenType::PLUS;
            if (text[i] == '*') curTokenType = TokenType::STAR;
            if (text[i] == '/') curTokenType = TokenType::SLASH;
            curToken = Token(curTokenType, 0, 0, 0);
            tokens.push_back(curToken);
            cursor = 0;
            continue;
        }

        if (std::isdigit(text[i])) {
            if (                                                // Current token is num
                tokens.back().type == TokenType::NUMBER
                &&
                tokens.back().end == i-1
                ) {
                tokens.back().end++;
                tokens.back().value = tokens.back().value * 10 + text[i]-'0';
            } else {                                            // New num token
                auto curToken = Token(TokenType::NUMBER, static_cast<int>(i), i, i);
                tokens.push_back(curToken);
                cursor = i;
            }
        }

    }
    
    return tokens;
}


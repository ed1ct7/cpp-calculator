#ifndef CPPLCCLIMB_LEXER_H
#define CPPLCCLIMB_LEXER_H

#include "Token.h"
#include <map>
#include <string>
#include <vector>

class Lexer {
    public:
    Lexer();
    static std::vector<Token> tokenize(const std::string &text);

private:

};

#endif //CPPLCCLIMB_LEXER_H

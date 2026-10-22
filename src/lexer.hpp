#pragma once

#include "token.hpp"

#include <string>
#include <vector>

namespace dotdash {

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::vector<Token> tokenize();

private:
    std::string source;
    std::size_t current = 0;
    std::size_t line = 1;
    std::size_t column = 1;
};

} // namespace dotdash
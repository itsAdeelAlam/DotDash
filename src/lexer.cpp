#include "lexer.hpp"

#include <cctype>
#include <string>

namespace dotdash
{

    // Stores the source code and prepares the lexer.
    Lexer::Lexer(const std::string &source)
        : source(source)
    {
    }

    // Returns true when there are no characters left to read.
    bool Lexer::isAtEnd() const
    {
        return current >= source.size();
    }

    // Returns the current character without advancing the position.
    char Lexer::peek() const
    {
        if (isAtEnd())
        {
            return '\0';
        }

        return source[current];
    }

    // Returns the character immediately after the current character.
    char Lexer::peekNext() const
    {
        if (current + 1 >= source.size())
        {
            return '\0';
        }

        return source[current + 1];
    }

    // Reads the current character and advances the source position.
    char Lexer::advance()
    {
        if (isAtEnd())
        {
            return '\0';
        }

        char character = source[current++];

        if (character == '\n')
        {
            ++line;
            column = 1;
            atLineStart = true;
        }
        else
        {
            ++column;
        }

        return character;
    }

    // Consumes the current character only when it matches expected.
    bool Lexer::match(char expected)
    {
        if (isAtEnd() || peek() != expected)
        {
            return false;
        }

        advance();
        return true;
    }

    // Adds a token with its original text and starting source location.
    void Lexer::addToken(
        TokenType type,
        const std::string &lexeme)
    {
        tokens.push_back({type,
                          lexeme,
                          tokenStartLine,
                          tokenStartColumn});
    }

    // Checks whether the source code at the current position begins
    // with the DotDash comment marker "--".
    // Returns true if the marker is found; otherwise, returns false.
    bool Lexer::isCommentStart() const
    {
        if (source.compare(current, 2, "--") != 0)
        {
            return false;
        }

        const std::size_t next = current + 2;

        return next >= source.size() ||
               std::isspace(
                   static_cast<unsigned char>(source[next]));
    }

    // Skips the remainder of the current line.
    // The newline is left for the normal scanner to process.
    void Lexer::skipComment()
    {
        while (!isAtEnd() && peek() != '\n')
        {
            advance();
        }
    }

    // Reads a quoted string.
    //
    // A string begins with " and ends at the next ".
    // Spaces inside the string are preserved.
    // Escape sequences and multiline strings are not supported.
    void Lexer::scanString()
    {
        const std::size_t startIndex = current;

        // Consume the opening quotation mark.
        advance();

        while (!isAtEnd() && peek() != '"' && peek() != '\n')
        {
            advance();
        }

        if (match('"'))
        {
            if (!isAtEnd() &&
                !std::isspace(static_cast<unsigned char>(peek())))
            {
                // write a loop here to consume the rest of the string until the next whitespace or end of line or end of file, and then add an Invalid token for the rest of the string
                while (!isAtEnd() && !std::isspace(static_cast<unsigned char>(peek())) )
                {
                    advance();
                } 

                addToken(TokenType::Invalid,
                         source.substr(startIndex, current - startIndex));
                return;
            }

            addToken(
                TokenType::StringLiteral,
                source.substr(startIndex, current - startIndex));
            return;
        }

        addToken(
            TokenType::Invalid,
            source.substr(startIndex, current - startIndex));
    }

    // Reads one whitespace-delimited token and determines its type.
    //
    // This routine handles complete tokens such as identifiers, numbers,
    // Boolean values, operators, and keywords. Strings are handled by
    // scanString() because they may contain spaces.
    void Lexer::scanWhitespaceDelimitedToken()
    {
        const std::size_t startIndex = current;

        while (!isAtEnd() &&
               !std::isspace(
                   static_cast<unsigned char>(peek())))
        {
            advance();
        }

        const std::string lexeme =
            source.substr(startIndex, current - startIndex);

        if (lexeme.empty())
        {
            advance();
            return;
        }

        // Boolean literals.
        if (lexeme == ".")
        {
            addToken(TokenType::BooleanTrue, lexeme);
            return;
        }

        if (lexeme == "-")
        {
            addToken(TokenType::BooleanFalse, lexeme);
            return;
        }

        // DotDash operators and keywords.
        // Each spelling follows the current Operators and Keywords documents.
        if (lexeme == "..")
        {
            addToken(TokenType::Assignment, lexeme);
        }
        else if (lexeme == ".-")
        {
            addToken(TokenType::Plus, lexeme);
        }
        else if (lexeme == "-.")
        {
            addToken(TokenType::Minus, lexeme);
        }
        else if (lexeme == "..-")
        {
            addToken(TokenType::Multiply, lexeme);
        }
        else if (lexeme == "--.")
        {
            addToken(TokenType::Divide, lexeme);
        }
        else if (lexeme == ".--")
        {
            addToken(TokenType::Modulo, lexeme);
        }
        else if (lexeme == "-..")
        {
            addToken(TokenType::Power, lexeme);
        }
        else if (lexeme == "...")
        {
            addToken(TokenType::Equal, lexeme);
        }
        else if (lexeme == "---")
        {
            addToken(TokenType::NotEqual, lexeme);
        }
        else if (lexeme == "--..")
        {
            addToken(TokenType::LessThan, lexeme);
        }
        else if (lexeme == "..--")
        {
            addToken(TokenType::GreaterThan, lexeme);
        }
        else if (lexeme == "-.-.")
        {
            addToken(TokenType::LessThanOrEqual, lexeme);
        }
        else if (lexeme == ".-.-")
        {
            addToken(TokenType::GreaterThanOrEqual, lexeme);
        }
        else if (lexeme == "....")
        {
            addToken(TokenType::LogicalAnd, lexeme);
        }
        else if (lexeme == "----")
        {
            addToken(TokenType::LogicalOr, lexeme);
        }
        else if (lexeme == "-.-")
        {
            addToken(TokenType::LogicalNot, lexeme);
        }
        else if (lexeme == "-...-")
        {
            addToken(TokenType::Let, lexeme);
        }
        else if (lexeme == ".---.")
        {
            addToken(TokenType::Const, lexeme);
        }
        else if (lexeme == ".-..")
        {
            addToken(TokenType::If, lexeme);
        }
        else if (lexeme == ".--.")
        {
            addToken(TokenType::Elif, lexeme);
        }
        else if (lexeme == ".---")
        {
            addToken(TokenType::Else, lexeme);
        }
        else if (lexeme == "-.--")
        {
            addToken(TokenType::While, lexeme);
        }
        else if (lexeme == "-...")
        {
            addToken(TokenType::Break, lexeme);
        }
        else if (lexeme == "-..-")
        {
            addToken(TokenType::Continue, lexeme);
        }
        else if (lexeme == "...---")
        {
            addToken(TokenType::Input, lexeme);
        }
        else if (lexeme == "---...")
        {
            addToken(TokenType::Output, lexeme);
        }
        else
        {
            // Determine whether the token is a number.
            // A decimal point is permitted once, between digits.
            bool isNumber = !lexeme.empty();
            bool hasDecimalPoint = false;
            std::size_t digitStart = 0;

            // A leading minus is accepted as part of a negative number.
            if (lexeme[0] == '-')
            {
                digitStart = 1;
            }

            if (digitStart == lexeme.size())
            {
                isNumber = false;
            }

            for (std::size_t i = digitStart;
                 i < lexeme.size();
                 ++i)
            {
                const char character = lexeme[i];

                if (std::isdigit(
                        static_cast<unsigned char>(character)))
                {
                    continue;
                }

                if (character == '.' &&
                    !hasDecimalPoint &&
                    i > digitStart &&
                    i + 1 < lexeme.size() &&
                    std::isdigit(
                        static_cast<unsigned char>(lexeme[i + 1])))
                {
                    hasDecimalPoint = true;
                    continue;
                }

                isNumber = false;
                break;
            }

            if (isNumber)
            {
                addToken(
                    hasDecimalPoint
                        ? TokenType::DoubleLiteral
                        : TokenType::IntegerLiteral,
                    lexeme);
                return;
            }

            // Identifiers start with a letter or underscore.
            // Remaining characters may be letters, digits, or underscores.
            bool isIdentifier = !lexeme.empty();

            if (isIdentifier)
            {
                const unsigned char first =
                    static_cast<unsigned char>(lexeme[0]);

                isIdentifier =
                    std::isalpha(first) || lexeme[0] == '_';
            }

            for (std::size_t i = 1;
                 isIdentifier && i < lexeme.size();
                 ++i)
            {
                const unsigned char character =
                    static_cast<unsigned char>(lexeme[i]);

                if (!std::isalnum(character) &&
                    lexeme[i] != '_')
                {
                    isIdentifier = false;
                }
            }

            if (isIdentifier)
            {
                addToken(TokenType::Identifier, lexeme);
            }
            else
            {
                addToken(TokenType::Invalid, lexeme);
            }
        }
    }

    void Lexer::handleIndentation()
    {
        if (!atLineStart)
        {
            return;
        }

        const std::size_t indentationStart = current;
        std::size_t indentationWidth = 0;

        // Measure the indentation at the beginning of the line.
        while (!isAtEnd() &&
               (peek() == ' ' || peek() == '\t'))
        {
            if (peek() == '\t')
            {
                // Advance tabs to the next multiple of four columns.
                indentationWidth =
                    ((indentationWidth / 4) + 1) * 4;
            }
            else
            {
                ++indentationWidth;
            }

            advance();
        }

        // Blank lines do not change the indentation stack.
        if (isAtEnd() || peek() == '\n' || peek() == '\r')
        {
            return;
        }

        // Comment-only lines do not change the indentation stack.
        if (isCommentStart())
        {
            return;
        }

        atLineStart = false;

        const std::size_t currentIndentation =
            indentationStack.back();

        // A greater indentation opens a new block.
        if (indentationWidth > currentIndentation)
        {
            indentationStack.push_back(indentationWidth);

            tokenStartLine = line;
            tokenStartColumn = 1;

            addToken(
                TokenType::Indent,
                source.substr(
                    indentationStart,
                    current - indentationStart));

            return;
        }

        // A smaller indentation closes one or more blocks.
        if (indentationWidth < currentIndentation)
        {
            while (indentationStack.size() > 1 &&
                   indentationWidth < indentationStack.back())
            {
                indentationStack.pop_back();

                tokenStartLine = line;
                tokenStartColumn = 1;

                addToken(
                    TokenType::Dedent,
                    source.substr(
                        indentationStart,
                        current - indentationStart));
            }

            // The indentation must match an existing level.
            if (indentationWidth != indentationStack.back())
            {
                tokenStartLine = line;
                tokenStartColumn = 1;

                addToken(
                    TokenType::Invalid,
                    "<invalid indentation>");

                // Recover using the new width without adding a stack level.
                indentationStack.back() = indentationWidth;
            }
        }
    }

    // Tokenizes the complete source code.
    //
    // The lexer can be called more than once on the same instance.
    // Each call starts from the beginning and returns a fresh token sequence.
    std::vector<Token> Lexer::tokenize()
    {
        current = 0;
        line = 1;
        column = 1;
        tokenStartLine = 1;
        tokenStartColumn = 1;
        atLineStart = true;
        indentationStack.clear();
        indentationStack.push_back(0);
        tokens.clear();

        while (!isAtEnd())
        {
            // Handle indentation before reading the next line's tokens.
            if (atLineStart)
            {
                handleIndentation();
            }

            if (isAtEnd())
            {
                break;
            }

            // Comments are discarded; their newline remains significant.
            if (isCommentStart())
            {
                skipComment();
                continue;
            }

            scanToken();
        }

        // Add a final newline if the last statement needs one.
        if (!tokens.empty() &&
            tokens.back().type != TokenType::Newline &&
            tokens.back().type != TokenType::Indent &&
            tokens.back().type != TokenType::Dedent)
        {
            tokenStartLine = line;
            tokenStartColumn = column;
            addToken(TokenType::Newline, "\n");
        }

        // Close any remaining indentation blocks.
        while (indentationStack.size() > 1)
        {
            indentationStack.pop_back();

            tokenStartLine = line;
            tokenStartColumn = 1;
            addToken(TokenType::Dedent, "");
        }

        // Mark the end of the source.
        tokenStartLine = line;
        tokenStartColumn = column;
        addToken(TokenType::EndOfFile, "");

        return tokens;
    }

} // namespace dotdash
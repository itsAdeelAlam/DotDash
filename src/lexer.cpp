#include "lexer.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace dotdash
{
    namespace
    {
        // ---------------------------------------------------------------
        // Character helpers
        //
        // These are written by hand instead of using <cctype> because:
        //  - they are locale-independent, so results never change between
        //    machines,
        //  - they need no unsigned char casts,
        //  - they are trivially inlined.
        // DotDash source is ASCII, so this is sufficient.
        // ---------------------------------------------------------------
        constexpr bool isSpace(char c)
        {
            return c == ' ' || c == '\t' || c == '\n' ||
                   c == '\r' || c == '\v' || c == '\f';
        }

        constexpr bool isDigit(char c)
        {
            return c >= '0' && c <= '9';
        }

        constexpr bool isIdentifierStart(char c)

        {
            return (c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z') ||
                   c == '_';
        }

        constexpr bool isIdentifierPart(char c)
        {
            return isIdentifierStart(c) || isDigit(c);
        }

        // ---------------------------------------------------------------
        // Fixed spellings: Boolean literals, operators and keywords.
        //
        // A hash lookup replaces the long if / else-if chain. It costs one
        // hash per token no matter how many spellings exist. The keys are
        // string_views of string literals, so no allocation happens when
        // looking a lexeme up.
        // ---------------------------------------------------------------
        const std::unordered_map<std::string_view, TokenType> &fixedSpellings()
        {
            static const std::unordered_map<std::string_view, TokenType> table = {
                // Boolean literals.
                {".", TokenType::BooleanTrue},
                {"-", TokenType::BooleanFalse},

                // Assignment and arithmetic operators.
                {"..", TokenType::Assignment},
                {".-", TokenType::Plus},
                {"-.", TokenType::Minus},
                {"..-", TokenType::Multiply},
                {"--.", TokenType::Divide},

                {".--", TokenType::Modulo},
                {"-..", TokenType::Power},

                // Comparison operators.
                {"...", TokenType::Equal},
                {"---", TokenType::NotEqual},
                {"--..", TokenType::LessThan},
                {"..--", TokenType::GreaterThan},
                {"-.-.", TokenType::LessThanOrEqual},
                {".-.-", TokenType::GreaterThanOrEqual},

                // Logical operators.
                {"....", TokenType::LogicalAnd},
                {"----", TokenType::LogicalOr},
                {"-.-", TokenType::LogicalNot},

                // Keywords.
                {"-...-", TokenType::Let},
                {".---.", TokenType::Const},
                {".-..", TokenType::If},
                {".--.", TokenType::Elif},
                {".---", TokenType::Else},
                {"-.--", TokenType::While},
                {"-...", TokenType::Break},
                {"-..-", TokenType::Continue},
                {"...---", TokenType::Input},
                {"---...", TokenType::Output},
            };

            return table;
        }

        // Classifies a lexeme that is not a fixed spelling. Returns one of
        // IntegerLiteral, DoubleLiteral, Identifier or Invalid.
        //
        // Looking at the first character is enough to decide which rule
        // can apply, because the rules cannot overlap:
        //   digit or '-'            -> must be a number
        //   letter or '_'           -> must be an identifier
        //   anything else           -> invalid
        TokenType classifyOperand(std::string_view lexeme)
        {
            const char first = lexeme.front();

            // Number: optional leading '-', digits, and at most one decimal
            // point that has a digit on each side ("1.5" is valid, "1." and
            // ".5" are not).
            if (isDigit(first) || first == '-')
            {
                const std::size_t digitStart = (first == '-') ? 1 : 0;

                // A lone "-" is a Boolean and was handled before we got here,
                // but guard anyway so lexeme[digitStart] is always valid.
                if (digitStart == lexeme.size())
                {
                    return TokenType::Invalid;
                }

                bool hasDecimalPoint = false;

                for (std::size_t i = digitStart; i < lexeme.size(); ++i)
                {
                    const char c = lexeme[i];

                    if (isDigit(c))
                    {
                        continue;
                    }

                    const bool validDecimalPoint =
                        c == '.' &&
                        !hasDecimalPoint &&
                        i > digitStart &&
                        i + 1 < lexeme.size() &&
                        isDigit(lexeme[i + 1]);

                    if (!validDecimalPoint)
                    {
                        return TokenType::Invalid;
                    }

                    hasDecimalPoint = true;
                }

                return hasDecimalPoint ? TokenType::DoubleLiteral
                                       : TokenType::IntegerLiteral;
            }

            // Identifier: starts with a letter or '_', followed by letters,
            // digits or '_'.
            if (isIdentifierStart(first))
            {
                for (std::size_t i = 1; i < lexeme.size(); ++i)
                {
                    if (!isIdentifierPart(lexeme[i]))
                    {

                        return TokenType::Invalid;
                    }
                }

                return TokenType::Identifier;
            }

            return TokenType::Invalid;
        }

    } // namespace

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

    // Returns the current character without advancing, or '\0' at the end.
    char Lexer::peek() const
    {
        return isAtEnd() ? '\0' : source[current];
    }

    // Returns the character after the current one, or '\0' past the end.
    char Lexer::peekNext() const

    {
        return (current + 1 < source.size()) ? source[current + 1] : '\0';
    }

    // Reads the current character and advances, keeping line and column
    // up to date. Use this for single characters that may be a newline.
    // Code that skips a run known to contain no newline updates
    // current/column directly instead, which is faster.
    char Lexer::advance()
    {
        if (isAtEnd())
        {
            return '\0';
        }

        const char character = source[current++];

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
        if (isAtEnd() || source[current] != expected)
        {
            return false;
        }

        advance();
        return true;
    }

    // Adds a token with its original text and the starting location that
    // was recorded in tokenStartLine / tokenStartColumn.
    void Lexer::addToken(
        TokenType type,
        const std::string &lexeme)
    {
        tokens.push_back({type,
                          lexeme,
                          tokenStartLine,
                          tokenStartColumn});
    }

    // Returns true when the current position starts a comment.
    //
    // A comment is "--" followed by whitespace or the end of the file.
    // The extra condition is what separates a comment from operators such
    // as "--." (divide) and "---" (not equal).
    bool Lexer::isCommentStart() const
    {
        if (peek() != '-' || peekNext() != '-')
        {

            return false;
        }

        const std::size_t afterMarker = current + 2;

        return afterMarker >= source.size() ||
               isSpace(source[afterMarker]);
    }

    // Skips the rest of the current line.
    // The newline itself is left for tokenize() so it can emit a Newline.
    void Lexer::skipComment()
    {
        // A line never contains '\n', so only current and column change.
        const std::size_t start = current;
        std::size_t end = current;

        while (end < source.size() && source[end] != '\n')
        {
            ++end;
        }

        column += end - start;
        current = end;
    }

    // Reads a quoted string.
    //
    // A string begins with " and ends at the next ".
    // Spaces inside are preserved. Escapes and multiline strings are not
    // supported, so a newline or the end of the file ends an unterminated
    // string.

    //
    // Result:
    //   "text"       -> StringLiteral
    //   "text        -> Invalid (unterminated)
    //   "text"abc    -> Invalid (text glued to the closing quote; the whole
    //                   run up to the next whitespace is one Invalid token)
    void Lexer::scanString()
    {
        const std::size_t start = current;
        const std::size_t size = source.size();

        // Skip the opening quote, then look for the closing one.
        std::size_t end = start + 1;

        while (end < size && source[end] != '"' && source[end] != '\n')
        {
            ++end;
        }

        TokenType type = TokenType::Invalid;

        if (end < size && source[end] == '"')
        {
            ++end; // Consume the closing quote.
            type = TokenType::StringLiteral;

            // A string must be followed by whitespace or the end of the
            // file. Anything else makes the whole run invalid.
            if (end < size && !isSpace(source[end]))
            {
                type = TokenType::Invalid;

                while (end < size && !isSpace(source[end]))
                {
                    ++end;
                }
            }
        }

        // The scanned range never contains a newline (the loops above stop
        // at '\n'), so the position can be updated in one step.
        column += end - start;
        current = end;

        addToken(type, source.substr(start, end - start));
    }

    // Reads one whitespace-delimited word and determines its type.
    //
    // Precondition: the current character is not whitespace (tokenize()
    // guarantees this), so the word is never empty.
    //
    // Handles Boolean literals, operators, keywords, numbers and
    // identifiers. Strings are handled by scanString() because they may
    // contain spaces.
    void Lexer::scanWhitespaceDelimitedToken()
    {
        const std::size_t start = current;
        std::size_t end = start;

        while (end < source.size() && !isSpace(source[end]))
        {
            ++end;
        }

        // The word contains no whitespace, so no newline either.
        column += end - start;
        current = end;

        const std::string lexeme = source.substr(start, end - start);

        // 1. Exact spellings (Booleans, operators, keywords).
        const auto &table = fixedSpellings();
        const auto found = table.find(lexeme);

        if (found != table.end())
        {
            addToken(found->second, lexeme);
            return;
        }

        // 2. Number, identifier or invalid.
        addToken(classifyOperand(lexeme), lexeme);
    }

    // Starts a new token: records where it begins, then dispatches on the
    // first character.
    void Lexer::scanToken()
    {
        tokenStartLine = line;
        tokenStartColumn = column;

        if (peek() == '"')
        {
            scanString();
        }

        else
        {
            scanWhitespaceDelimitedToken();
        }
    }

    // Converts the leading whitespace of a line into Indent / Dedent tokens.
    //
    // Called at the start of every line. Blank lines and comment-only lines
    // are ignored so they cannot affect block structure. Tabs advance to
    // the next multiple of four columns.
    void Lexer::handleIndentation()
    {
        if (!atLineStart)
        {
            return;
        }

        // Measure the leading spaces and tabs. No newline can occur here,
        // so position is updated once at the end.
        const std::size_t indentationStart = current;
        std::size_t indentationWidth = 0;
        std::size_t end = current;

        while (end < source.size() &&
               (source[end] == ' ' || source[end] == '\t'))
        {
            indentationWidth = (source[end] == '\t')
                                   ? ((indentationWidth / 4) + 1) * 4
                                   : indentationWidth + 1;
            ++end;
        }

        column += end - indentationStart;
        current = end;

        // Blank and comment-only lines leave atLineStart set, so the next
        // line is checked again.
        if (isAtEnd() ||
            peek() == '\n' ||
            peek() == '\r' ||
            isCommentStart())
        {
            return;
        }

        atLineStart = false;

        // Indent and Dedent tokens are reported at column 1 of their line.
        tokenStartLine = line;
        tokenStartColumn = 1;

        const std::string indentText =
            source.substr(indentationStart, current - indentationStart);

        // Deeper than before: open one block.
        if (indentationWidth > indentationStack.back())
        {
            indentationStack.push_back(indentationWidth);
            addToken(TokenType::Indent, indentText);
            return;
        }

        // Shallower than before: close blocks until the width matches.

        while (indentationStack.size() > 1 &&
               indentationWidth < indentationStack.back())
        {
            indentationStack.pop_back();
            addToken(TokenType::Dedent, indentText);
        }

        // If the width does not match any open level (for example 4, 8,
        // then 6), report it and carry on with the new width so a single
        // mistake does not produce an error on every following line.
        if (indentationWidth != indentationStack.back())
        {
            addToken(TokenType::Invalid, "<invalid indentation>");
            indentationStack.back() = indentationWidth;
        }
    }

    // Tokenizes the complete source code.
    //
    // Can be called repeatedly on the same instance. Each call starts from
    // the beginning and returns a fresh token sequence.
    //
    // Token stream shape (similar to Python):
    //   statement NEWLINE
    //   INDENT statement NEWLINE
    //   DEDENT statement NEWLINE
    //   ... EndOfFile
    std::vector<Token> Lexer::tokenize()
    {
        // Reset all state.
        current = 0;
        line = 1;

        column = 1;
        tokenStartLine = 1;
        tokenStartColumn = 1;
        atLineStart = true;
        indentationStack.assign(1, 0);
        tokens.clear();

        // Rough guess (about one token per 4 characters) to avoid repeated
        // reallocation while the vector grows.
        tokens.reserve(source.size() / 4 + 8);

        while (!isAtEnd())
        {
            // Indentation is only meaningful at the start of a line.
            if (atLineStart)
            {
                handleIndentation();

                if (isAtEnd())
                {
                    break;
                }
            }

            const char c = peek();

            // End of a line. Emit a Newline unless the previous token is
            // already one, or no token exists yet. This collapses blank
            // and comment-only lines instead of producing empty statements.
            if (c == '\n')
            {
                if (!tokens.empty() &&

                    tokens.back().type != TokenType::Newline)
                {
                    tokenStartLine = line;
                    tokenStartColumn = column;
                    addToken(TokenType::Newline, "\n");
                }

                advance(); // Also sets atLineStart for the next line.
                continue;
            }

            // Whitespace between tokens (the '\n' case was handled above).
            if (isSpace(c))
            {
                ++current;
                ++column;
                continue;
            }

            // Comments are discarded, but their newline stays significant.
            if (isCommentStart())
            {
                skipComment();
                continue;
            }

            scanToken();
        }

        // Terminate the last statement if the file has no trailing newline.
        if (!tokens.empty() &&
            tokens.back().type != TokenType::Newline)

        {
            tokenStartLine = line;
            tokenStartColumn = column;
            addToken(TokenType::Newline, "\n");
        }

        // Close all blocks that are still open at the end of the file.
        tokenStartLine = line;
        tokenStartColumn = 1;

        while (indentationStack.size() > 1)
        {
            indentationStack.pop_back();
            addToken(TokenType::Dedent, "");
        }

        // Mark the end of the source.
        tokenStartLine = line;
        tokenStartColumn = column;
        addToken(TokenType::EndOfFile, "");

        // The vector is cleared on the next tokenize() call, so moving it
        // out is safe and avoids copying every token.
        return std::move(tokens);
    }

} // namespace dotdash
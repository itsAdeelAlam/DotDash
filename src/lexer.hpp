#pragma once

#include "token.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace dotdash {

// Converts DotDash source code into a sequence of tokens.
//
// The lexer recognizes literals, identifiers, operators, keywords,
// comments, newlines, and indentation. It does not evaluate expressions
// or determine whether the program is semantically valid.
class Lexer {
public:
    // Creates a lexer for the supplied source code.
    //
    // The source is copied into the Lexer, so the caller does not need
    // to keep the original string alive after construction.
    explicit Lexer(const std::string& source);

    // Tokenizes the complete source and returns the resulting tokens.
    //
    // The returned sequence includes an EndOfFile token.
    std::vector<Token> tokenize();

private:
    // Returns true when no source characters remain to be read.
    bool isAtEnd() const;

    // Returns the current character without consuming it.
    // Returns '\0' when the source has ended.
    char peek() const;

    // Returns the character after the current character without
    // consuming either character. Returns '\0' if unavailable.
    char peekNext() const;

    // Returns the current character and advances the source position.
    // Returns '\0' when the source has ended.
    char advance();

    // Consumes the current character only when it matches expected.
    // Returns true if a character was consumed; otherwise false.
    bool match(char expected);

    // Adds a token using the supplied type and source text.
    // The token's location is its starting line and column.
    void addToken(TokenType type, const std::string& lexeme);

    // Reads and classifies the next source item.
    void scanToken();

    // Reads a quoted string through its closing double quotation mark.
    // Spaces inside a string are part of the string, not separators.
    // Escape sequences are not supported in this version.
    void scanString();

    // Reads one whitespace-delimited, non-string source token and
    // classifies it as a literal, identifier, keyword, or operator.
    void scanWhitespaceDelimitedToken();

    // Checks whether the current source position begins the comment
    // marker "--". Comments continue to the end of their line.
    bool isCommentStart() const;

    // Skips a comment without consuming the following newline.
    void skipComment();

    // Processes indentation at the beginning of a source line.
    // Emits Indent or Dedent tokens when indentation levels change.
    void handleIndentation();

    // Tracks the position in the source string.
    std::string source;
    std::size_t current = 0; // Zero-based index of the next character.
    std::size_t line = 1;    // One-based current line number.
    std::size_t column = 1;  // One-based current column number.

    // Stores the starting position of the token currently being scanned.
    std::size_t tokenStartLine = 1;
    std::size_t tokenStartColumn = 1;

    // Tracks indentation widths for nested blocks.
    // The initial zero represents the indentation level of the first block.
    std::vector<std::size_t> indentationStack{0};

    // Indicates whether the lexer is currently at the beginning of a line.
    bool atLineStart = true;

    // Tokens produced during the current tokenization operation.
    std::vector<Token> tokens;
};

} // namespace dotdash
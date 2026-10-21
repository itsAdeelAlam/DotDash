#pragma once

#include <cstddef>
#include <string>

namespace dotdash {

// Represents every token category recognized by the lexer.
enum class TokenType {
    EndOfFile,       // End of the source code.
    Invalid,         // Unrecognized or invalid token.

    IntegerLiteral,  // Integer numeric literal.
    DoubleLiteral,   // Floating-point numeric literal.
    StringLiteral,   // String literal.
    BooleanTrue,     // Boolean true value: .
    BooleanFalse,    // Boolean false value: -
    Identifier,      // Variable name or other identifier.

    Plus,            // Addition operator: .-
    Minus,           // Subtraction operator: -.
    Multiply,        // Multiplication operator: ..-
    Divide,          // Division operator: --.
    Modulo,          // Modulo operator: .--
    Power,           // Exponentiation operator: -..

    Equal,           // Equality operator: ...
    NotEqual,        // Inequality operator: ---
    LessThan,        // Less-than operator: ..--
    GreaterThan,     // Greater-than operator: --..
    Assignment,      // Assignment operator: ..

    Let,             // Mutable variable declaration: -...-
    Const,           // Constant declaration: .---.
    If,              // Conditional statement: .-..
    Elif,            // Additional conditional branch: .--.
    Else,            // Fallback conditional branch: .---
    While,           // Loop statement: -.--
    Break,           // Exit the current loop: -..-
    Continue,        // Skip to the next loop iteration: -...
    Input,           // Input operation: ...---
    Output,          // Output operation: ---...

    Newline,         // Line break in the source code.
    Indent,          // Increase in indentation level.
    Dedent           // Decrease in indentation level.
};

// Stores a token and its source location.
struct Token {
    TokenType type;       // Category of the token.
    std::string lexeme;   // Original text of the token.
    std::size_t line;     // One-based source line number.
    std::size_t column;   // One-based source column number.
};

} // namespace dotdash
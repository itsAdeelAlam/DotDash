#pragma once

#include <cstddef>
#include <string>

namespace dotdash {

// Identifies the category of a token recognized by the DotDash lexer.
enum class TokenType {
    // Special tokens
    EndOfFile,       // Marks the end of the source code.
    Invalid,         // Represents a token that the lexer cannot recognize.

    // Literal values
    IntegerLiteral,  // An integer literal, such as 42 or -12.
    DoubleLiteral,   // A decimal literal, such as 3.14 or 0.5.
    StringLiteral,   // A quoted string, such as "Hello world".
    BooleanTrue,     // The standalone DotDash value ".".
    BooleanFalse,    // The standalone DotDash value "-".

    // Identifiers
    Identifier,      // A name that is not a recognized keyword.

    // Arithmetic operators
    Plus,            // .-    Addition
    Minus,           // -.    Subtraction
    Multiply,        // ..-   Multiplication
    Divide,          // --.   Division
    Modulo,          // .--   Modulo (remainder)
    Power,           // -..   Exponentiation

    // Comparison operators
    Equal,           // ...   Equal to
    NotEqual,        // ---   Not equal to
    LessThan,        // --..  Less than
    GreaterThan,     // ..--  Greater than
    LessThanOrEqual, // -.-.  Less than or equal to
    GreaterThanOrEqual, // .-.- Greater than or equal to

    // Logical operators
    LogicalAnd,      // ....  Logical AND
    LogicalOr,       // ----  Logical OR
    LogicalNot,      // -.-   Logical NOT

    // Assignment operator
    Assignment,      // ..    Assigns a value to a variable.

    // Declaration and control-flow keywords
    Let,             // -...-   Declares a mutable variable.
    Const,           // .---.   Declares a constant.
    If,              // .-..    Starts a conditional branch.
    Elif,            // .--.    Tests another conditional branch.
    Else,            // .---    Starts the alternative branch.
    While,           // -.--    Starts a while loop.
    Break,           // -...    Exits the current loop.
    Continue,        // -..-    Skips to the next loop iteration.

    // Input/output keywords
    Input,           // ...---  Requests or reads input.
    Output,          // ---...  Produces output.

    // Indentation and line structure
    Newline,         // Marks a line break in the source code.
    Indent,          // Marks an increase in block indentation.
    Dedent           // Marks a decrease in block indentation.
};

// Stores one token recognized by the lexer.
//
// lexeme contains the exact source text associated with the token.
// line and column identify the token's starting position in the source.
// Line and column numbers are one-based.
struct Token {
    TokenType type = TokenType::Invalid;
    std::string lexeme;
    std::size_t line = 1;
    std::size_t column = 1;
};

} // namespace dotdash
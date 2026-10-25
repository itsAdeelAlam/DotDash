# DotDash Lexer

**Document:** `lexer.md`  
**Component:** DotDash Lexer  
**Implementation language:** C++  
**Source file:** `lexer.cpp`  
**Status:** Implementation documentation  
**Scope:** Explanation of the current lexer implementation

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [What Is a Lexer?](#2-what-is-a-lexer)
3. [The Lexer Pipeline](#3-the-lexer-pipeline)
4. [Required Headers](#4-required-headers)
5. [Namespace](#5-namespace)
6. [Lexer Construction](#6-lexer-construction)
7. [Source Position Tracking](#7-source-position-tracking)
8. [Character Reading Functions](#8-character-reading-functions)
9. [Token Creation](#9-token-creation)
10. [Comment Recognition](#10-comment-recognition)
11. [String Literal Recognition](#11-string-literal-recognition)
12. [Whitespace-Delimited Token Scanning](#12-whitespace-delimited-token-scanning)
13. [Boolean Literals](#13-boolean-literals)
14. [Operators and Keywords](#14-operators-and-keywords)
15. [Number Recognition](#15-number-recognition)
16. [Identifier Recognition](#16-identifier-recognition)
17. [Invalid Tokens](#17-invalid-tokens)
18. [Indentation Handling](#18-indentation-handling)
19. [The Main Tokenization Function](#19-the-main-tokenization-function)
20. [Final Newline and End-of-File Handling](#20-final-newline-and-end-of-file-handling)
21. [Complete Examples](#21-complete-examples)
22. [Error Handling and Recovery](#22-error-handling-and-recovery)
23. [Current Limitations](#23-current-limitations)
24. [Potential Improvements](#24-potential-improvements)
25. [Conclusion](#25-conclusion)

---

## 1. Introduction

The DotDash lexer is the component responsible for reading DotDash source code and converting it into a sequence of tokens.

DotDash uses combinations of dots and dashes to represent language constructs. For example, the current lexer recognizes `.-` as addition, `-.` as subtraction, and `..` as assignment.

The lexer also recognizes identifiers, numeric literals, strings, Boolean literals, comments, indentation, and line boundaries.

Instead of asking what an entire source-code expression means, the lexer asks a more basic question:

**What individual language elements appear in this source code?**

For example, consider:

```text
-...- count .. 10
```

The lexer recognizes the source as a sequence of tokens corresponding to:

1. The `let` keyword.
2. The identifier `count`.
3. The assignment operator.
4. The integer literal `10`.

The lexer does not decide whether the variable declaration is semantically valid. That responsibility belongs to later stages of the language implementation.

This document explains the current implementation in `lexer.cpp`, including how it reads characters, recognizes tokens, tracks source positions, handles indentation, and reports invalid input.

## 2. What Is a Lexer?

A lexer, also called a lexical analyzer or tokenizer, is the first major processing stage in a conventional programming-language implementation.

Its job is to transform raw source text into structured tokens.

Consider this source:

```text
-...- count .. 10
```

The raw source is a string of characters. The lexer identifies meaningful groups within that string.

A conceptual token sequence looks like this:

| Token category | Lexeme |
|---|---|
| `Let` | `-...-` |
| `Identifier` | `count` |
| `Assignment` | `..` |
| `IntegerLiteral` | `10` |

A **token type** describes the category of a token. A **lexeme** is the actual source text that produced it.

For example:

- `TokenType::Plus` is a token type.
- `".-"` is the lexeme for that token.

Keeping these concepts separate makes the language implementation easier to maintain. The parser can reason about token categories without repeatedly comparing raw strings.

### What the lexer does not do

The current lexer does not:

- Evaluate arithmetic expressions.
- Execute assignments.
- Determine whether a variable has been declared.
- Check whether an operation has compatible operand types.
- Execute conditional statements or loops.
- Build a complete syntax tree.

Those tasks belong to other components, such as the parser, semantic analyzer, and interpreter.

## 3. The Lexer Pipeline

The lexer is part of a larger processing pipeline:

```text
DotDash source code
        |
        v
      Lexer
        |
        v
   Token sequence
        |
        v
      Parser
        |
        v
 Abstract syntax tree
        |
        v
 Semantic analysis
        |
        v
 Interpreter or compiler
```

The current `lexer.cpp` implements the lexer stage.

For example, the lexer can recognize an addition operator without knowing whether the surrounding expression is valid.

Given:

```text
10 .- 20
```

the lexer identifies the integer literals and addition operator. The parser later determines how those tokens form an expression.

This separation is important because each component has one primary responsibility.

## 4. Required Headers

The implementation begins with:

```cpp
#include "lexer.hpp"

#include <cctype>
#include <string>
```

Each header serves a different purpose.

### `lexer.hpp`

This is the project's own header file. It should declare the `Lexer` class, its member functions, its internal state, and the related types used by the implementation.

The source file includes it so that the definitions in `lexer.cpp` match the declarations in the header.

The provided implementation also uses `std::vector` and `std::size_t`. Their declarations should be provided by the relevant headers, typically through `lexer.hpp` or another included header. For a self-contained implementation, include the standard headers that directly declare the facilities being used.

### `<cctype>`

This standard C++ header provides character-classification functions, including:

- `std::isspace()`
- `std::isdigit()`
- `std::isalpha()`
- `std::isalnum()`

The lexer uses these functions to recognize whitespace, digits, letters, and valid identifier characters.

### `<string>`

This header provides `std::string`, which stores the source code and the lexemes extracted from it.

The lexer uses strings to preserve source text and construct tokens.

## 5. Namespace

The implementation is enclosed in:

```cpp
namespace dotdash
{
    // Lexer implementation
}
```

A namespace groups related declarations under a common name.

The `dotdash` namespace helps distinguish this project's types and functions from names belonging to other libraries.

For example, the lexer class is identified as:

```cpp
dotdash::Lexer
```

This reduces the likelihood of naming conflicts as the project grows.

The closing comment:

```cpp
} // namespace dotdash
```

simply identifies which namespace is being closed. It has no effect on program execution.

## 6. Lexer Construction

The constructor is:

```cpp
Lexer::Lexer(const std::string &source)
    : source(source)
{
}
```

A constructor initializes an object when it is created.

Let's examine its parts.

### The parameter

```cpp
const std::string &source
```

The parameter receives a reference to a string containing the DotDash source code.

- `std::string` is the string type.
- `&` means the parameter is a reference rather than a separate string parameter copy.
- `const` prevents the constructor from modifying the caller's string through this reference.

Passing the source by reference avoids creating an unnecessary copy just to pass the argument into the constructor.

### The member initializer list

```cpp
: source(source)
```

This initializes the object's `source` member from the constructor parameter.

The parameter and member have the same name, but they are different objects.

Conceptually:

```text
Constructor parameter: source
             |
             v
       copied into
             |
             v
Lexer member: source
```

The lexer therefore owns its own copy of the source text. Its stored source remains valid even after the original string supplied by the caller goes out of scope.

This is a useful ownership decision because the lexer retains the source while scanning it.

The constructor body is empty because the member initializer list performs the necessary initialization.

## 7. Source Position Tracking

A lexer must track its position within the source so that it knows what character to read next and where each token begins.

The implementation uses several position-related members, including:

- `current`: the index of the next character to read.
- `line`: the current source line.
- `column`: the current source column.
- `tokenStartLine`: the line where the current token begins.
- `tokenStartColumn`: the column where the current token begins.
- `atLineStart`: whether indentation handling should run.

The exact member declarations belong in `lexer.hpp`.

### Character index

The `current` index identifies the next character to be read from the source string.

For example, in:

```text
cat
```

the string has three characters:

| Character | Index |
|---|---:|
| `c` | 0 |
| `a` | 1 |
| `t` | 2 |

After reading all three characters, `current` becomes `3`, which is equal to the string length.

### Line and column

The `line` member tracks the current line number. The `column` member tracks the current column.

The implementation initializes them to `1`, so source positions use one-based line and column numbering.

For example:

```text
first
second
```

The first character of `first` is at line 1, column 1. The first character of `second` is at line 2, column 1.

### Token start positions

Before scanning a token, the lexer records its starting position.

These values are later passed to `addToken()` so that the token remembers where it originated.

This information is useful for error messages such as:

```text
Invalid token at line 4, column 8.
```

The current implementation records positions but does not itself display formatted diagnostic messages.

## 8. Character Reading Functions

The lexer uses small helper functions to manage source access.

These helpers centralize character reading so that the rest of the lexer does not need to duplicate bounds checks.

### 8.1 `isAtEnd()`

```cpp
bool Lexer::isAtEnd() const
{
    return current >= source.size();
}
```

This function determines whether the lexer has reached the end of the source.

The expression:

```cpp
current >= source.size()
```

compares the current index with the source length.

If `current` is equal to or greater than the source length, no characters remain to be read.

The function is marked `const` because it does not modify the lexer object.

### 8.2 `peek()`

```cpp
char Lexer::peek() const
{
    if (isAtEnd())
    {
        return '\0';
    }

    return source[current];
}
```

This function examines the current character without advancing the source position.

If the lexer has reached the end, it returns the null character, `'\0'`, as a sentinel value.

Otherwise, it returns the character at index `current`.

For example, if the source is:

```text
hello
```

and `current` points to `h`, then `peek()` returns `'h'` without changing `current`.

This is useful when the lexer needs to inspect a character before deciding what to do with it.

### 8.3 `peekNext()`

```cpp
char Lexer::peekNext() const
{
    if (current + 1 >= source.size())
    {
        return '\0';
    }

    return source[current + 1];
}
```

This function examines the character immediately after the current character without consuming either character.

It returns `'\0'` if there is no next character.

For example, if the source is:

```text
--
```

and `current` points to the first dash, `peekNext()` returns the second dash.

The current implementation does not call `peekNext()` elsewhere in the supplied file. It is therefore currently an unused helper and could be retained for future scanning logic or removed if it remains unnecessary.

### 8.4 `advance()`

```cpp
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
```

This function consumes the current character and advances the source position.

The statement:

```cpp
char character = source[current++];
```

reads the character at the current index and then increments `current`.

The postfix increment means the old value is used to access the string before the index increases.

After consuming the character, the function updates the source location.

If the character is a newline:

```cpp
if (character == '\n')
```

the lexer increments the line number, resets the column to `1`, and sets `atLineStart` to `true`.

Otherwise, it increments the column.

Finally, the function returns the character that it consumed.

Returning the consumed character allows callers to inspect it without needing to read it again.

### 8.5 `match()`

```cpp
bool Lexer::match(char expected)
{
    if (isAtEnd() || peek() != expected)
    {
        return false;
    }

    advance();
    return true;
}
```

This function consumes the current character only when it matches an expected character.

Its behavior is straightforward:

1. If the lexer has reached the end, return `false`.
2. If the current character does not match `expected`, return `false`.
3. Otherwise, consume the character and return `true`.

For example, `match('"')` consumes a quotation mark when one is present.

If the current character is `a`, the call returns `false` and leaves the source position unchanged.

The string scanner uses this helper to consume a closing quotation mark.

## 9. Token Creation

The helper responsible for creating tokens is:

```cpp
void Lexer::addToken(
    TokenType type,
    const std::string &lexeme)
{
    tokens.push_back({type,
                      lexeme,
                      tokenStartLine,
                      tokenStartColumn});
}
```

The function receives a token type and the original source text associated with that token.

It then appends a new token to the `tokens` vector.

### The token fields

The initializer supplies four values:

1. `type`: the category of token.
2. `lexeme`: the source text represented by the token.
3. `tokenStartLine`: the token's starting line.
4. `tokenStartColumn`: the token's starting column.

The exact structure of `Token` is defined in the header file.

### Why use a vector?

The `tokens` vector stores the tokens in the order in which they are produced.

As the lexer reads source code, the vector grows:

```text
Source characters
       |
       v
   Token 1
   Token 2
   Token 3
      ...
       |
       v
 tokens vector
```

The parser can later consume this ordered sequence.

### Important detail

`addToken()` does not calculate the token's position itself. It uses the previously stored `tokenStartLine` and `tokenStartColumn`.

Consequently, every scanning function must set these values correctly before adding its tokens.

## 10. Comment Recognition

DotDash comments currently begin with `--`, subject to a boundary condition.

The recognition function is:

```cpp
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
```

### Step 1: Check for the marker

```cpp
source.compare(current, 2, "--")
```

This compares two characters beginning at the current source position with the string `"--"`.

If they do not match, the function returns `false`.

### Step 2: Check the following character

```cpp
const std::size_t next = current + 2;
```

This calculates the position immediately after the two dashes.

The function recognizes a comment if either:

- The two dashes are at the end of the source, or
- The next character is whitespace.

The whitespace check uses:

```cpp
std::isspace(
    static_cast<unsigned char>(source[next]))
```

The conversion to `unsigned char` is important because the C character-classification functions require either `EOF` or a value representable as `unsigned char`.

Passing a potentially negative plain `char` directly to `std::isspace()` can cause undefined behavior.

### Examples

Under the current implementation:

| Source fragment | Comment recognized? |
|---|---|
| `-- comment` | Yes |
| `--` at end of source | Yes |
| `--` followed by a newline | Yes |
| `--.` | No |
| `x--comment` | No |

The `--.` operator remains distinguishable from a comment because the character after its first two dashes is a dot rather than whitespace.

However, the current boundary rule means that `x-- comment` does not start a comment at the dashes if the scanner has already begun consuming the entire whitespace-delimited token `x--`. Comment recognition is checked at the beginning of a token, not continuously inside ordinary tokens.

This behavior follows the current implementation and should be kept consistent with the language specification.

### Skipping comments

The next function is:

```cpp
void Lexer::skipComment()
{
    while (!isAtEnd() && peek() != '\n')
    {
        advance();
    }
}
```

This function consumes characters until it reaches a newline or the end of the source.

Notice that it does not consume the newline itself.

That is deliberate: the normal scanner processes the newline and creates a `Newline` token.

This preserves line boundaries even when a comment appears at the end of a statement.

For example:

```text
-...- count .. 10 -- declare a variable
```

The comment text is discarded, while the newline remains available for normal processing.

## 11. String Literal Recognition

Strings require special handling because their contents may contain whitespace.

The implementation uses:

```cpp
void Lexer::scanString()
```

A string begins with a double quotation mark and ends at the next double quotation mark. Newline characters cannot occur inside a valid string under the current rules, and escape sequences are not supported.

### 11.1 Remember the starting position

```cpp
const std::size_t startIndex = current;
```

The lexer saves the position of the opening quotation mark.

It needs this position to extract the complete source text of the string later.

### 11.2 Consume the opening quote

```cpp
advance();
```

The opening quotation mark is consumed before the lexer begins scanning the string contents.

The saved `startIndex` still points to the quote, so the final lexeme can include both quotation marks.

### 11.3 Scan the string contents

```cpp
while (!isAtEnd() && peek() != '"' && peek() != '\n')
{
    advance();
}
```

The loop continues until one of three things happens:

- The end of the source is reached.
- A closing quotation mark is found.
- A newline is found.

Spaces inside the string are consumed normally.

For example:

```text
"hello world"
```

The space between `hello` and `world` is part of the string rather than a token separator.

### 11.4 Consume the closing quotation mark

```cpp
if (match('"'))
```

If the current character is a closing quotation mark, `match()` consumes it and returns `true`.

If there is no closing quote, the string is invalid.

### 11.5 Validate the character after the closing quote

The current implementation includes this check:

```cpp
if (!isAtEnd() &&
    !std::isspace(static_cast<unsigned char>(peek())))
{
                
    while (!isAtEnd() && !std::isspace(static_cast<unsigned char>(peek())))
    {
        advance();
    } 

    addToken(TokenType::Invalid,
        source.substr(startIndex, current - startIndex));
    return;
}
```

This enforces a boundary after the closing quotation mark.

If the source continues and the next character is not whitespace, the lexer emits an `Invalid` token for the quoted portion.

For example:

```text
"abc"def
```

After consuming `"abc"`, the next character is `d`. It is not whitespace, so the lexer adds:

```text
Invalid("\"abc\"def")
```

The function then returns with out consuming the newline

On the next iteration of the main scanning loop, newline can be recognized.

### 11.6 Add a valid string token

If the closing quote is followed by whitespace or the end of the source, the lexer adds:

```cpp
addToken(
    TokenType::StringLiteral,
    source.substr(startIndex, current - startIndex));
```

The `substr()` call extracts the original string, including its quotation marks.

For example:

```text
"hello world"
```

produces a string token whose lexeme is exactly:

```text
"hello world"
```

The quotation marks are retained in the lexeme. Any later stage that needs the actual string value must interpret or remove them.

### 11.7 Handle an unterminated string

If the closing quote cannot be consumed, the function executes:

```cpp
addToken(
    TokenType::Invalid,
    source.substr(startIndex, current - startIndex));
```

For example:

```text
"hello world
```

contains no closing quotation mark before the newline.

The lexer emits an `Invalid` token for the portion it consumed.

The newline remains unconsumed and can be processed by the main scanner.

### String behavior summary

| Input | Current behavior |
|---|---|
| `"abc"` | Valid `StringLiteral` |
| `"hello world"` | Valid `StringLiteral` |
| `"abc" ` | Valid `StringLiteral` |
| `"abc"def` | Invalid string token, then `def` is scanned separately |
| `"abc"123` | Invalid string token, then `123` is scanned separately |
| `"abc` | Invalid unterminated string token |
| `"a\nb"` with an actual newline inside the quotes | Invalid string before the newline |

The boundary rule is currently whitespace-or-end-of-source, not a general punctuation-delimiter rule. If DotDash later permits punctuation immediately after a string, that policy will need to be revised deliberately.

## 12. Whitespace-Delimited Token Scanning

The function:

```cpp
void Lexer::scanWhitespaceDelimitedToken()
```

handles tokens that do not require special string processing.

It recognizes ordinary tokens such as:

- Identifiers.
- Integer literals.
- Double literals.
- Boolean literals.
- Operators.
- Keywords.
- Invalid sequences.

### 12.1 Save the beginning of the token

```cpp
const std::size_t startIndex = current;
```

This stores the index where the token begins.

### 12.2 Consume characters until whitespace

```cpp
while (!isAtEnd() &&
       !std::isspace(
           static_cast<unsigned char>(peek())))
{
    advance();
}
```

The loop consumes characters until it reaches whitespace or the end of the source.

For example:

```text
count .. 10
```

The whitespace separates the input into three lexemes:

```text
count
..
10
```

The scanner does not split every operator out of an arbitrary character sequence. Instead, it first collects a complete whitespace-delimited lexeme and then classifies that lexeme.

This is a defining characteristic of the current lexer.

### 12.3 Extract the lexeme

```cpp
const std::string lexeme =
    source.substr(startIndex, current - startIndex);
```

`substr()` creates a string containing the characters from `startIndex` through the characters consumed by the scanner.

The length is:

```cpp
current - startIndex
```

For example, if the scanner begins at index `5` and finishes at index `10`, the lexeme length is `5`.

### 12.4 Handle an empty lexeme

```cpp
if (lexeme.empty())
{
    advance();
    return;
}
```

This is a defensive check intended to prevent the scanner from processing an empty lexeme.

Under the normal call path, `scanToken()` skips whitespace and delegates only non-whitespace characters to this function. Therefore, the lexeme should ordinarily be nonempty.

If the guard is reached, it advances one character and returns. This prevents the function from repeatedly processing the same position, although it also means an unexpected character could be silently consumed.

The guard can be reviewed later to decide whether an assertion or a more explicit error path would be clearer.

### 12.5 Classify the lexeme

After extracting the lexeme, the scanner checks it against known Boolean values, operators, keywords, numbers, and identifiers.

The order of these checks matters.

For example, a known operator should be recognized as an operator before the lexer attempts to interpret the same text as a number or identifier.

## 13. Boolean Literals

The lexer recognizes two Boolean literals:

```cpp
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
```

The dot represents `true`, and the dash represents `false`.

Examples:

| Source | Token type |
|---|---|
| `.` | `BooleanTrue` |
| `-` | `BooleanFalse` |

The exact lexeme is retained.

The `return` statements are important because they stop further classification once the token has been recognized.

For example, the dash is also used as a character in operators and negative numeric literals. Recognizing the standalone Boolean value first ensures that the single-character lexeme `-` receives its intended meaning.

## 14. Operators and Keywords

After Boolean recognition, the scanner compares the lexeme against a sequence of exact strings.

Each match produces a token with the corresponding `TokenType`.

### 14.1 Operator mappings

The current implementation recognizes the following operator spellings:

| DotDash spelling | Token type | Meaning |
|---|---|---|
| `..` | `Assignment` | Assignment |
| `.-` | `Plus` | Addition |
| `-.` | `Minus` | Subtraction |
| `..-` | `Multiply` | Multiplication |
| `--.` | `Divide` | Division |
| `.--` | `Modulo` | Modulo |
| `-..` | `Power` | Exponentiation |
| `...` | `Equal` | Equality comparison |
| `---` | `NotEqual` | Inequality comparison |
| `--..` | `LessThan` | Less-than comparison |
| `..--` | `GreaterThan` | Greater-than comparison |
| `-.-.` | `LessThanOrEqual` | Less than or equal to |
| `.-.-` | `GreaterThanOrEqual` | Greater than or equal to |
| `....` | `LogicalAnd` | Logical AND |
| `----` | `LogicalOr` | Logical OR |
| `-.-` | `LogicalNot` | Logical NOT |

These mappings describe the current implementation. Their official language status depends on the project's specification and explicit design decisions.

### 14.2 Keyword mappings

The current implementation recognizes these keywords:

| DotDash spelling | Token type | Intended meaning |
|---|---|---|
| `-...-` | `Let` | Mutable variable declaration |
| `.---.` | `Const` | Constant declaration |
| `.-..` | `If` | Conditional branch |
| `.--.` | `Elif` | Additional conditional branch |
| `.---` | `Else` | Alternative branch |
| `-.--` | `While` | Loop |
| `-...` | `Break` | Exit a loop |
| `-..-` | `Continue` | Skip to the next loop iteration |
| `...---` | `Input` | Input operation |
| `---...` | `Output` | Output operation |

The scanner recognizes the spellings and assigns token types. It does not execute the operations associated with these keywords.

For example, recognizing `.-..` as `If` does not evaluate a condition or determine whether a block should execute. Those responsibilities belong to later stages.

### 14.3 Why exact comparisons are used

Each operator and keyword is recognized through a comparison such as:

```cpp
else if (lexeme == ".-")
{
    addToken(TokenType::Plus, lexeme);
}
```

This is simple and easy to understand.

However, the lexer collects the entire whitespace-delimited lexeme before performing these comparisons. Consequently, the input:

```text
10 .- 20
```

is tokenized as expected, while:

```text
10.-20
```

is collected as a single lexeme rather than three separate tokens.

The current lexer therefore requires separators between ordinary tokens. If DotDash is intended to allow operators directly adjacent to values, its scanning strategy will eventually need to change.

## 15. Number Recognition

If a lexeme is not a Boolean, operator, or keyword, the scanner attempts to classify it as a number.

The current implementation supports:

- Integer literals.
- Decimal literals.
- A leading minus sign for negative numbers.

Examples include:

```text
0
42
-15
3.14
-0.5
```

### 15.1 Initialize the number state

```cpp
bool isNumber = !lexeme.empty();
bool hasDecimalPoint = false;
std::size_t digitStart = 0;
```

These variables track whether the lexeme is still considered a number, whether a decimal point has been encountered, and where digit scanning begins.

Because the empty-lexeme guard appears earlier, `isNumber` is expected to begin as `true` during normal execution.

`hasDecimalPoint` starts as `false` because no decimal point has been examined yet.

`digitStart` starts at `0`, indicating that scanning normally begins at the first character.

### 15.2 Handle a leading minus sign

```cpp
if (lexeme[0] == '-')
{
    digitStart = 1;
}
```

If the first character is a minus sign, the scanner begins examining digits at index `1`.

For example:

```text
-123
```

The minus sign is treated as part of the number rather than as a separate subtraction token.

The scanner does not implement unary arithmetic as a separate syntactic operation here. It simply recognizes the entire lexeme as a signed numeric literal.

### 15.3 Reject a lone minus at this stage

```cpp
if (digitStart == lexeme.size())
{
    isNumber = false;
}
```

If the lexeme contains only a minus sign, there are no characters after the sign to form a number.

The lexer therefore rejects it as a numeric literal.

Under normal classification, a standalone `-` has already been recognized as `BooleanFalse`, so this condition is defensive and generally redundant.

### 15.4 Examine every remaining character

```cpp
for (std::size_t i = digitStart;
     i < lexeme.size();
     ++i)
{
    const char character = lexeme[i];
```

The loop examines each character after the optional leading minus sign.

If the character is a digit:

```cpp
if (std::isdigit(
        static_cast<unsigned char>(character)))
{
    continue;
}
```

the scanner continues to the next character.

The `continue` statement skips the rest of the current loop iteration.

### 15.5 Recognize a decimal point

```cpp
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
```

A decimal point is accepted only when all these conditions hold:

1. The character is `.`.
2. No earlier decimal point has been accepted.
3. At least one character appears before it in the digit portion.
4. At least one character appears after it.
5. The following character is a digit.

This rejects incomplete decimal forms such as:

```text
.5
5.
```

under the current number rules.

It also rejects multiple decimal points, such as:

```text
1.2.3
```

The first decimal point may be accepted, but the second causes numeric classification to fail.

Note that the standalone dot is recognized earlier as `BooleanTrue`, so the number scanner does not process the lexeme `.`.

### 15.6 Reject other characters

```cpp
isNumber = false;
break;
```

If a character is neither a valid digit nor an accepted decimal point, the scanner marks the lexeme as not being a number and stops the loop.

For example:

```text
12abc
```

is not recognized as a numeric literal.

### 15.7 Emit the appropriate numeric token

```cpp
if (isNumber)
{
    addToken(
        hasDecimalPoint
            ? TokenType::DoubleLiteral
            : TokenType::IntegerLiteral,
        lexeme);
    return;
}
```

The conditional expression chooses the token type.

If a decimal point was accepted, the token is `DoubleLiteral`. Otherwise, it is `IntegerLiteral`.

The original lexeme is preserved.

### Numeric limitations

The current implementation performs lexical validation, not full numeric conversion.

It does not check whether an integer fits within the range of the eventual numeric type. It also does not support exponent notation such as `1e6`, explicit plus signs such as `+10`, or leading-decimal forms such as `.5`.

Those are language-design choices that can be addressed separately.

## 16. Identifier Recognition

If the lexeme is not a recognized literal, operator, keyword, or number, the scanner checks whether it is an identifier.

An identifier is a name used for entities such as variables.

The current rules are:

- The first character must be a letter or underscore.
- Subsequent characters may be letters, digits, or underscores.

Examples of valid identifiers include:

```text
count
player1
_total
my_variable
```

Examples of invalid identifiers include:

```text
1count
my-variable
user.name
```

### 16.1 Initialize identifier recognition

```cpp
bool isIdentifier = !lexeme.empty();
```

The lexer initially assumes the lexeme might be an identifier.

As with number recognition, the earlier empty-lexeme guard means the lexeme should already be nonempty during normal execution.

### 16.2 Check the first character

```cpp
if (isIdentifier)
{
    const unsigned char first =
        static_cast<unsigned char>(lexeme[0]);

    isIdentifier =
        std::isalpha(first) || lexeme[0] == '_';
}
```

The first character is converted to `unsigned char` before it is passed to `std::isalpha()`.

The lexer accepts the character if it is an alphabetic character according to the active C locale or if it is an underscore.

A digit cannot begin an identifier under these rules.

### 16.3 Check the remaining characters

```cpp
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
```

Every remaining character must be alphanumeric or an underscore.

The loop condition includes `isIdentifier`, so the loop stops once an invalid character is found.

For example:

```text
player1
```

is valid because it begins with a letter and the remaining characters are letters or digits.

By contrast:

```text
player-one
```

is invalid because the hyphen is not permitted inside an identifier.

### 16.4 Emit the token

```cpp
if (isIdentifier)
{
    addToken(TokenType::Identifier, lexeme);
}
else
{
    addToken(TokenType::Invalid, lexeme);
}
```

If every character satisfies the identifier rules, the lexer creates an `Identifier` token.

Otherwise, it creates an `Invalid` token.

This is the final classification step in `scanWhitespaceDelimitedToken()`.

## 17. Invalid Tokens

The lexer uses `TokenType::Invalid` to represent source fragments that it cannot recognize as valid tokens.

Invalid tokens can result from:

- Malformed numeric literals.
- Invalid identifier-like sequences.
- Unterminated strings.
- Strings followed immediately by non-whitespace characters.
- Invalid indentation.

For example:

```text
12abc
```

does not satisfy the numeric rules and does not satisfy the identifier rules, so it becomes an invalid token.

Similarly:

```text
"abc"def
```

causes the string scanner to emit an invalid token for `"abc"` because the closing quote is immediately followed by `d`.

### Invalid tokens versus diagnostic messages

An invalid token records that the lexer encountered an invalid source fragment. It is not itself a human-readable explanation of the error.

A later component can inspect the token type and its source location to produce an appropriate diagnostic.

The current lexer generally continues scanning after creating invalid tokens. This allows it to discover additional lexical problems in the same source file.

## 18. Indentation Handling

The `handleIndentation()` function processes leading spaces and tabs at the beginning of a line.

Its purpose is to emit `Indent` and `Dedent` tokens that represent changes in indentation.

This supports indentation-based blocks without requiring the lexer to depend on braces.

For example:

```text
.-..
    ---...
```

The second line is indented relative to the first. The lexer can represent this increase with an `Indent` token.

The exact meaning of the block is determined by the parser and language grammar.

### 18.1 Check whether indentation processing is needed

```cpp
if (!atLineStart)
{
    return;
}
```

The function does nothing unless `atLineStart` is true.

This prevents indentation from being processed in the middle of a line.

The `advance()` function sets `atLineStart` to true when it consumes a newline.

### 18.2 Save the indentation start

```cpp
const std::size_t indentationStart = current;
std::size_t indentationWidth = 0;
```

`indentationStart` stores the index where leading whitespace begins.

`indentationWidth` records the calculated width of the indentation prefix.

The initial width is zero.

### 18.3 Measure spaces and tabs

```cpp
while (!isAtEnd() &&
       (peek() == ' ' || peek() == '\t'))
{
    if (peek() == '\t')
    {
        indentationWidth =
            ((indentationWidth / 4) + 1) * 4;
    }
    else
    {
        ++indentationWidth;
    }

    advance();
}
```

The loop consumes leading spaces and tabs.

A space increases the indentation width by one.

A tab advances the indentation width to the next multiple of four.

For example:

| Current width | Character | New width |
|---:|---|---:|
| 0 | Tab | 4 |
| 1 | Tab | 4 |
| 4 | Tab | 8 |
| 5 | Tab | 8 |
| 0 | Space | 1 |

The formula:

```cpp
((indentationWidth / 4) + 1) * 4
```

rounds the current width up to the next tab stop.

This is a measurement policy. It does not mean the tab character is replaced with four spaces in the source.

### 18.4 Ignore blank lines

```cpp
if (isAtEnd() || peek() == '\n' || peek() == '\r')
{
    return;
}
```

If the scanner reaches the end of the source or encounters a blank line after consuming indentation, it returns without changing the indentation stack.

This prevents empty lines from opening or closing indentation blocks.

The newline remains available for the main scanner.

The explicit `'\r'` check also accounts for a carriage return in the line-ending area, although the main newline logic specifically recognizes `'\n'`.

### 18.5 Ignore comment-only lines

```cpp
if (isCommentStart())
{
    return;
}
```

A comment-only line does not change the indentation stack.

The comment will be consumed later by `skipComment()`, while its newline remains available for normal processing.

### 18.6 Mark the beginning of the line as processed

```cpp
atLineStart = false;
```

Once a nonblank, non-comment line has passed the initial checks, indentation processing continues.

Setting `atLineStart` to false prevents the function from repeatedly processing the same indentation prefix during that line.

### 18.7 Compare the current indentation with the stack

```cpp
const std::size_t currentIndentation =
    indentationStack.back();
```

The indentation stack stores the indentation levels currently open.

The final element represents the active indentation level.

For example:

```text
[0]
```

means that no indented block is currently open.

A stack such as:

```text
[0, 4, 8]
```

represents indentation levels of zero, four, and eight columns.

The first element remains the base level.

### 18.8 Handle an indentation increase

```cpp
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
```

If the current line is more indented than the active level, a new block level is opened.

The lexer pushes the new indentation width onto the stack and emits an `Indent` token.

The token's lexeme contains the original whitespace used for that indentation.

Its recorded starting column is set to `1`, representing the beginning of the line rather than the position of the first non-whitespace character.

For example, moving from indentation level zero to four produces a stack transition:

```text
Before: [0]
After:  [0, 4]
```

An `Indent` token is emitted for the increase.

### 18.9 Handle an indentation decrease

```cpp
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
```

When the current line is less indented than the active level, the lexer closes blocks by popping levels from the stack.

Each popped level produces a `Dedent` token.

For example, a transition from indentation level eight to zero may pop two levels:

```text
Before: [0, 4, 8]
After:  [0]
```

Two `Dedent` tokens are emitted.

The stack size check:

```cpp
indentationStack.size() > 1
```

prevents the base indentation level from being removed.

### 18.10 Recover from an invalid indentation level

After popping deeper levels, the lexer checks:

```cpp
if (indentationWidth != indentationStack.back())
```

The indentation must match an existing level.

Consider:

```text
Level 0
    Level 4
        Level 8
      Invalid level 6
```

If the stack contains `[0, 4, 8]` and the new indentation width is six, the lexer pops the level eight entry. The remaining stack is `[0, 4]`.

Since six does not match four, the indentation is invalid.

The current code emits:

```cpp
addToken(
    TokenType::Invalid,
    "<invalid indentation>");
```

It then performs recovery:

```cpp
indentationStack.back() = indentationWidth;
```

This replaces the top stack value with the new indentation width rather than pushing an additional level.

The intention is to let subsequent lines at the same width continue without repeatedly producing the same indentation error, while keeping the stack depth consistent with the number of open indentation levels.

One limitation is that the invalid token's lexeme is a descriptive placeholder rather than the original whitespace. That is different from the lexeme policy used by most other tokens.

### Current indentation limitations

The supplied implementation allows both spaces and tabs in an indentation prefix but does not explicitly reject a prefix that mixes them.

It also does not impose a fixed indentation increment. For example, an indentation width of three may be accepted as a new level even if the previous level was zero.

Whether mixed indentation should be rejected and whether indentation levels should follow a fixed width are language-design decisions that should be documented separately.

## 19. The Main Tokenization Function

The `tokenize()` function coordinates the lexer.

It resets the lexer state, repeatedly scans source characters, finalizes the token stream, and returns the resulting vector.

### 19.1 Reset the state

```cpp
current = 0;
line = 1;
column = 1;
tokenStartLine = 1;
tokenStartColumn = 1;
atLineStart = true;
indentationStack.clear();
indentationStack.push_back(0);
tokens.clear();
```

The function resets all relevant scanning state.

- `current = 0` restarts at the beginning of the source.
- `line = 1` and `column = 1` reset the source location.
- The token-start coordinates are reset.
- `atLineStart = true` enables initial indentation handling.
- `indentationStack.clear()` removes any old indentation levels.
- `indentationStack.push_back(0)` establishes the base indentation level.
- `tokens.clear()` removes tokens from a previous invocation.

This design allows the same `Lexer` object to tokenize its stored source more than once.

Each call starts from a fresh state rather than continuing from the previous scan.

### 19.2 Main scanning loop

```cpp
while (!isAtEnd())
```

The main loop continues until all source characters have been consumed.

Each iteration checks indentation, comments, and ordinary tokens.

### 19.3 Process indentation at the start of a line

```cpp
if (atLineStart)
{
    handleIndentation();
}
```

If the lexer is at the beginning of a line, it processes the indentation prefix before scanning the line's contents.

This is necessary because an `Indent` or `Dedent` token must appear before the tokens belonging to that line.

The function then checks whether indentation processing consumed the remainder of the source:

```cpp
if (isAtEnd())
{
    break;
}
```

This prevents the main loop from trying to scan a character when no characters remain.

### 19.4 Skip comments

```cpp
if (isCommentStart())
{
    skipComment();
    continue;
}
```

If a comment begins at the current position, the lexer skips it.

The `continue` statement immediately begins the next iteration of the loop.

Since `skipComment()` leaves the newline untouched, the next iteration can process that newline through the ordinary scanner.

### 19.5 Scan the next token

```cpp
scanToken();
```

If the input is neither handled by indentation processing nor recognized as a comment, the lexer delegates to `scanToken()`.

That function determines whether the next source element is whitespace, a newline, a string, or an ordinary whitespace-delimited token.

This separation keeps the main loop relatively simple.

## 20. Final Newline and End-of-File Handling

After the main loop finishes, the lexer finalizes the token sequence.

### 20.1 Add a synthetic final newline

```cpp
if (!tokens.empty() &&
    tokens.back().type != TokenType::Newline &&
    tokens.back().type != TokenType::Indent &&
    tokens.back().type != TokenType::Dedent)
{
    tokenStartLine = line;
    tokenStartColumn = column;
    addToken(TokenType::Newline, "\n");
}
```

Some parsers expect the final statement to be terminated by a `Newline` token.

However, source files do not always end with a newline character.

This code adds a synthetic newline when the token sequence is nonempty and the last token is not already a `Newline`, `Indent`, or `Dedent`.

The lexeme is:

```cpp
"\n"
```

This is one newline character, not the two-character sequence consisting of a backslash followed by `n`.

The synthetic newline does not correspond to a character consumed from the source. It is added to normalize the token stream for later processing.

The implementation's exact condition is worth preserving in documentation: it does not add a final newline when the last token is `Indent` or `Dedent`.

### 20.2 Close remaining indentation blocks

```cpp
while (indentationStack.size() > 1)
{
    indentationStack.pop_back();

    tokenStartLine = line;
    tokenStartColumn = 1;
    addToken(TokenType::Dedent, "");
}
```

If the source ends while one or more indentation levels remain open, the lexer emits `Dedent` tokens to close them.

For example, if the indentation stack is:

```text
[0, 4, 8]
```

two `Dedent` tokens are emitted before the lexer finishes.

These final `Dedent` tokens have empty lexemes because they do not correspond to actual whitespace characters consumed at the end of the file.

The base indentation level is preserved.

### 20.3 Emit the end-of-file token

```cpp
tokenStartLine = line;
tokenStartColumn = column;
addToken(TokenType::EndOfFile, "");
```

The lexer appends an `EndOfFile` token to mark the end of the token stream.

The parser can use this token to recognize that no more input remains.

The token has an empty lexeme because it represents a boundary rather than source text.

### 20.4 Return the token sequence

```cpp
return tokens;
```

The function returns the accumulated vector of tokens.

Returning a local vector by value is idiomatic modern C++. C++ can use return-value optimization or implicit move behavior to avoid an unnecessary copy.

An explicit `std::move(tokens)` is not required here and should not be added merely because the return type is a vector.

## 21. Complete Examples

The following examples illustrate the current implementation. They are lexer examples, not complete programs guaranteed to pass the DotDash parser.

### Example 1: Variable declaration

Input:

```text
-...- count .. 10
```

Expected token categories:

| Order | Type | Lexeme |
|---:|---|---|
| 1 | `Let` | `-...-` |
| 2 | `Identifier` | `count` |
| 3 | `Assignment` | `..` |
| 4 | `IntegerLiteral` | `10` |
| 5 | `Newline` | `\n` |
| 6 | `EndOfFile` | Empty |

The newline is synthesized if the source does not end with one.

### Example 2: Arithmetic expression

Input:

```text
10 .- 20
```

Expected token categories:

| Order | Type | Lexeme |
|---:|---|---|
| 1 | `IntegerLiteral` | `10` |
| 2 | `Plus` | `.-` |
| 3 | `IntegerLiteral` | `20` |
| 4 | `Newline` | `\n` |
| 5 | `EndOfFile` | Empty |

The lexer identifies the tokens but does not calculate the result.

### Example 3: String containing spaces

Input:

```text
"hello world"
```

Expected token categories:

| Order | Type | Lexeme |
|---:|---|---|
| 1 | `StringLiteral` | `"hello world"` |
| 2 | `Newline` | `\n` |
| 3 | `EndOfFile` | Empty |

The space inside the quotes remains part of the string.

### Example 4: Invalid string boundary

Input:

```text
"abc"def
```

The current scanner behaves conceptually as follows:

1. `scanString()` consumes `"abc"`.
2. It sees `d` immediately after the closing quote.
3. It emits `Invalid` with lexeme `"abc"`.
4. It returns without consuming `def`.
5. The next scanning iteration recognizes `def` as an identifier.
6. The lexer finalizes the token sequence.

The significant tokens are therefore:

| Order | Type | Lexeme |
|---:|---|---|
| 1 | `Invalid` | `"abc"` |
| 2 | `Identifier` | `def` |

A final `Newline` and `EndOfFile` follow, assuming the usual finalization condition applies.

The entire input is not accepted as a valid string followed by an ordinary identifier. The string portion is explicitly marked invalid.

### Example 5: Comment

Input:

```text
-...- count .. 10 -- variable declaration
```

The comment is skipped when the scanner reaches the comment marker at a token boundary.

The comment text does not produce ordinary tokens. The newline remains available for normal processing.

### Example 6: Indented block

Input:

```text
.-..
    ---...
```

Assuming the four leading spaces represent a new indentation level, the lexer emits an `Indent` token before the second line's `Output` token.

At the end of the source, it emits the necessary `Dedent` token to close the block, followed by `EndOfFile`.

The parser later determines whether the tokens form a valid conditional statement.

## 22. Error Handling and Recovery

The current lexer uses invalid tokens for several kinds of malformed input.

This approach allows scanning to continue instead of terminating at the first error.

For example, if one line contains an invalid numeric lexeme and another line contains an unterminated string, the lexer can potentially report both through its token sequence.

### Current recovery mechanisms

- Invalid numeric or identifier-like lexemes produce `Invalid` tokens.
- Invalid string literals produce `Invalid` tokens.
- Invalid indentation produces an `Invalid` token and adjusts the indentation stack.
- The scanner continues processing subsequent source characters where possible.

### What is not implemented here

The provided code does not show a dedicated diagnostic system with structured error codes or human-readable error messages.

It also does not show a parser-level recovery strategy.

Those mechanisms can be developed later, but they should remain distinct from the basic responsibility of recognizing tokens.

## 23. Current Limitations

The following limitations are based on the supplied implementation.

### 23.1 Tokens are primarily whitespace-delimited

The lexer collects ordinary tokens until whitespace.

Therefore, source such as:

```text
10.-20
```

is not automatically split into an integer, operator, and another integer.

A future lexer may need character-by-character operator recognition if adjacent punctuation is intended to be legal.

### 23.2 String boundaries are restrictive

The current implementation requires whitespace or end-of-source after a closing quote.

It rejects a string immediately followed by a non-whitespace character, but it emits an invalid token for the quoted portion and leaves the trailing text to be scanned separately.

### 23.3 Escape sequences are unsupported

The string scanner stops at the next quotation mark and does not interpret escaped quotes or other escape sequences.

For example, there is no implemented rule for representing a quotation mark inside a string.

### 23.4 Multiline strings are unsupported

The scanner stops when it reaches a newline before finding a closing quote.

This makes a string that spans multiple lines invalid under the current implementation.

### 23.5 Mixed indentation is not explicitly rejected

The indentation code measures both spaces and tabs but does not track whether the same prefix contains both.

As a result, it does not currently enforce a no-mixed-indentation rule.

### 23.6 Invalid indentation uses a placeholder lexeme

The indentation recovery path emits `"<invalid indentation>"` rather than preserving the original whitespace as its lexeme.

This differs from the usual source-text preservation approach used for other invalid tokens.

### 23.7 Numeric range is not checked

The lexer checks the form of a numeric literal but does not convert it to a numeric type or check whether its value is representable.

### 23.8 Character classification follows C++ locale rules

The implementation uses `std::isalpha()`, `std::isalnum()`, `std::isdigit()`, and `std::isspace()`.

Identifier recognition therefore follows the active C locale's classification rules. The language does not yet define a separate Unicode identifier policy in this file.

### 23.9 Some helper logic is redundant

The `lexeme.empty()` guard should ordinarily be unnecessary if the scanner is called correctly. Similarly, `digitStart == lexeme.size()` is generally redundant for the standalone `-` case because that lexeme has already been classified as Boolean false.

These are maintainability observations rather than proof of a functional defect.

## 24. Potential Improvements

The following are possible future improvements, not claims about features already implemented.

### 24.1 Add a formal lexical grammar

Document the exact boundaries between tokens, especially:

- Whether whitespace is required between values and operators.
- Whether punctuation can appear immediately after strings.
- Where comments can begin.
- Which characters are allowed in identifiers.
- Which numeric formats are valid.

A formal grammar would make the implementation easier to evaluate against the intended language.

### 24.2 Improve string error recovery

The current string boundary check rejects `"abc"def` but leaves `def` to be scanned as another token.

That is a defensible recovery strategy, but the behavior should be tested and documented. A future implementation could instead consume the complete malformed sequence and emit one invalid token if that provides clearer diagnostics.

The chosen behavior should avoid producing misleading secondary errors.

### 24.3 Consider character-by-character operator recognition

If DotDash is intended to support adjacent operators and operands, the lexer will need to recognize operator spellings while scanning rather than relying exclusively on whitespace-delimited lexemes.

Since several operators share prefixes, a systematic longest-match strategy would be useful.

This should be a deliberate design change, not an isolated workaround.

### 24.4 Define indentation rules explicitly

The language specification should determine:

- Whether tabs and spaces may be mixed.
- Whether indentation increments have a fixed width.
- How invalid dedents recover.
- Whether an indented first statement is valid.
- How blank and comment-only lines interact with block boundaries.

The implementation can then be tested against those decisions.

### 24.5 Add automated lexer tests

Each scanning behavior should have tests that verify token types, lexemes, source positions, and end-of-file handling.

Useful test categories include:

- Valid and invalid identifiers.
- Integer and decimal literals.
- Every supported operator and keyword.
- Valid, unterminated, and malformed strings.
- Comments at the beginning and end of lines.
- Blank lines and final newlines.
- Nested indentation and invalid dedents.
- Repeated calls to `tokenize()` on the same lexer instance.
- Empty source and source containing only whitespace.

Tests should verify exact token sequences rather than merely checking that tokenization finishes.

### 24.6 Separate tokenization from diagnostic presentation

The lexer can continue emitting `Invalid` tokens while a separate diagnostic layer produces useful error messages.

For example, the lexer could report the invalid token's source location, while the diagnostic layer explains why that token was rejected.

This keeps the scanner focused and makes error reporting easier to maintain.

## 25. Conclusion

The DotDash lexer transforms source text into an ordered token sequence for the parser.

Its main responsibilities are:

- Reading characters safely.
- Tracking source locations.
- Recognizing comments and strings.
- Classifying operators, keywords, Boolean literals, numbers, and identifiers.
- Emitting indentation and line-boundary tokens.
- Marking invalid source fragments.
- Finalizing the token sequence with closing indentation tokens and an end-of-file marker.

The implementation is divided into small helper functions, each handling a particular part of scanning. This structure makes the lexer understandable and gives the project a foundation for incremental improvements.

The most important next step is to keep the implementation, tests, and language specification aligned. A lexer should not merely recognize the examples that happen to work; it should implement predictable rules for valid input, invalid input, and edge cases.

**Documentation note:** This file describes the supplied `lexer.cpp`. Any behavior not present in that source should be treated as a proposed improvement rather than an implemented feature.
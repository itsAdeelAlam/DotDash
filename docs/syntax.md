# DotDash Syntax

**Status:** Proposed
<br>
**Version:** 0.1.0

This document defines the proposed syntax structure of the DotDash programming language.

DotDash uses a **Python-like syntax structure** to keep programs readable and simple while allowing DotDash to have its own operators, keywords, values, and language rules.

The rules in this document are **proposed** and are not yet part of the official DotDash specification.

---

## 1. Syntax Philosophy

DotDash syntax follows these principles:

* Readability
* Simplicity
* Consistency
* Minimal punctuation
* Clear structure
* Easy parsing and implementation

DotDash uses Python as a structural reference, but DotDash is **not Python**.

Python-like syntax describes the general organization of the language. DotDash-specific operators, keywords, and other constructs are defined independently.

---

## 2. Source Files

DotDash source files use the `.dd` file extension.

Example:

```text
hello.dd
```

A DotDash source file contains one or more statements.

Example:

```dd
x .. 10
y .. 20
z .. x .- y
```

---

## 3. Statements

A statement represents an individual instruction or expression in a DotDash program.

Each statement normally occupies its own line.

Example:

```dd
x .. 10
y .. 20
z .. x .- y
```

### 3.1 Statement Separation

A newline separates statements.

Example:

```dd
x .. 10
y .. 20
```

A semicolon is not required at the end of a statement.

---

## 4. Expressions

An expression is a combination of values, variables, operators, and other valid language elements that produces a value.

Example:

```dd
10 .- 5
```

Expressions may contain variables:

```dd
x .- y
```

Expressions may also contain multiple operations:

```dd
x .- y ..- z
```

Operator precedence and associativity will be defined as the expression system is developed.

See the [Operator Reference](docs/operators.md) for the current DotDash operators.

---

## 5. Operators

DotDash operators are represented using combinations of `.` and `-`.

The complete operator system is maintained separately in the [Operator Reference](docs/operators.md).

The current operator system includes:

* Arithmetic operators
* Comparison operators
* Logical operators
* Assignment
* Boolean values
* Line comments

### 5.1 Operator Patterns

Some related operators use reversed dot-and-dash patterns.

A pattern can be reversed by swapping:

```text
. → -
- → .
```

For example:

```text
.- → -.
```

and:

```text
..- → --.
```

This provides a systematic relationship between related operators.

---

## 6. Parentheses

Parentheses are used to group expressions.

Example:

```dd
(x .- y) ..- z
```

Nested parentheses are allowed:

```dd
((x .- y) ..- z)
```

Parentheses allow an expression to be treated as a single unit.

The exact precedence rules will determine when parentheses are required.

---

## 7. Whitespace

Whitespace is generally insignificant in expressions and statements, except where it is required to separate language elements or define indentation.

For example:

```dd
x .- y
```

and:

```dd
x    .-    y
```

may represent the same expression.

Whitespace must not create ambiguity between DotDash symbols.

The lexer will define the exact rules for token separation.

---

## 8. Indentation

DotDash uses indentation for block structure, following the general structural approach of Python.

Example:

```dd
if condition
    statement
    statement
```

Indentation indicates that statements belong to the block controlled by the preceding construct.

The exact indentation rules will be finalized when conditional statements and other block-based constructs are designed.

For now, indentation is **proposed** as the primary method for representing blocks.

---

## 9. Comments

DotDash uses:

```text
--
```

as the line-comment operator.

Everything after `--` on the same line is treated as a comment.

Example:

```dd
x .. 10 -- This is a comment
```

A comment does not produce a value or execute as a statement.

A line containing only a comment is also valid:

```dd
-- This is a comment
```

The `--` comment syntax is part of the proposed DotDash operator system.

---

## 10. Blank Lines

Blank lines do not represent statements.

They may be used to improve readability.

Example:

```dd
x .. 10
y .. 20


z .. x .- y
```

Blank lines do not affect program execution.

---

## 11. Line Structure

A normal DotDash statement follows the general structure:

```text
statement → expression
```

For example:

```dd
10 .- 5
```

As the language develops, additional statement types will be added.

The grammar may eventually contain structures such as:

```text
program   → statement*
statement → expression
          | declaration
          | assignment
          | conditional
          | loop
          | function
```

These additional rules are placeholders for future development and are not yet finalized.

---

## 12. Program Structure

A DotDash program does not currently require a special entry-point function.

A `.dd` file can contain statements directly.

Example:

```dd
x .. 10
y .. 20
z .. x .- y
```

Execution begins with the first valid statement and proceeds in source order.

This may be refined later if modules, functions, or other program-level structures require a different model.

---

## 13. Source Order

Statements are normally processed in the order in which they appear.

Example:

```dd
x .. 10
y .. 20
z .. x .- y
```

The statements are processed in this order:

1. `x .. 10`
2. `y .. 20`
3. `z .. x .- y`

This provides a predictable execution model.

---

## 14. Values and DotDash Symbols

DotDash symbols have meanings defined by the DotDash specification.

The language does not rely on Morse-code meanings.

For example:

```text
.-
```

is interpreted according to the DotDash specification rather than Morse code.

DotDash therefore develops its own syntax and semantics around the dot-and-dash symbol system.

---

## 15. Syntax Errors

A DotDash implementation should report invalid syntax clearly.

For example:

```dd
x .-
```

is an incomplete expression and should produce a syntax error.

Error reporting should eventually provide useful information such as:

* File name
* Line number
* Position
* Error description

The exact error format will be defined during interpreter development.

---

## 16. Current Proposed Syntax Rules

| Feature              | Proposed Rule                  |
| -------------------- | ------------------------------ |
| File extension       | `.dd`                          |
| General syntax style | Python-like                    |
| Statement separation | Newline                        |
| Semicolon            | Not required                   |
| Expressions          | Supported                      |
| Expression grouping  | Parentheses                    |
| Whitespace           | Generally insignificant        |
| Blocks               | Indentation                    |
| Blank lines          | Allowed                        |
| Line comments        | `--`                           |
| Program entry point  | Not required                   |
| Execution order      | Source order                   |
| Operators            | Defined in `docs/operators.md` |

---

## 17. Example Program

The following demonstrates the current proposed syntax:

```dd
x .. 10
y .. 20

sum .. x .- y
difference .. y -. x

is_equal .. x ... y
is_smaller .. x ..-- y
```

This example demonstrates:

* Statements separated by newlines
* Blank lines
* Assignment
* Arithmetic operators
* Comparison operators
* Variables
* Expressions

Variable declaration rules and the complete expression grammar are still being developed.

---

## 18. Proposed Grammar

The initial grammar is intentionally small.

```text
program     → statement*

statement   → expression

expression  → value
            | variable
            | expression operator expression
            | "(" expression ")"
```

This grammar is only a starting point.

It will be expanded as DotDash gains:

* Variables
* Declarations
* Conditions
* Loops
* Functions
* Collections
* Other language features

The grammar should remain a central reference as the language develops.

---

## 19. Design Status

The syntax described in this document is **Proposed**.

A proposed rule may be changed after testing.

Before becoming official, syntax should be evaluated for:

1. Consistency
2. Readability
3. Ambiguity
4. Parser implementation
5. Interaction with existing DotDash rules
6. Future extensibility

A syntax rule should only become **Official** after it has been reviewed and tested against practical examples.

---

## 20. Future Development

The following syntax areas remain to be defined:

* Number syntax
* String syntax
* Variable declaration syntax
* Assignment rules
* Operator precedence
* Operator associativity
* Unary operators
* Exact indentation rules
* Block syntax
* Conditional syntax
* Loop syntax
* Function syntax
* Collection syntax
* Import/module syntax

These features should be developed incrementally rather than added all at once.

---

**Status:** Proposed
**Version:** 0.1.0

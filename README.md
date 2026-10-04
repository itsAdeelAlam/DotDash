# DotDash

DotDash is an experimental programming language built around **dot (`.`) and dash (`-`) patterns**.

The goal of DotDash is to create a real programming language with its own syntax, grammar, operators, values, and execution model.

DotDash is inspired by the simple visual idea of dots and dashes in Morse Code, but it is **not intended to be a replacement for Morse code**. Its symbols have meanings defined by the DotDash language itself.

The planned file extension for DotDash programs is:

```text
.dd
```

---

## Project Goal

DotDash is being developed from the ground up as a small, structured, and understandable programming language.

The project aims to define its own:

* Syntax
* Keywords
* Operators
* Values
* Variables
* Expressions
* Statements
* Grammar
* Execution rules
* Standard library
* Interpreter or compiler

The language will grow gradually. New features will be added only after the existing syntax and rules are clear enough to support them.

---

## How DotDash Works

DotDash uses combinations of `.` and `-` as language symbols.

For example, operators can be represented using DotDash patterns:

```text
.-   addition
-.   subtraction
..   assignment
```

These patterns are **DotDash language symbols**. Their meanings come from the DotDash specification and are not automatically based on Morse code.

The language is designed so that DotDash patterns can represent different parts of a program while following a consistent syntax.

A DotDash program will eventually be processed roughly like this:

```text
DotDash source
      ↓
Lexer
      ↓
Parser
      ↓
AST
      ↓
Semantic analysis
      ↓
Interpreter / Compiler
      ↓
Execution
```

The implementation may initially be written in C++, but the implementation language is separate from the design of DotDash itself.

---

## Design Principles

DotDash is being developed around a few simple principles.

### Consistency

Similar language features should follow similar rules.

New syntax should not introduce unnecessary special cases.

### Simplicity

The language should remain understandable and practical to implement.

### Extensibility

The initial language will be small, but its structure should allow future features such as:

* Variables
* Numbers
* Strings
* Boolean values
* Arithmetic
* Comparisons
* Conditions
* Loops
* Functions
* Lists and other data structures
* Input and output
* Modules
* Error handling

### Distinct Identity

DotDash uses dots and dashes as its core visual symbols, but it is intended to develop its own language structure rather than simply copying Morse-code conventions.

---

## Development Status

DotDash is currently in the **early language-design stage**.

The project is being developed incrementally.

Current work focuses on establishing the core language before moving into more advanced features.

Some rules are already defined, while others are still being designed.

Language ideas are treated as one of three states:

* **Proposed** - being discussed or designed
* **Experimental** - implemented or tested but not finalized
* **Official** - confirmed as part of the DotDash specification

A proposed idea should not be treated as an official language rule.

---

## Development Direction

The planned development order is approximately:

```text
1. Core symbols
2. Basic syntax
3. Values
4. Operators
5. Expressions
6. Variables
7. Statements
8. Conditions
9. Loops
10. Functions
11. Data structures
12. Standard library
13. Interpreter
14. Tooling
15. Formal specification
```

This order may change as the language develops.

The goal is to stabilize the foundations before adding complex features.

---

## Example

A simple DotDash program might eventually look something like:

```DotDash
-- This is a comment.

x .. 5 .- 6       -- x = 5 + 6
```

The exact program syntax is still being developed.

The examples in the documentation should therefore be treated according to their documented status rather than assuming that every example represents a finalized language rule.

---

## Documentation

The `docs/` directory contains the current language documentation.

As DotDash grows, the documentation will cover areas such as:

* Language syntax
* Keywords
* Operators
* Values
* Grammar
* Examples
* Design decisions
* Testing
* Language changes

The documentation will evolve alongside the language.

---

## Repository

The project currently focuses on the language documentation and core project files.

The repository structure will grow as the project develops.

```text
DotDash/
├── README.md
├── LICENSE.md
├── CHANGELOG.md
└── docs/
```

Additional directories and implementation files will be added when they become necessary.

---

## License

DotDash is released under the **DDL-1.0.0** license.

See [`LICENSE.md`](LICENSE.md) for the full license text.

---

## Project Status

**DotDash is an evolving project.**

The current documentation describes the language as it exists at this stage of development. Rules may change while the language is being designed, and significant changes will be documented rather than silently replacing previous decisions.

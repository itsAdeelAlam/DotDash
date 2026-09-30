# DotDash Operators

**Version:** v0.x
**Status:** Official

This document defines the official operators currently included in DotDash.

The operator system is part of the DotDash language syntax. Each operator has a specific meaning and behavior.

---

## 1. Boolean Values

DotDash uses a single dot and dash to represent boolean values.

| Symbol | Meaning |
| ------ | ------- |
| `.`    | True    |
| `-`    | False   |

Examples:

```text
.
```

represents `true`.

```text
-
```

represents `false`.

---

## 2. Comments

DotDash uses `--` to create a line comment.

| Symbol | Meaning      |
| ------ | ------------ |
| `--`   | Line comment |

The `--` operator must appear at the beginning of a line.

Everything after `--` on that line is treated as a comment and is ignored by the language.

Example:

```text
-- This is a comment
```

A comment does not affect the following lines.

Example:

```text
-- Set the value
x .. 10

-- Change the value
x .. 20
```

### Comment Rules

1. `--` must be at the beginning of the line.
2. Everything after `--` on the same line is ignored.
3. The comment ends at the line break.
4. A comment does not affect other lines.

> `--` at the beginning of a line is interpreted as a comment. Other contexts may give a longer pattern beginning with `--` a different meaning.

---

## 3. Assignment

| Symbol | Meaning    | Operands |
| ------ | ---------- | -------- |
| `..`   | Assignment | 2        |

The assignment operator assigns the value on the right to the target on the left.

Example:

```text
x .. 10
```

This assigns `10` to `x`.

---

## 4. Arithmetic Operators

| Symbol | Meaning        | Operands |
| ------ | -------------- | -------- |
| `.-`   | Addition       | 2        |
| `-.`   | Subtraction    | 2        |
| `..-`  | Multiplication | 2        |
| `--.`  | Division       | 2        |
| `.--`  | Modulo         | 2        |
| `-..`  | Power          | 2        |

Examples:

```text
5 .- 3
```

Addition.

```text
5 -. 3
```

Subtraction.

```text
5 ..- 3
```

Multiplication.

```text
6 --. 2
```

Division.

```text
7 .-- 3
```

Modulo.

```text
2 -.. 3
```

Power.

---

## 5. Comparison Operators

| Symbol | Meaning               | Operands |
| ------ | --------------------- | -------- |
| `...`  | Equal                 | 2        |
| `---`  | Not equal             | 2        |
| `..--` | Less than             | 2        |
| `--..` | Greater than          | 2        |
| `.-.-` | Less than or equal    | 2        |
| `-.-.` | Greater than or equal | 2        |

Examples:

```text
5 ... 5
```

Equal.

```text
5 --- 3
```

Not equal.

```text
3 ..-- 5
```

Less than.

```text
5 --.. 3
```

Greater than.

```text
5 .-.- 5
```

Less than or equal.

```text
5 -.-. 3
```

Greater than or equal.

---

## 6. Logical Operators

| Symbol | Meaning | Operands |
| ------ | ------- | -------- |
| `....` | AND     | 2        |
| `----` | OR      | 2        |
| `.-.`  | NOT     | 1        |

Examples:

```text
. .... .
```

AND.

```text
. ---- -
```

OR.

```text
.-.
```

NOT.

---

## 7. Operator Summary

| Symbol | Meaning               | Operands |
| ------ | --------------------- | -------- |
| `.`    | True                  | 0        |
| `-`    | False                 | 0        |
| `--`   | Line comment          | 0        |
| `..`   | Assignment            | 2        |
| `.-`   | Addition              | 2        |
| `-.`   | Subtraction           | 2        |
| `..-`  | Multiplication        | 2        |
| `--.`  | Division              | 2        |
| `.--`  | Modulo                | 2        |
| `-..`  | Power                 | 2        |
| `...`  | Equal                 | 2        |
| `---`  | Not equal             | 2        |
| `..--` | Less than             | 2        |
| `--..` | Greater than          | 2        |
| `.-.-` | Less than or equal    | 2        |
| `-.-.` | Greater than or equal | 2        |
| `....` | AND                   | 2        |
| `----` | OR                    | 2        |
| `.-.`  | NOT                   | 1        |

---

## Operator Pattern

DotDash uses reversed dot-and-dash patterns for some counterpart or related operators.

To create the related pattern, each `.` is swapped with `-`, and each `-` is swapped with `.`.

For example:

```text
.-  →  -.
```

and:

```text
..-  →  --.
```

This keeps related operators connected by a simple and consistent pattern instead of giving every operator a completely unrelated symbol.

---

## Status

All operators documented in this file are **Official** for the current DotDash v0.x specification.

The operator system may be extended as DotDash develops.

New operators must be evaluated for:

* Consistency
* Ambiguity
* Usability
* Extensibility
* Implementation feasibility

Existing official operators should not be changed without an explicit language-design decision.

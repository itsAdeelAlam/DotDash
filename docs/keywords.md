# DotDash Keywords

**Status:** Proposed
<br>
**Version:** v0.x

This document defines the proposed keyword system for the DotDash programming language.

Keywords are reserved language constructs used to express program structure and common operations. Each keyword is represented by a DotDash symbol.

The symbols in this document are **proposed** and are not yet part of the official DotDash specification.

---

## 1. Keyword Principles

DotDash keywords follow these principles:

1. Keywords should have short and memorable DotDash symbols.
2. Related language features should use visually recognizable symbols where practical.
3. Keyword symbols must not conflict with existing operators.
4. A keyword symbol must represent one keyword only.
5. Keywords should be added only when the language requires them.
6. Proposed keywords may be changed before becoming official.

DotDash does not use Morse-code representations of the English keyword itself. The DotDash symbol is a language-design symbol, not an encoded spelling.

---

## 2. Proposed Keywords

### 2.1 Variables

| Keyword | Symbol  | Purpose                     |
| ------- | ------- | --------------------------- |
| `let`   | `-...-` | Declares a mutable variable |
| `const` | `.---.` | Declares a constant         |

Example:

```text
-...- x .. 10
.---. limit .. 100
```

Normal programming equivalent:

```text
let x = 10
const limit = 100
```

The `..` symbol is the existing assignment operator.

---

### 2.2 Types

The initial DotDash type system contains four primitive types:

| Type     | Purpose               |
| -------- | --------------------- |
| `int`    | Integer values        |
| `double` | Floating-point values |
| `bool`   | Boolean values        |
| `string` | Text values           |

The exact syntax for type names is not yet finalized.

DotDash currently intends to support implicit type conversion in the interpreter to keep the language simple. Compiler behavior may be defined later.

Examples of the four primitive values:

```text
42
3.14
.
"Hello"
```

Normal programming equivalent:

```text
42
3.14
true
"Hello"
```

In DotDash:

* `42` is an `int`.
* `3.14` is a `double`.
* `.` represents `true`.
* `-` represents `false`.
* `"Hello"` is a `string`.

---

### 2.3 Conditions

| Keyword | Symbol | Purpose                                             |
| ------- | ------ | --------------------------------------------------- |
| `if`    | `.-..` | Starts a conditional branch                         |
| `elif`  | `.--.` | Tests another condition if previous conditions fail |
| `else`  | `.---` | Executes when previous conditions fail              |

The three keywords form the conditional family.

Conceptual structure:

```text
.-.. condition
    -- some code
.--. condition
    -- some code
.---
    -- some code
```

Normal programming equivalent:

```text
if condition
    ...
elif condition
    ...
else
    ...
```

Example:

```text
.-.. x ... 10
    ---... "x is 10"
.--. x ... 20
    ---... "x is 20"
.---
    ---... "x is something else"
```

Normal programming equivalent:

```text
if x == 10
    output "x is 10"
elif x == 20
    output "x is 20"
else
    output "x is something else"
```

The exact block syntax is not yet finalized.

---

### 2.4 Loops

| Keyword    | Symbol | Purpose                                         |
| ---------- | ------ | ----------------------------------------------- |
| `while`    | `-.--` | Repeats a block while a condition is true       |
| `continue` | `-..-` | Skips to the next iteration of the current loop |
| `break`    | `-...` | Exits the current loop                          |

`break` and `continue` are valid only within a loop.

Conceptual example:

```text
-.-- x --.. 10
    ---... x
    x .. x .- 1
```

Normal programming equivalent:

```text
while x < 10
    output x
    x = x + 1
```

Example using `break`:

```text
-.-- .
    -...
```

Normal programming equivalent:

```text
while true
    break
```

Example using `continue`:

```text
-.-- .
    -..-
```

Normal programming equivalent:

```text
while true
    continue
```

---

### 2.5 Input and Output

| Keyword  | Symbol   | Purpose                 |
| -------- | -------- | ----------------------- |
| `input`  | `...---` | Receives input          |
| `output` | `---...` | Produces program output |

The `input` and `output` symbols intentionally use a related structure.

Conceptual examples:

```text
...--- name
---... name
```

Normal programming equivalent:

```text
input name
output name
```

A more complete example:

```text
-...- name .. ...--- 
---... name
```

Normal programming equivalent:

```text
let name = input
output name
```

The exact syntax and behavior of these operations are not yet finalized.

---

## 3. Complete Proposed Keyword Table

| Category  | Keyword    | DotDash Symbol |
| --------- | ---------- | -------------- |
| Variable  | `let`      | `-...-`        |
| Variable  | `const`    | `.---.`        |
| Type      | `int`      | TBD            |
| Type      | `double`   | TBD            |
| Type      | `bool`     | TBD            |
| Type      | `string`   | TBD            |
| Condition | `if`       | `.-..`         |
| Condition | `elif`     | `.--.`         |
| Condition | `else`     | `.---`         |
| Loop      | `while`    | `-.--`         |
| Loop      | `continue` | `-..-`         |
| Loop      | `break`    | `-...`         |
| I/O       | `input`    | `...---`       |
| I/O       | `output`   | `---...`       |

There are currently **14 proposed keywords/type names**, with **10 assigned DotDash symbols** and **4 type symbols still to be designed**.

---

## 4. Reserved Future Keywords

The following are intentionally **not** keywords yet:

```text
for
function
return
class
struct
import
export
try
catch
```

They may be introduced when the corresponding language features are designed.

A concept should not receive a keyword before the language has a concrete use for it.

---

## 5. Status

All keyword symbols in this document are currently:

**PROPOSED**

They must not be treated as official DotDash syntax until explicitly promoted to the official specification.

Changes to proposed keywords may be made without requiring backwards-compatibility guarantees.

---

## 6. Design Notes

DotDash keyword symbols are designed independently of standard Morse-code word encoding.

The goal is to create a compact symbolic vocabulary that is:

* Easy to recognize
* Easy to distinguish
* Reasonably memorable
* Consistent with the DotDash identity
* Free from conflicts with existing operators

The keyword system should remain small during the early development of DotDash.

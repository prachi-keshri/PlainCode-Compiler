# PlainCode Compiler

## Project Title

PlainCode: Design and Implementation of an English-Like Programming Language Compiler

## Quick Start — Build and Run

Run the sample PlainCode program in four simple steps.

**Requirements:** Ubuntu/WSL with GCC, G++, and GNU Make installed.

**Step 1: Clone the repository**

```bash
git clone https://github.com/prachi-keshri/PlainCode-Compiler.git
cd PlainCode-Compiler
```

**Step 2: Build the compiler**

```bash
make
```

**Step 3: Generate C code from the sample program**

```bash
./build/plaincode examples/demo.plain output.c
```

**Step 4: Compile and run the generated C code**

```bash
gcc -std=c11 output.c -o program
./program
```

**Expected output:**

```text
30
```

The PlainCode compiler translates the sample `.plain` program into C code. GCC then compiles the generated C code into an executable.

For the complete architecture, language features, compiler phases, and testing details, see the sections below.

---

## Overview

PlainCode is an English-like programming language compiler developed using C++17.

It translates PlainCode source programs into C code through lexical analysis,
parsing, AST construction, semantic analysis, three-address code generation,
optimization, and C code generation.

The generated C program can be compiled and executed using GCC.

## Technologies Used

- C++17
- C
- GCC / G++
- GNU Make
- VS Code
- Ubuntu WSL
- Git and GitHub

## Compiler Architecture

```text
PlainCode Source (.plain)
          |
          v
    Lexical Analyzer
          |
          v
    Syntax Analyzer
          |
          v
 Abstract Syntax Tree
          |
          v
   Semantic Analyzer
          |
          v
     TAC Generator
          |
          v
      TAC Optimizer
          |
          v
    C Code Generator
          |
          v
   Generated C File
          |
          v
         GCC
          |
          v
     Executable
```

## Supported Language Features

- Variable assignment using SET ... TO ...
- Output statements using DISPLAY
- Integer literals and variables
- Addition (+)
- Subtraction (-)
- Multiplication (*)
- Integer division (/)
- Parentheses
- Arithmetic operator precedence
- Variable reassignment
- Undefined-variable detection
- Constant folding and constant propagation
- Runtime division-by-zero protection

Conditional statements and loops are not currently supported.

## Sample PlainCode Program

```text
SET a TO 10
SET b TO 20
SET result TO a + b * 2
DISPLAY result
```

Expected output:

```text
50
```

## Project Structure

```text
PlainCode-Compiler/
├── docs/
├── examples/
├── include/
├── src/
│   ├── lexer.cpp
│   ├── parser.cpp
│   ├── semantic.cpp
│   ├── tac_generator.cpp
│   ├── optimizer.cpp
│   ├── codegen.cpp
│   └── main.cpp
├── tests/
├── Makefile
├── .gitignore
└── README.md
```

## Build Instructions

### Requirements

Install GCC, G++, and GNU Make.

On Ubuntu:

```bash
sudo apt update
sudo apt install build-essential
```

### Build the Compiler

Open a terminal in the project directory and run:

```bash
make
```

This creates:

```text
build/plaincode
```

To clean the build directory:

```bash
make clean
```

## Running a PlainCode Program

The repository includes a ready-to-run sample program at `examples/demo.plain`.

**Step 1: View the sample source code**

Open `examples/demo.plain` in VS Code or run:

```bash
cat examples/demo.plain
```

The program contains:

```text
SET a TO 10
SET b TO 20
DISPLAY a + b
```

**Step 2: Generate C code**

After building the compiler with `make`, run:

```bash
./build/plaincode examples/demo.plain output.c
```

This translates the PlainCode source file into C and saves it as `output.c`.

**Step 3: View the generated C code (optional)**

```bash
cat output.c
```

**Step 4: Compile the generated C program**

```bash
gcc -std=c11 output.c -o program
```

**Step 5: Run the executable**

```bash
./program
```

**Expected output:**

```text
30
```

**Note:** The PlainCode compiler generates C source code. GCC compilation and execution are performed separately.

---

## Compiler Phases

### 1. Lexical Analysis

Converts PlainCode source text into tokens.

### 2. Syntax Analysis

Uses a recursive-descent parser to validate syntax and construct an AST.

### 3. Semantic Analysis

Checks variable usage and detects undefined variables.

### 4. Three-Address Code Generation

Converts AST expressions and statements into intermediate TAC instructions.

### 5. TAC Optimization

Performs optimizations including constant folding, constant propagation,
and removal of unused temporary computations.

### 6. C Code Generation

Converts optimized TAC into C source code.

### 7. GCC Compilation

The generated C file can be compiled separately using GCC.

## Error Handling

The compiler detects invalid variable usage.

Example:

```text
DISPLAY x + 5
```

Expected error:

```text
Compilation failed: Semantic error: undefined variable 'x'
```

Generated C programs also include checks for division by zero.

## Testing

The integrated compiler has passed seven manual integration tests:

1. Basic arithmetic and output
2. Variable reassignment
3. Operator precedence and parentheses
4. Undefined-variable detection
5. Self-referencing first assignment
6. Integer division
7. Division-by-zero runtime protection

Individual module tests are available in the `tests/` directory.

## Team Responsibilities

### Member 1 — Prachi

- Repository setup
- Lexical analyzer
- Token definitions
- Semantic analyzer
- TAC optimizer
- Compiler integration
- Integration testing
- Build setup and documentation

### Member 2 — Yashita

- Grammar implementation
- Recursive-descent parser
- AST construction
- TAC generator
- C code generator
- Module testing

## Project Status

The core PlainCode compiler is implemented and integrated.

The compiler successfully translates supported PlainCode programs into C code.

The generated C code is compiled separately using GCC.

Seven end-to-end integration tests have passed.

## Future Development

- Conditional statements and loops
- Additional data types
- Compile-time detection of division by zero
- Removal of unused assignments
- Automatic GCC compilation and execution
- Automated end-to-end testing

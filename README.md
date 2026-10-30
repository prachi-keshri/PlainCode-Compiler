# PlainCode Compiler

## Project Title

PlainCode: Design and Implementation of an English-Like Programming Language Compiler

## Quick Start — Build and Run

Run the sample PlainCode program in four simple steps.

**Requirements:** GCC, G++, and GNU Make.

Supported environments:
- Ubuntu / WSL
- Windows with MSYS2 UCRT64 GCC and `mingw32-make`

### Step 1: Clone the repository


git clone https://github.com/prachi-keshri/PlainCode-Compiler.git
cd PlainCode-Compiler


### Step 2: Build the compiler

On Ubuntu / WSL:


make


On Windows PowerShell:


mingw32-make


### Step 3: Generate C code from the sample program

On Ubuntu / WSL:


./build/plaincode examples/demo.plain output.c


On Windows PowerShell:

.\build\plaincode.exe examples\demo.plain output.c


### Step 4: Compile and run the generated C code

On Ubuntu / WSL:


gcc -std=c11 output.c -o program
./program


On Windows PowerShell:


gcc -std=c11 output.c -o program.exe
.\program.exe


**Expected output:**


30


The PlainCode compiler translates the sample `.plain` program into C code. GCC then compiles the generated C code into an executable.

For the complete architecture, language features, compiler phases, and testing details, see the sections below.

---

## Overview

PlainCode is an English-like programming language compiler developed using C++17.

It translates PlainCode source programs into C code through lexical analysis, parsing, AST construction, semantic analysis, three-address code generation, optimization, and C code generation.

The generated C program can be compiled and executed using GCC.

## Technologies Used

- C++17
- C
- GCC / G++
- GNU Make
- VS Code
- Ubuntu / WSL
- Windows / MSYS2 UCRT64
- Git and GitHub

## Compiler Architecture


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

## Supported Language Features

- Variable assignment using `SET ... TO ...`
- Output statements using `DISPLAY`
- Integer and floating-point literals and variables
- String literals and string variables (assignment and display)
- User-defined functions with numeric parameters and return values
- Function calls inside expressions, including recursive calls
- Addition (`+`)
- Subtraction (`-`)
- Multiplication (`*`)
- Division (`/`)
- Parentheses
- Arithmetic operator precedence
- Variable reassignment
- Undefined-variable detection with line and column diagnostics
- IF / ELSE / ENDIF conditional statements
- WHILE / ENDWHILE loops, including nested control-flow blocks
- Comparison operators: `<`, `>`, `<=`, `>=`, `==`, `!=`
- Constant folding and constant propagation
- Algebraic simplification and dead temporary elimination
- Runtime division-by-zero protection

**Type and function rules:** Numeric variables support integers and floating-point values. String variables support assignment and DISPLAY; string concatenation is not supported. Functions accept numeric parameters and return numeric values. Function parameters and return values are emitted as C `double`. Function-local variables are scoped to their function.

## Sample PlainCode Program


SET a TO 10
SET b TO 20
SET result TO a + b * 2
DISPLAY result


Expected output:


50


## Functions and Data Types Example


FUNCTION add(a, b)
    RETURN a + b
ENDFUNCTION

SET answer TO add(12, 30)
SET price TO 19.95
SET tax TO 1.5
SET message TO "PlainCode supports strings and decimals"
DISPLAY answer
DISPLAY price + tax
DISPLAY message


Expected output:


42
21.45
PlainCode supports strings and decimals


See `examples/functions_and_types.plain` for the complete example.

## Control-Flow Example


SET count TO 1
WHILE count <= 5 DO
    DISPLAY count
    SET count TO count + 1
ENDWHILE

IF count == 6 THEN
    DISPLAY 100
ELSE
    DISPLAY 0
ENDIF


Expected output:

1
2
3
4
5
100


See `examples/control_flow.plain` and `docs/grammar.md` for the language specification.

## Recursion Example


FUNCTION factorial(n)
    IF n <= 1 THEN
        RETURN 1
    ELSE
        RETURN n * factorial(n - 1)
    ENDIF
ENDFUNCTION

SET result TO factorial(5)
DISPLAY result


Expected output:


120


See `examples/recursion.plain` for the complete example.

## Project Structure


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


## Build Instructions

### Requirements

Install:
- GCC
- G++
- GNU Make

### Ubuntu / WSL

Install the required build tools:


sudo apt update
sudo apt install build-essential


Build the compiler from the project directory:


make


This creates:


build/plaincode


To clean the build:


make clean


### Windows (MSYS2 UCRT64)

Install the MSYS2 UCRT64 GCC toolchain and Make. Ensure that `C:\msys64\ucrt64\bin` is added to your Windows PATH.

Open PowerShell in the project directory and build:


mingw32-make

This creates:


build/plaincode.exe


To clean the build:


mingw32-make clean


## Running a PlainCode Program

The repository includes a sample program at `examples/demo.plain`.

### Step 1: View the sample source code

On Ubuntu / WSL:


cat examples/demo.plain


On Windows PowerShell:


Get-Content examples\demo.plain


The program contains:


SET a TO 10
SET b TO 20
DISPLAY a + b


### Step 2: Generate C code

On Ubuntu / WSL:


./build/plaincode examples/demo.plain output.c


On Windows PowerShell:

.\build\plaincode.exe examples\demo.plain output.c


This translates the PlainCode source file into C and saves it as `output.c`.

### Step 3: View the generated C code (optional)

On Ubuntu / WSL:


cat output.c


On Windows PowerShell:

Get-Content output.c


### Step 4: Compile the generated C program


gcc -std=c11 output.c -o program


On Windows, use:


gcc -std=c11 output.c -o program.exe


### Step 5: Run the executable

On Ubuntu / WSL:


./program


On Windows PowerShell:


.\program.exe


**Expected output:**


30

**Note:** The PlainCode compiler generates C source code. GCC compilation and execution are performed separately.

---

## Compiler Phases

### 1. Lexical Analysis

Converts PlainCode source text into tokens.

### 2. Syntax Analysis
Uses a recursive-descent parser to validate syntax and construct an AST.

3. Semantic Analysis

Checks variable usage and detects undefined variables.

4. Three-Address Code Generation

Converts AST expressions and statements into intermediate TAC instructions.

5. TAC Optimization

Performs optimizations including constant folding, constant propagation, algebraic simplification, and removal of unused temporary computations.

6. C Code Generation

Converts optimized TAC into C source code.

7. GCC Compilation

The generated C file can be compiled separately using GCC.

Error Handling

The compiler reports lexical, syntax, and semantic errors with source line and column information where available.

Example:

DISPLAY x + 5

The diagnostic identifies x as undefined and reports its source location.

Generated C programs also include checks for division by zero.

Testing

The integrated compiler has passed seven manual integration tests:

Basic arithmetic and output

Variable reassignment

Operator precedence and parentheses

Undefined-variable detection

Self-referencing first assignment

Integer division

Division-by-zero runtime protection

Individual module tests are available in the tests/ directory.

Run the module test suite on Ubuntu / WSL:

make test

On Windows PowerShell:

mingw32-make test

The test suite includes lexer, parser, semantic, TAC, optimizer, code-generation, control-flow, functions, and data-type coverage.

End-to-end examples are also available in the examples/ directory, including the original demo, control flow, functions and data types, and recursion.

Team Responsibilities

Member 1 — Prachi

Repository setup

Lexical analyzer

Token definitions

Semantic analyzer

TAC optimizer

Compiler integration

Integration testing

Build setup and documentation

Member 2 — Yashita

Grammar implementation

Recursive-descent parser

AST construction

TAC generator

C code generator

Module testing

Project Status

The compiler pipeline supports conditional statements, loops, comparisons, user-defined functions, numeric parameters and return values, recursive calls, integer and floating-point values, string assignment and output, source diagnostics, and additional optimizations.

The compiler translates supported PlainCode programs into C code; GCC compiles the generated C separately.

Module regression tests and end-to-end examples cover the original demo, control flow, functions, decimals, strings, and recursion.

Future Development

String concatenation and string function parameters/returns

Stronger static type inference and conversions

Compile-time detection of division by zero

Removal of unused assignments

Automatic GCC compilation and execution

Automated end-to-end testing

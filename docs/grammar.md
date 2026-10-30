# PlainCode Language Specification

## Overview
PlainCode is an English-like language compiled by the C++17 compiler into C source code. GCC can then compile the generated C program.

## Keywords
`SET`, `TO`, `DISPLAY`, `IF`, `THEN`, `ELSE`, `ENDIF`, `WHILE`, `DO`, `ENDWHILE`, `FUNCTION`, `ENDFUNCTION`, `RETURN`. Keywords are uppercase and case-sensitive.

## Variables and expressions
- Numeric variables support integer and decimal (floating-point) values. Their numeric type is inferred from assigned expressions; an existing variable cannot be changed into a string or vice versa.
- String literals use double quotes, for example `"hello"`. String variables can be assigned and displayed. String concatenation and string arithmetic are not supported.
- `SET name TO expression` creates or updates a variable; `DISPLAY expression` prints a number or string.
- Numeric expressions support `+`, `-`, `*`, `/`, parentheses and comparisons `<`, `>`, `<=`, `>=`, `==`, `!=` (also `<>` for not-equal).
- Multiplication and division bind more tightly than addition and subtraction. Comparisons have lower precedence than arithmetic.
- A variable must be assigned before it is read. A variable first assigned in an IF/ELSE is available after the statement only if assigned in both branches. A variable first assigned inside a WHILE body is not guaranteed to exist after the loop.

## Control flow

```plain
SET age TO 20
IF age >= 18 THEN
    DISPLAY 1
ELSE
    DISPLAY 0
ENDIF

SET count TO 1
WHILE count <= 5 DO
    DISPLAY count
    SET count TO count + 1
ENDWHILE
```

IF statements require `THEN` and terminate with `ENDIF`. ELSE is optional. WHILE statements require `DO` and terminate with `ENDWHILE`. Blocks may contain nested IF and WHILE statements.

## User-defined functions
Functions are declared at the top level. Parameters are comma-separated identifiers, parameters and return values are numeric, and every function must contain a RETURN statement. Calls are expressions and can be nested or recursive.

```plain
FUNCTION add(a, b)
    SET total TO a + b
    RETURN total
ENDFUNCTION

SET answer TO add(12, 30)
DISPLAY answer
```

Function variables are local to that function; global variables are not implicitly visible inside function bodies. The compiler checks that called functions exist and that argument counts match. Functions currently return numeric values; strings are not supported as parameters or return values. The generated C ABI uses `double` for function parameters and return values.

## Grammar (EBNF)

```ebnf
Program        = { Newline | Function | Statement, (Newline | EOF) } ;
Function       = "FUNCTION", Identifier, "(", [ Identifier, { ",", Identifier } ], ")", Newline,
                 { Statement, Newline }, "ENDFUNCTION" ;
Statement      = Assignment | Display | IfStatement | WhileStatement | Return ;
Assignment     = "SET", Identifier, "TO", Expression ;
Display        = "DISPLAY", Expression ;
Return         = "RETURN", Expression ;
IfStatement    = "IF", Comparison, "THEN", Newline,
                 { Statement, Newline },
                 [ "ELSE", Newline, { Statement, Newline } ], "ENDIF" ;
WhileStatement = "WHILE", Comparison, "DO", Newline,
                 { Statement, Newline }, "ENDWHILE" ;
Comparison     = Expression, CompareOp, Expression ;
CompareOp      = "<" | ">" | "<=" | ">=" | "==" | "!=" | "<>" ;
Expression     = Term, { ("+" | "-"), Term } ;
Term           = Factor, { ("*" | "/"), Factor } ;
Factor         = Number | String | Identifier | FunctionCall | "(", Expression, ")" ;
FunctionCall   = Identifier, "(", [ Expression, { ",", Expression } ], ")" ;
Number         = Digit, { Digit }, [ ".", Digit, { Digit } ] ;
String         = '"', { Character | Escape }, '"' ;
```

## Diagnostics and optimization
Lexical, syntax and semantic errors report source line and column where available. The optimizer performs integer constant folding, constant propagation, comparison folding, safe algebraic simplification, and dead temporary elimination in straight-line code. It conservatively avoids dead-temporary removal across control-flow boundaries.

## Current limitations
String concatenation and string function parameters/returns are not supported. Function parameters and return values are numeric and represented as C `double`. Function-local variables are separate from top-level variables. The generated C is compiled separately with GCC.


# PlainCode Language Specification

## 1. Overview
PlainCode is a structured English-like programming language
that will be compiled into C using a compiler written in C++17.

## 2. Supported Features (Review-1)
- Integer variables
- Variable assignment and reassignment
- Arithmetic operations: +, -, *, /
- Parenthesized expressions
- DISPLAY statements

## 3. Keywords
SET, TO, DISPLAY

Keywords are uppercase and case-sensitive.

## 4. Identifiers
- Must begin with a letter or underscore
- May contain letters, digits, and underscores
- Cannot be reserved keywords

## 5. Numbers
Only non-negative integer literals are supported initially.
Negative values may be produced through subtraction.

## 6. Statement Rules
- One statement per line
- Newline terminates a statement
- Blank lines are ignored
- Spaces and tabs separate tokens where necessary
- No semicolons required

## 7. Grammar (EBNF)

Program   = { Newline | Statement, (Newline | EOF) } ;

Statement = "SET", Identifier, "TO", Expression
          | "DISPLAY", Expression ;

Expression = Term, { ("+" | "-"), Term } ;

Term       = Factor, { ("*" | "/"), Factor } ;

Factor     = Identifier
           | Number
           | "(", Expression, ")" ;

Identifier = LetterOrUnderscore,
             { LetterOrUnderscore | Digit } ;

Number     = Digit, { Digit } ;

Newline    = "\n" | "\r\n" ;

## 8. Operator Precedence
1. Parentheses
2. Multiplication and division
3. Addition and subtraction

Operators at the same precedence level are left-associative.

## 9. Variable Rules
- First SET creates an integer variable.
- Later SET updates the existing variable.
- DISPLAY may print an integer expression.
- Using an undefined variable is a semantic error.
- Integer division follows C integer division rules.
- Division by zero must be diagnosed when detectable.

## 10. Valid Programs

SET a TO 10
SET b TO 20
SET result TO a + b * 2
DISPLAY result

## 11. Invalid Programs

SET x 10
Reason: Missing TO keyword.

SET x TO
Reason: Missing expression.

DISPLAY (5 + 2
Reason: Missing closing parenthesis.

SET 123 TO 5
Reason: Invalid identifier.

## 12. Planned Future Features
- INPUT statements
- IF / ELSE / ENDIF
- WHILE / ENDWHILE
- Comparison operators
- Semantic analysis and symbol table
- Three-address code generation
- Basic optimization
- C code generation

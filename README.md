# Mini Expression Compiler

**23CSE314 | Team 17 | Section C**

A complete C-based mini compiler for arithmetic expressions and variable assignments using:

- `Flex` for lexical analysis
- `Bison` for `LALR(1)` parsing
- AST construction in C
- stack-based intermediate code generation
- stack-based evaluation with a persistent symbol table
- interactive REPL, batch mode, demo mode, and Docker support

## Project Structure

```text
mini_compiler/
├── src/
│   ├── lexer.l        Flex token rules
│   ├── parser.y       Bison grammar and AST construction
│   ├── ast.h          AST type definitions
│   ├── ast.c          AST constructors, printer, cleanup
│   ├── codegen.h      IR, symbol table, evaluator declarations
│   ├── codegen.c      IR generation + stack VM evaluator
│   └── main.c         REPL, verbose pipeline view, batch mode
├── Makefile           One-command build
├── Dockerfile         Reproducible container build
├── tests.sh           Shell-based smoke tests
├── sample.txt         Batch-mode sample input
└── README.md
```

## Features

- Arithmetic: `+`, `-`, `*`, `/`
- Parentheses
- Unary minus
- Variable assignment and reuse
- Multiple statements using semicolons
- Full pipeline visualization:
  - token stream
  - AST
  - intermediate stack code
  - final result
- REPL commands:
  - `:verbose`
  - `:vars`
  - `:reset`
  - `:demo`
  - `:help`
  - `:quit`

## WSL Setup

```bash
sudo apt update
sudo apt install -y gcc flex bison make docker.io
cd mini_compiler
make
```

## Run

Interactive REPL:

```bash
./compiler
```

Batch file mode:

```bash
./compiler sample.txt
```

Built-in demo:

```bash
./compiler
:demo
```

Verbose internal pipeline:

```text
>>> :verbose
>>> x = 5 + 3 * 2
```

Output sections:

1. `LEXER - Token Stream`
2. `PARSER - Abstract Syntax Tree`
3. `CODE GENERATOR - Stack-Based Intermediate Code`
4. `EVALUATOR - Result`
5. `SYMBOL TABLE`

## Grammar

```text
statement   -> IDENTIFIER '=' expression | expression
expression  -> expression '+' term
            | expression '-' term
            | term
term        -> term '*' unary
            | term '/' unary
            | unary
unary       -> '-' unary | factor
factor      -> NUMBER | IDENTIFIER | '(' expression ')'
```

## Tests

```bash
chmod +x tests.sh
./tests.sh
```

## Docker

Build:

```bash
docker build -t mini-compiler .
```

Run:

```bash
docker run -it mini-compiler
```

## Team Split

- Member 1: `src/lexer.l` and token design
- Member 2: `src/parser.y`, grammar, and AST
- Member 3: `src/codegen.c`, `src/main.c`, integration, evaluator, demo

## Presentation Lines

- "We used Flex for regex-based lexical analysis."
- "We used Bison to generate an LALR(1) parser."
- "We generate stack-based intermediate code before evaluation."
- "We containerized the project with Docker for reproducibility."

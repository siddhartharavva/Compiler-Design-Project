%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int yylex(void);
void yyerror(const char *message);
void run_statement(ASTNode *node);
extern int yylineno;
%}

%code requires {
#include "ast.h"
}

%union {
    double num;
    char *str;
    ASTNode *node;
}

%token <num> NUMBER
%token <str> IDENTIFIER
%token ASSIGN PLUS MINUS MUL DIV LPAREN RPAREN SEMICOLON INVALID
%type <node> statement expression term unary factor

%left PLUS MINUS
%left MUL DIV
%right UMINUS

%%

input:
      /* empty */
    | input SEMICOLON
    | input statement SEMICOLON { run_statement($2); }
    | input error SEMICOLON     { yyerrok; }
    ;

statement:
      IDENTIFIER ASSIGN expression {
          $$ = ast_assign($1, $3);
          free($1);
      }
    | expression {
          $$ = $1;
      }
    ;

expression:
      expression PLUS term  { $$ = ast_binop('+', $1, $3); }
    | expression MINUS term { $$ = ast_binop('-', $1, $3); }
    | term                  { $$ = $1; }
    ;

term:
      term MUL unary { $$ = ast_binop('*', $1, $3); }
    | term DIV unary { $$ = ast_binop('/', $1, $3); }
    | unary          { $$ = $1; }
    ;

unary:
      MINUS unary %prec UMINUS { $$ = ast_unary('-', $2); }
    | factor                   { $$ = $1; }
    ;

factor:
      NUMBER                   { $$ = ast_num($1); }
    | IDENTIFIER               { $$ = ast_var($1); free($1); }
    | LPAREN expression RPAREN { $$ = $2; }
    ;

%%

void yyerror(const char *message) {
    fprintf(stderr, "SyntaxError at line %d: %s\n", yylineno, message);
}

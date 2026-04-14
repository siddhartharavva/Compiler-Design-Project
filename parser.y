
%code requires {
    typedef struct ASTNode ASTNode;
}


%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex();

/* ================= AST ================= */
typedef struct ASTNode {
    char* type;
    int value;
    char* name;
    struct ASTNode* left;
    struct ASTNode* right;
} ASTNode;

ASTNode* new_int(int val);
ASTNode* new_var(char* name);
ASTNode* new_op(char* type, ASTNode* l, ASTNode* r);
void print_ast(ASTNode* node, int level);
%}

/* ================= UNION ================= */

%union {
    int ival;
    char* sval;
    ASTNode* node;
}

/* ================= TOKENS ================= */

%token <ival> INTEGER
%token <sval> IDENTIFIER
%token PLUS MINUS MUL DIV ASSIGN
%token LPAREN RPAREN SEMICOLON

%type <node> expr statement

/* ================= PRECEDENCE ================= */

%left PLUS MINUS
%left MUL DIV

%%

program:
    program statement
    | /* empty */
;

statement:
    IDENTIFIER ASSIGN expr SEMICOLON {
        $$ = new_op("ASSIGN", new_var($1), $3);
        print_ast($$, 0);
    }
    | expr SEMICOLON {
        $$ = $1;
        print_ast($$, 0);
    }
;

expr:
      expr PLUS expr   { $$ = new_op("ADD", $1, $3); }
    | expr MINUS expr  { $$ = new_op("SUB", $1, $3); }
    | expr MUL expr    { $$ = new_op("MUL", $1, $3); }
    | expr DIV expr    { $$ = new_op("DIV", $1, $3); }
    | LPAREN expr RPAREN { $$ = $2; }
    | INTEGER          { $$ = new_int($1); }
    | IDENTIFIER       { $$ = new_var($1); }
;

%%

/* ================= AST BUILDERS ================= */

ASTNode* new_int(int val) {
    ASTNode* n = malloc(sizeof(ASTNode));
    n->type = "INT";
    n->value = val;
    n->left = n->right = NULL;
    return n;
}

ASTNode* new_var(char* name) {
    ASTNode* n = malloc(sizeof(ASTNode));
    n->type = "VAR";
    n->name = name;
    n->left = n->right = NULL;
    return n;
}

ASTNode* new_op(char* type, ASTNode* l, ASTNode* r) {
    ASTNode* n = malloc(sizeof(ASTNode));
    n->type = type;
    n->left = l;
    n->right = r;
    return n;
}

/* ================= AST PRINT ================= */

void print_ast(ASTNode* node, int level) {
    if (!node) return;

    for (int i = 0; i < level; i++) printf("  ");

    if (strcmp(node->type, "INT") == 0)
        printf("INT(%d)\n", node->value);

    else if (strcmp(node->type, "VAR") == 0)
        printf("VAR(%s)\n", node->name);

    else
        printf("%s\n", node->type);

    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

void yyerror(const char *s) {
    printf("Error: %s\n", s);
}

int main() {
    return yyparse();
}
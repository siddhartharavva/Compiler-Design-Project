#ifndef AST_H
#define AST_H

typedef enum {
    NODE_NUM,
    NODE_VAR,
    NODE_BINOP,
    NODE_UNARY,
    NODE_ASSIGN
} NodeType;

typedef struct ASTNode {
    NodeType type;
    double num_value;
    char var_name[64];
    char op;
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode *ast_num(double value);
ASTNode *ast_var(const char *name);
ASTNode *ast_binop(char op, ASTNode *left, ASTNode *right);
ASTNode *ast_unary(char op, ASTNode *operand);
ASTNode *ast_assign(const char *name, ASTNode *value);

void ast_print_tree(ASTNode *node);
void ast_free(ASTNode *node);

#endif

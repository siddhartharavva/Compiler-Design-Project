#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static ASTNode *new_node(NodeType type) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    node->type = type;
    return node;
}

ASTNode *ast_num(double value) {
    ASTNode *node = new_node(NODE_NUM);
    node->num_value = value;
    return node;
}

ASTNode *ast_var(const char *name) {
    ASTNode *node = new_node(NODE_VAR);
    strncpy(node->var_name, name, sizeof(node->var_name) - 1);
    return node;
}

ASTNode *ast_binop(char op, ASTNode *left, ASTNode *right) {
    ASTNode *node = new_node(NODE_BINOP);
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

ASTNode *ast_unary(char op, ASTNode *operand) {
    ASTNode *node = new_node(NODE_UNARY);
    node->op = op;
    node->left = operand;
    return node;
}

ASTNode *ast_assign(const char *name, ASTNode *value) {
    ASTNode *node = new_node(NODE_ASSIGN);
    strncpy(node->var_name, name, sizeof(node->var_name) - 1);
    node->left = value;
    return node;
}

static void ast_print(ASTNode *node, int depth, int is_last) {
    if (!node) {
        return;
    }

    for (int i = 0; i < depth - 1; i++) {
        printf("    ");
    }
    if (depth > 0) {
        printf(is_last ? "|__ " : "|-- ");
    }

    switch (node->type) {
        case NODE_NUM:
            if (node->num_value == (int) node->num_value) {
                printf("Num: %d\n", (int) node->num_value);
            } else {
                printf("Num: %.6g\n", node->num_value);
            }
            break;
        case NODE_VAR:
            printf("Var: %s\n", node->var_name);
            break;
        case NODE_BINOP:
            printf("BinOp: %c\n", node->op);
            ast_print(node->left, depth + 1, 0);
            ast_print(node->right, depth + 1, 1);
            break;
        case NODE_UNARY:
            printf("Unary: %c\n", node->op);
            ast_print(node->left, depth + 1, 1);
            break;
        case NODE_ASSIGN:
            printf("Assign: %s\n", node->var_name);
            ast_print(node->left, depth + 1, 1);
            break;
    }
}

void ast_print_tree(ASTNode *node) {
    ast_print(node, 0, 1);
}

void ast_free(ASTNode *node) {
    if (!node) {
        return;
    }
    ast_free(node->left);
    ast_free(node->right);
    free(node);
}

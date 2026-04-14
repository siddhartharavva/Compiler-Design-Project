#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

typedef enum {
    OP_PUSH_NUM,
    OP_PUSH_VAR,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NEG,
    OP_STORE
} OpCode;

typedef struct {
    OpCode op;
    double num_val;
    char var_name[64];
} Instruction;

#define MAX_INSTRUCTIONS 1024
#define MAX_VARS 256

typedef struct {
    Instruction instrs[MAX_INSTRUCTIONS];
    int count;
} Program;

typedef struct {
    char name[64];
    double value;
    int defined;
} VarEntry;

typedef struct {
    VarEntry entries[MAX_VARS];
    int count;
} SymbolTable;

void program_init(Program *program);
void codegen(ASTNode *node, Program *program);
void program_dump(const Program *program);

void symtab_init(SymbolTable *table);
void symtab_set(SymbolTable *table, const char *name, double value);
int symtab_get(SymbolTable *table, const char *name, double *out);
void symtab_dump(const SymbolTable *table);

double execute(const Program *program, SymbolTable *table);

#endif

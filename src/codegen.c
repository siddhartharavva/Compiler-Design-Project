#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

void symtab_init(SymbolTable *table) {
    table->count = 0;
    memset(table->entries, 0, sizeof(table->entries));
}

void symtab_set(SymbolTable *table, const char *name, double value) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->entries[i].name, name) == 0) {
            table->entries[i].value = value;
            table->entries[i].defined = 1;
            return;
        }
    }

    if (table->count >= MAX_VARS) {
        fprintf(stderr, "Symbol table full\n");
        return;
    }

    strncpy(table->entries[table->count].name, name, 63);
    table->entries[table->count].value = value;
    table->entries[table->count].defined = 1;
    table->count++;
}

int symtab_get(SymbolTable *table, const char *name, double *out) {
    for (int i = 0; i < table->count; i++) {
        if (table->entries[i].defined && strcmp(table->entries[i].name, name) == 0) {
            *out = table->entries[i].value;
            return 1;
        }
    }
    return 0;
}

void symtab_dump(const SymbolTable *table) {
    if (table->count == 0) {
        printf("  (empty)\n");
        return;
    }

    for (int i = 0; i < table->count; i++) {
        if (!table->entries[i].defined) {
            continue;
        }
        double value = table->entries[i].value;
        if (value == (int) value) {
            printf("  %-12s = %d\n", table->entries[i].name, (int) value);
        } else {
            printf("  %-12s = %.6g\n", table->entries[i].name, value);
        }
    }
}

void program_init(Program *program) {
    program->count = 0;
}

static void emit(Program *program, OpCode op, double num_val, const char *var_name) {
    if (program->count >= MAX_INSTRUCTIONS) {
        fprintf(stderr, "Instruction buffer full\n");
        return;
    }

    Instruction *instr = &program->instrs[program->count++];
    instr->op = op;
    instr->num_val = num_val;
    instr->var_name[0] = '\0';
    if (var_name) {
        strncpy(instr->var_name, var_name, sizeof(instr->var_name) - 1);
    }
}

void codegen(ASTNode *node, Program *program) {
    if (!node) {
        return;
    }

    switch (node->type) {
        case NODE_NUM:
            emit(program, OP_PUSH_NUM, node->num_value, NULL);
            break;
        case NODE_VAR:
            emit(program, OP_PUSH_VAR, 0, node->var_name);
            break;
        case NODE_BINOP:
            codegen(node->left, program);
            codegen(node->right, program);
            switch (node->op) {
                case '+': emit(program, OP_ADD, 0, NULL); break;
                case '-': emit(program, OP_SUB, 0, NULL); break;
                case '*': emit(program, OP_MUL, 0, NULL); break;
                case '/': emit(program, OP_DIV, 0, NULL); break;
            }
            break;
        case NODE_UNARY:
            codegen(node->left, program);
            emit(program, OP_NEG, 0, NULL);
            break;
        case NODE_ASSIGN:
            codegen(node->left, program);
            emit(program, OP_STORE, 0, node->var_name);
            break;
    }
}

void program_dump(const Program *program) {
    for (int i = 0; i < program->count; i++) {
        const Instruction *instr = &program->instrs[i];
        printf("  [%02d] ", i);
        switch (instr->op) {
            case OP_PUSH_NUM:
                if (instr->num_val == (int) instr->num_val) {
                    printf("PUSH  %d\n", (int) instr->num_val);
                } else {
                    printf("PUSH  %.6g\n", instr->num_val);
                }
                break;
            case OP_PUSH_VAR:
                printf("PUSH  %s\n", instr->var_name);
                break;
            case OP_ADD:
                printf("ADD\n");
                break;
            case OP_SUB:
                printf("SUB\n");
                break;
            case OP_MUL:
                printf("MUL\n");
                break;
            case OP_DIV:
                printf("DIV\n");
                break;
            case OP_NEG:
                printf("NEG\n");
                break;
            case OP_STORE:
                printf("STORE %s\n", instr->var_name);
                break;
        }
    }
}

#define STACK_SIZE 256

double execute(const Program *program, SymbolTable *table) {
    double stack[STACK_SIZE];
    int top = 0;
    double result = 0.0;

    for (int i = 0; i < program->count; i++) {
        const Instruction *instr = &program->instrs[i];

        switch (instr->op) {
            case OP_PUSH_NUM:
                if (top >= STACK_SIZE) {
                    fprintf(stderr, "Stack overflow\n");
                    exit(1);
                }
                stack[top++] = instr->num_val;
                break;

            case OP_PUSH_VAR: {
                double value;
                if (!symtab_get(table, instr->var_name, &value)) {
                    fprintf(stderr, "NameError: variable '%s' is not defined\n", instr->var_name);
                    exit(1);
                }
                if (top >= STACK_SIZE) {
                    fprintf(stderr, "Stack overflow\n");
                    exit(1);
                }
                stack[top++] = value;
                break;
            }

            case OP_NEG:
                if (top < 1) {
                    fprintf(stderr, "Stack underflow\n");
                    exit(1);
                }
                stack[top - 1] = -stack[top - 1];
                break;

            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV: {
                if (top < 2) {
                    fprintf(stderr, "Stack underflow\n");
                    exit(1);
                }
                double right = stack[--top];
                double left = stack[--top];
                double value = 0.0;

                switch (instr->op) {
                    case OP_ADD: value = left + right; break;
                    case OP_SUB: value = left - right; break;
                    case OP_MUL: value = left * right; break;
                    case OP_DIV:
                        if (right == 0.0) {
                            fprintf(stderr, "ZeroDivisionError: division by zero\n");
                            exit(1);
                        }
                        value = left / right;
                        break;
                    default:
                        break;
                }

                stack[top++] = value;
                break;
            }

            case OP_STORE: {
                if (top < 1) {
                    fprintf(stderr, "Stack underflow\n");
                    exit(1);
                }
                double value = stack[top - 1];
                symtab_set(table, instr->var_name, value);
                result = value;
                break;
            }
        }

        if (top > 0) {
            result = stack[top - 1];
        }
    }

    if (result == (int) result) {
        return (int) result;
    }
    return result;
}

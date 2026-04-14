#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "codegen.h"

SymbolTable g_symtab;
int g_verbose = 0;

extern int yyparse(void);
extern FILE *yyin;

static void section(const char *title) {
    printf("\n==================================================\n");
    printf("  %s\n", title);
    printf("==================================================\n");
}

void run_statement(ASTNode *node) {
    if (!node) {
        return;
    }

    if (g_verbose) {
        section("PARSER - Abstract Syntax Tree");
        ast_print_tree(node);
    }

    Program program;
    program_init(&program);
    codegen(node, &program);

    if (g_verbose) {
        section("CODE GENERATOR - Stack-Based Intermediate Code");
        program_dump(&program);
    }

    double result = execute(&program, &g_symtab);

    if (g_verbose) {
        section("EVALUATOR - Result");
    }

    if (node->type == NODE_ASSIGN) {
        double value = 0.0;
        symtab_get(&g_symtab, node->var_name, &value);
        if (value == (int) value) {
            printf("  %s = %d\n", node->var_name, (int) value);
        } else {
            printf("  %s = %.6g\n", node->var_name, value);
        }
    } else {
        if (result == (int) result) {
            printf("  %d\n", (int) result);
        } else {
            printf("  %.6g\n", result);
        }
    }

    if (g_verbose) {
        section("SYMBOL TABLE");
        symtab_dump(&g_symtab);
    }

    ast_free(node);
}

static void print_banner(void) {
    printf("Mini Expression Compiler v2.0\n");
    printf("Flex + Bison + C + Stack VM\n");
    printf("Commands: :verbose  :vars  :reset  :demo  :help  :quit\n\n");
}

static void print_tokens_from_source(const char *line) {
    const char *p = line;
    int first = 1;

    printf("  ");
    while (*p) {
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (!*p) {
            break;
        }

        if (!first) {
            printf(" -> ");
        }
        first = 0;

        if ((*p >= '0' && *p <= '9') || (*p == '.' && p[1] >= '0' && p[1] <= '9')) {
            char buffer[64];
            int i = 0;
            while ((*p >= '0' && *p <= '9') || *p == '.') {
                buffer[i++] = *p++;
            }
            buffer[i] = '\0';
            printf("NUMBER(%s)", buffer);
        } else if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_') {
            char buffer[64];
            int i = 0;
            while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
                   (*p >= '0' && *p <= '9') || *p == '_') {
                buffer[i++] = *p++;
            }
            buffer[i] = '\0';
            printf("IDENTIFIER(%s)", buffer);
        } else {
            switch (*p) {
                case '=': printf("ASSIGN"); break;
                case '+': printf("PLUS"); break;
                case '-': printf("MINUS"); break;
                case '*': printf("TIMES"); break;
                case '/': printf("DIVIDE"); break;
                case '(': printf("LPAREN"); break;
                case ')': printf("RPAREN"); break;
                case ';': printf("SEMICOLON"); break;
                default:  printf("UNKNOWN(%c)", *p); break;
            }
            p++;
        }
    }
    printf("\n");
}

static void parse_source_string(const char *source) {
    FILE *tmp = tmpfile();
    if (!tmp) {
        perror("tmpfile");
        return;
    }

    fprintf(tmp, "%s\n", source);
    rewind(tmp);
    yyin = tmp;
    yyparse();
    fclose(tmp);
}

static void run_demo(void) {
    int old_verbose = g_verbose;
    g_verbose = 1;

    printf("\nFULL PIPELINE DEMONSTRATION\n");
    printf("---------------------------\n");

    const char *demo_inputs[] = {
        "x = 5 + 3 * 2",
        "y = (5 + 3) * 2",
        "z = x + y",
        "neg = -x + 1",
        "x = 5 +"
    };

    for (size_t i = 0; i < sizeof(demo_inputs) / sizeof(demo_inputs[0]); i++) {
        printf("\n>>> %s\n", demo_inputs[i]);
        section("LEXER - Token Stream");
        print_tokens_from_source(demo_inputs[i]);
        parse_source_string(demo_inputs[i]);
    }

    g_verbose = old_verbose;
}

static void repl(void) {
    char line[1024];
    print_banner();

    while (1) {
        printf(">>> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\nBye!\n");
            break;
        }

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        if (line[0] == '\0') {
            continue;
        }

        if (strcmp(line, ":quit") == 0 || strcmp(line, ":q") == 0) {
            printf("Bye!\n");
            break;
        }
        if (strcmp(line, ":verbose") == 0) {
            g_verbose = !g_verbose;
            printf("  Verbose: %s\n", g_verbose ? "ON" : "OFF");
            continue;
        }
        if (strcmp(line, ":vars") == 0) {
            printf("\n  Symbol Table:\n");
            symtab_dump(&g_symtab);
            printf("\n");
            continue;
        }
        if (strcmp(line, ":reset") == 0) {
            symtab_init(&g_symtab);
            printf("  Symbol table cleared.\n");
            continue;
        }
        if (strcmp(line, ":demo") == 0) {
            run_demo();
            continue;
        }
        if (strcmp(line, ":help") == 0) {
            print_banner();
            continue;
        }

        if (g_verbose) {
            section("LEXER - Token Stream");
            print_tokens_from_source(line);
        }
        parse_source_string(line);
    }
}

int main(int argc, char *argv[]) {
    symtab_init(&g_symtab);

    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror(argv[1]);
            return 1;
        }
        yyin = file;
        g_verbose = 1;
        yyparse();
        fclose(file);
        return 0;
    }

    repl();
    return 0;
}

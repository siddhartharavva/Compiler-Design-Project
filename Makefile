# ═══════════════════════════════════════════════════════════════
# Makefile  —  Mini Expression Compiler
# Usage:
#   make          build the compiler binary
#   make run      build + launch REPL
#   make demo     build + run the built-in demo sequence
#   make clean    remove generated files
# ═══════════════════════════════════════════════════════════════

CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
LDFLAGS = -lm

# Source / generated paths
SRCDIR  = src
GENDIR  = src/gen

LEX_SRC  = $(SRCDIR)/lexer.l
BISON_SRC= $(SRCDIR)/parser.y

LEX_C    = $(GENDIR)/lex.yy.c
BISON_C  = $(GENDIR)/parser.tab.c
BISON_H  = $(GENDIR)/parser.tab.h

SRCS = $(SRCDIR)/ast.c \
       $(SRCDIR)/codegen.c \
       $(SRCDIR)/main.c \
       $(BISON_C) \
       $(LEX_C)

TARGET = compiler

# ── default target ──────────────────────────────────────────────
all: $(TARGET)

# ── ensure generated dir exists ────────────────────────────────
$(GENDIR):
	mkdir -p $(GENDIR)

# ── Bison: parser.y → parser.tab.c + parser.tab.h ──────────────
$(BISON_C) $(BISON_H): $(BISON_SRC) | $(GENDIR)
	bison -d -o $(BISON_C) $(BISON_SRC)
	@echo "  [BISON] Generated LALR(1) parser"

# ── Flex: lexer.l → lex.yy.c ───────────────────────────────────
$(LEX_C): $(LEX_SRC) $(BISON_H) | $(GENDIR)
	flex -o $(LEX_C) $(LEX_SRC)
	@echo "  [FLEX]  Generated lexer"

# ── Compile everything ──────────────────────────────────────────
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -I$(GENDIR) $(SRCS) $(LDFLAGS) -o $(TARGET)
	@echo "  [GCC]   Compiled → $(TARGET)"
	@echo ""
	@echo "  Build complete!  Run:  ./compiler"

# ── Convenience targets ─────────────────────────────────────────
run: all
	./$(TARGET)

demo: all
	@echo ":demo" | ./$(TARGET)

clean:
	rm -rf $(GENDIR) $(TARGET)
	@echo "  Cleaned."

.PHONY: all run demo clean

# ═══════════════════════════════════════════════════════════════
# Dockerfile  —  Mini Expression Compiler
# ───────────────────────────────────────
# Builds and runs the compiler in a reproducible Ubuntu container.
#
# Build:   docker build -t mini-compiler .
# Run:     docker run -it mini-compiler          (REPL)
#          docker run -it mini-compiler demo     (demo mode)
# ═══════════════════════════════════════════════════════════════

FROM ubuntu:22.04

LABEL maintainer="Team 17 — 23CSE314 Compiler Design"
LABEL description="Mini Expression Compiler: Flex + Bison + C"

# ── Install build tools ─────────────────────────────────────────
RUN apt-get update && apt-get install -y \
    gcc \
    flex \
    bison \
    make \
    && rm -rf /var/lib/apt/lists/*

# ── Copy project ────────────────────────────────────────────────
WORKDIR /app
COPY . .

# ── Build ───────────────────────────────────────────────────────
RUN make

# ── Default: launch REPL ────────────────────────────────────────
ENTRYPOINT ["./compiler"]

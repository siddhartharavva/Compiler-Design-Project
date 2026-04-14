#!/usr/bin/env bash
set -euo pipefail

cleanup() {
  rm -f /tmp/mini_compiler_test.out
}
trap cleanup EXIT

run_case() {
  local name="$1"
  local input="$2"
  local expected="$3"

  printf "%b" "$input" | ./compiler > /tmp/mini_compiler_test.out 2>&1 || true
  if grep -qF "$expected" /tmp/mini_compiler_test.out; then
    echo "[PASS] $name"
  else
    echo "[FAIL] $name"
    echo "Expected to find: $expected"
    echo "Actual output:"
    cat /tmp/mini_compiler_test.out
    exit 1
  fi
}

run_case "basic arithmetic" "3 + 4 * 2\n:quit\n" "11"
run_case "parentheses" "(3 + 4) * 2\n:quit\n" "14"
run_case "assignment" "x = 5\n:quit\n" "x = 5"
run_case "variable reuse" "x = 5\nx + 3\n:quit\n" "8"
run_case "unary minus" "-5 + 2\n:quit\n" "-3"
run_case "division" "7 / 2\n:quit\n" "3.5"
run_case "zero division" "10 / 0\n:quit\n" "ZeroDivisionError"
run_case "syntax error" "x = 5 +\n:quit\n" "SyntaxError"
run_case "undefined variable" "missing + 1\n:quit\n" "NameError"
run_case "semicolon separated" "a = 1; b = a + 2\n:quit\n" "b = 3"

echo "All shell tests passed."

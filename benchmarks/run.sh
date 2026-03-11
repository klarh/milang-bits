#!/bin/bash
# Benchmark runner: milang vs Python
# Usage: ./run.sh [milang_binary]
#
# milang compiles to C which embeds an AST interpreter, so "milang (run)"
# includes compile+link+interpret and "milang (bin)" is interpret only.

MILANG="${1:-../milang}"
cd "$(dirname "$0")"
MILANG="$(cd - > /dev/null && realpath "$MILANG" 2>/dev/null || echo "$MILANG")"

RED='\033[0;31m'
GREEN='\033[0;32m'
BOLD='\033[1m'
DIM='\033[2m'
RESET='\033[0m'

time_cmd() {
  local start end ms out
  start=$(date +%s%3N)
  out=$(eval "$1" 2>&1)
  end=$(date +%s%3N)
  ms=$((end - start))
  echo "$ms $out"
}

printf "${BOLD}%-16s %11s %11s %11s  %-16s${RESET}\n" \
  "Benchmark" "milang(run)" "milang(bin)" "Python" "Result"
printf "%-16s %11s %11s %11s  %-16s\n" \
  "----------------" "-----------" "-----------" "-----------" "----------------"

for bench in bench_nested bench_primes bench_collatz bench_list; do
  [ -f "${bench}.mi" ] && [ -f "${bench}.py" ] || continue

  # milang: compile+run
  read run_ms mi_out <<< $(time_cmd "$MILANG run ${bench}.mi")

  # milang: pre-compiled binary only
  $MILANG compile "${bench}.mi" -o "/tmp/${bench}_bin" > /dev/null 2>&1
  read bin_ms bin_out <<< $(time_cmd "/tmp/${bench}_bin")

  # python
  read py_ms py_out <<< $(time_cmd "python3 ${bench}.py")

  if [ "$mi_out" = "$py_out" ] && [ "$mi_out" = "$bin_out" ]; then
    match="${GREEN}✓${RESET} $mi_out"
  else
    match="${RED}✗${RESET} mi=$mi_out py=$py_out"
  fi

  printf "%-16s %9dms %9dms %9dms  ${match}\n" \
    "$bench" "$run_ms" "$bin_ms" "$py_ms"
done

echo ""
printf "${DIM}milang(run) = compile + link + run; milang(bin) = pre-compiled binary${RESET}\n"

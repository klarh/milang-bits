#!/bin/bash
# Timeit-style benchmark runner: warm up, then measure min of N runs
# Usage: ./run_timeit.sh [milang_binary] [num_runs]

MILANG="${1:-../milang}"
RUNS="${2:-5}"
cd "$(dirname "$0")"
MILANG="$(cd - > /dev/null && realpath "$MILANG" 2>/dev/null || echo "$MILANG")"

RED='\033[0;31m'
GREEN='\033[0;32m'
BOLD='\033[1m'
DIM='\033[2m'
RESET='\033[0m'

time_min() {
  local cmd="$1" n="$2" best=999999 ms out
  # Warm-up run (discard)
  eval "$cmd" > /dev/null 2>&1
  for ((i=0; i<n; i++)); do
    local start end
    start=$(date +%s%3N)
    out=$(eval "$cmd" 2>&1)
    end=$(date +%s%3N)
    ms=$((end - start))
    if ((ms < best)); then best=$ms; fi
  done
  echo "$best $out"
}

printf "${BOLD}%-16s %11s %11s %11s %11s${RESET}\n" \
  "Benchmark" "milang(bin)" "Python" "numpy" "Ratio(mi/py)"
printf "%-16s %11s %11s %11s %11s\n" \
  "────────────────" "───────────" "───────────" "───────────" "────────────"

for bench in bench_nested bench_primes bench_collatz bench_list bench_mandelbrot bench_fibonacci bench_ackermann bench_sort bench_tree bench_strings bench_closure; do
  [ -f "${bench}.mi" ] || continue

  # milang: pre-compiled binary
  $MILANG compile "${bench}.mi" -o "/tmp/${bench}_bin" > /dev/null 2>&1
  if [ -f "/tmp/${bench}_bin" ]; then
    read bin_ms mi_out <<< $(time_min "/tmp/${bench}_bin" "$RUNS")
  else
    bin_ms="ERR"; mi_out="N/A"
  fi

  # Python
  if [ -f "${bench}.py" ]; then
    read py_ms py_out <<< $(time_min "python3 ${bench}.py" "$RUNS")
  else
    py_ms="-"; py_out="N/A"
  fi

  # numpy
  if [ -f "${bench}_numpy.py" ]; then
    read np_ms np_out <<< $(time_min "python3 ${bench}_numpy.py" "$RUNS")
  else
    np_ms="-"; np_out=""
  fi

  # Ratio
  if [[ "$bin_ms" =~ ^[0-9]+$ ]] && [[ "$py_ms" =~ ^[0-9]+$ ]] && ((py_ms > 0)); then
    ratio=$(python3 -c "print(f'{$bin_ms/$py_ms:.2f}x')")
  else
    ratio="-"
  fi

  printf "%-16s %9dms %9dms %9sms %12s\n" \
    "$bench" "$bin_ms" "$py_ms" "${np_ms}" "$ratio"
done

echo ""
printf "${DIM}milang(bin) = pre-compiled binary. Min of $RUNS runs after 1 warm-up.${RESET}\n"

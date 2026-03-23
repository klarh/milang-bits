#!/bin/bash
# Sort algorithm comparison: milang vs Python across sorting strategies
# Usage: ./run_sort_compare.sh [milang_binary] [num_runs]

MILANG="${1:-../milang}"
RUNS="${2:-3}"
cd "$(dirname "$0")"
MILANG="$(cd - > /dev/null && realpath "$MILANG" 2>/dev/null || echo "$MILANG")"

BOLD='\033[1m'
DIM='\033[2m'
RESET='\033[0m'

time_min() {
  local cmd="$1" n="$2" best=999999 ms out
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

printf "${BOLD}Sort Algorithm Comparison (n=5000 pseudo-random integers)${RESET}\n\n"
printf "${BOLD}%-12s %11s %11s %11s${RESET}\n" \
  "Algorithm" "milang(bin)" "Python" "Ratio"
printf "%-12s %11s %11s %11s\n" \
  "────────────" "───────────" "───────────" "───────────"

for entry in "qsort:quicksort" "msort:mergesort" "tsort:treesort"; do
  algo="${entry%%:*}"
  label="${entry##*:}"
  mi="bench_${algo}.mi"
  py="bench_${algo}.py"
  [ -f "$mi" ] || continue

  $MILANG compile "$mi" -o "/tmp/bench_${algo}_bin" > /dev/null 2>&1
  if [ -f "/tmp/bench_${algo}_bin" ]; then
    read mi_ms mi_out <<< $(time_min "/tmp/bench_${algo}_bin" "$RUNS")
  else
    mi_ms="ERR"; mi_out="N/A"
  fi

  if [ -f "$py" ]; then
    read py_ms py_out <<< $(time_min "python3 $py" "$RUNS")
  else
    py_ms="-"; py_out="N/A"
  fi

  if [[ "$mi_ms" =~ ^[0-9]+$ ]] && [[ "$py_ms" =~ ^[0-9]+$ ]] && ((py_ms > 0)); then
    ratio=$(python3 -c "print(f'{$mi_ms/$py_ms:.2f}x')")
  else
    ratio="-"
  fi

  printf "%-12s %9dms %9dms %11s\n" "$label" "$mi_ms" "$py_ms" "$ratio"
done

echo ""
printf "${DIM}Both languages use functional implementations of each algorithm.${RESET}\n"
printf "${DIM}Min of $RUNS runs after 1 warm-up. Compiled milang binary.${RESET}\n"

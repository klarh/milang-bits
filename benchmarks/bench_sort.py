"""Benchmark: Functional quicksort on 5000 pseudo-random integers"""
import sys
sys.setrecursionlimit(10000)

def qsort(xs):
    if len(xs) < 2:
        return xs
    p = xs[0]
    rest = xs[1:]
    lo = [x for x in rest if x < p]
    hi = [x for x in rest if x >= p]
    return qsort(lo) + [p] + qsort(hi)

s = 42
data = []
for _ in range(5000):
    data.append(s % 10000)
    s = (s * 1103515245 + 12345) % 2147483648
print(sum(qsort(data)))

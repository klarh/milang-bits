"""Benchmark: nested loop — sum of (i * j % 1000) for i,j in 1..500"""

def compute(n):
    total = 0
    for i in range(1, n + 1):
        for j in range(1, n + 1):
            total += i * j % 1000
    return total

print(compute(500))

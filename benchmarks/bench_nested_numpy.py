"""Benchmark: nested loop — sum of (i * j % 1000) for i,j in 1..500 (numpy)"""
import numpy as np

def compute(n):
    i = np.arange(1, n + 1)
    j = np.arange(1, n + 1)
    return int(np.sum(np.outer(i, j) % 1000))

print(compute(500))

"""Benchmark: Merge sort on 5000 pseudo-random integers"""
import sys
sys.setrecursionlimit(20000)

def msort(xs):
    if len(xs) <= 1:
        return xs
    mid = len(xs) // 2
    left = msort(xs[:mid])
    right = msort(xs[mid:])
    return merge(left, right)

def merge(xs, ys):
    result = []
    i = j = 0
    while i < len(xs) and j < len(ys):
        if xs[i] <= ys[j]:
            result.append(xs[i])
            i += 1
        else:
            result.append(ys[j])
            j += 1
    result.extend(xs[i:])
    result.extend(ys[j:])
    return result

s = 42
data = []
for _ in range(5000):
    data.append(s % 10000)
    s = (s * 1103515245 + 12345) % 2147483648
print(sum(msort(data)))

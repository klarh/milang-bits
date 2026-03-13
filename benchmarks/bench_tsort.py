"""Benchmark: Treesort (BST insert + in-order flatten) on 5000 pseudo-random integers"""
import sys
sys.setrecursionlimit(20000)

# BST node: None = leaf, (left, val, right) = node
def ins(t, x):
    if t is None:
        return (None, x, None)
    l, v, r = t
    if x < v:
        return (ins(l, x), v, r)
    else:
        return (l, v, ins(r, x))

def flatten(t, acc):
    if t is None:
        return acc
    l, v, r = t
    return flatten(l, [v] + flatten(r, acc))

def tsort(xs):
    t = None
    for x in xs:
        t = ins(t, x)
    return flatten(t, [])

s = 42
data = []
for _ in range(5000):
    data.append(s % 10000)
    s = (s * 1103515245 + 12345) % 2147483648
print(sum(tsort(data)))

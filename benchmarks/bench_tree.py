"""Benchmark: Binary search tree — insert 10000 values, sum all nodes"""

class Node:
    __slots__ = ('val', 'left', 'right')
    def __init__(self, v, l=None, r=None):
        self.val = v; self.left = l; self.right = r

def insert(t, v):
    if t is None:
        return Node(v)
    if v < t.val:
        return Node(t.val, insert(t.left, v), t.right)
    return Node(t.val, t.left, insert(t.right, v))

def sum_tree(t):
    if t is None:
        return 0
    stack = [t]
    total = 0
    while stack:
        n = stack.pop()
        total += n.val
        if n.left: stack.append(n.left)
        if n.right: stack.append(n.right)
    return total

s = 42
t = None
for _ in range(10000):
    t = insert(t, s % 100000)
    s = (s * 1103515245 + 12345) % 2147483648
print(sum_tree(t))

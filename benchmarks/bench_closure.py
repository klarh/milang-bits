"""Benchmark: Closure creation and application"""

def apply_n(f, n, x):
    for _ in range(n):
        x = f(x)
    return x

closures = [lambda x, i=i: x + i for i in range(1, 10001)]

def apply_all(fns, x):
    for f in fns:
        x = f(x)
    return x

r1 = apply_n(lambda x: x + 1, 1000000, 0)
r2 = apply_all(closures, 0)
print(r1 + r2)

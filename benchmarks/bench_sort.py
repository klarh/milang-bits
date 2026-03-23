"""Benchmark: Sort 5000 pseudo-random integers using built-in sorted()"""

s = 42
data = []
for _ in range(5000):
    data.append(s % 10000)
    s = (s * 1103515245 + 12345) % 2147483648
print(sum(sorted(data)))

"""Benchmark: total Collatz sequence steps for 1..100,000"""

def collatz(n):
    steps = 0
    while n > 1:
        n = n // 2 if n % 2 == 0 else 3 * n + 1
        steps += 1
    return steps

print(sum(collatz(i) for i in range(1, 100001)))

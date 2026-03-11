"""Benchmark: count primes up to 100,000 (trial division)"""

def is_prime(n):
    if n < 2:
        return False
    d = 2
    while d * d <= n:
        if n % d == 0:
            return False
        d += 1
    return True

print(sum(1 for i in range(2, 100001) if is_prime(i)))

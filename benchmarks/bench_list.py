"""Benchmark: functional list pipeline (filter + map + fold) over 100k elements"""

xs = list(range(1, 100001))
filtered = [x for x in xs if x % 3 > 0]
mapped = [x % 97 for x in filtered]
result = sum(mapped)
print(result)

"""Benchmark: functional list pipeline (filter + map + fold) over 100k elements (numpy)"""
import numpy as np

xs = np.arange(1, 100001)
filtered = xs[xs % 3 > 0]
mapped = filtered % 97
result = int(np.sum(mapped))
print(result)

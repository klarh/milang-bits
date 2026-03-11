"""Benchmark: String processing — build, reverse, scan"""

s = ""
for i in range(1000):
    s += "abcdefghij"
rev = s[::-1]
print(len(s) + len(rev) + s.count("a") + rev.count("a"))

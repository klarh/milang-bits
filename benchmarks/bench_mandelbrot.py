"""Benchmark: Mandelbrot set — 200x200 grid, max 100 iterations
x in [-2, 1), y in [-1.5, 1.5), dx=dy=0.015"""

def mandel_iter(cx, cy, max_iters):
    zr, zi = 0.0, 0.0
    for i in range(max_iters):
        if zr * zr + zi * zi > 4.0:
            return i
        zr, zi = zr * zr - zi * zi + cx, 2.0 * zr * zi + cy
    return max_iters

def compute(rows, cols, max_iters):
    total = 0
    cy = -1.5
    for _ in range(rows):
        cx = -2.0
        for _ in range(cols):
            total += mandel_iter(cx, cy, max_iters)
            cx += 0.015
        cy += 0.015
    return total

print(compute(200, 200, 100))

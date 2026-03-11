"""Benchmark: Mandelbrot set (fixed-point) — 200x200 grid, max 100 iterations
Uses integer fixed-point arithmetic (scale=1024) to match milang version.
Uses C-style truncation division (int(a/b)) to match milang's native codegen."""

def mandel_iter(cx, cy, max_iters):
    zr, zi = 0, 0
    for i in range(max_iters):
        zr2 = int(zr * zr / 1024)
        zi2 = int(zi * zi / 1024)
        if zr2 + zi2 > 4096:
            return i
        zr, zi = zr2 - zi2 + cx, int(2 * zr * zi / 1024) + cy
    return max_iters

def compute(rows, cols, max_iters):
    total = 0
    cy = -1536
    for _ in range(rows):
        cx = -2048
        for _ in range(cols):
            total += mandel_iter(cx, cy, max_iters)
            cx += 15
        cy += 15
    return total

print(compute(200, 200, 100))

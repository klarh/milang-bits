"""Benchmark: Mandelbrot set — numpy vectorized, 200x200 grid
x in [-2, 1), y in [-1.5, 1.5), dx=dy=0.015
Note: output may differ slightly from scalar version due to
floating-point accumulation order at escape boundaries."""
import numpy as np

def compute(rows, cols, max_iters):
    cy_vals = np.arange(rows, dtype=np.float64) * 0.015 - 1.5
    cx_vals = np.arange(cols, dtype=np.float64) * 0.015 - 2.0
    cx, cy = np.meshgrid(cx_vals, cy_vals)
    zr = np.zeros_like(cx)
    zi = np.zeros_like(cx)
    iters = np.full_like(cx, max_iters, dtype=np.int64)
    mask = np.ones_like(cx, dtype=bool)
    for i in range(max_iters):
        escaped = mask & (zr * zr + zi * zi > 4.0)
        iters[escaped] = i
        mask &= ~escaped
        if not mask.any():
            break
        new_zr = zr * zr - zi * zi + cx
        new_zi = 2.0 * zr * zi + cy
        zr[mask] = new_zr[mask]
        zi[mask] = new_zi[mask]
    return int(iters.sum())

print(compute(200, 200, 100))

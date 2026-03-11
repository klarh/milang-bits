"""Benchmark: Mandelbrot set (fixed-point) — numpy vectorized, 200x200 grid
Uses integer fixed-point arithmetic (scale=1024) to match milang version.
Note: numpy int64 // is truncation (like C), not floor division for positive
results of squaring, but 2*zr*zi can be negative. Use explicit truncation."""
import numpy as np

def trunc_div(a, b):
    """C-style truncation division for numpy arrays."""
    return np.trunc(a / b).astype(np.int64)

def compute(rows, cols, max_iters):
    cy_vals = np.arange(rows, dtype=np.int64) * 15 - 1536
    cx_vals = np.arange(cols, dtype=np.int64) * 15 - 2048
    cx, cy = np.meshgrid(cx_vals, cy_vals)
    zr = np.zeros_like(cx)
    zi = np.zeros_like(cx)
    iters = np.full_like(cx, max_iters)
    mask = np.ones_like(cx, dtype=bool)
    for i in range(max_iters):
        zr2 = trunc_div(zr * zr, 1024)
        zi2 = trunc_div(zi * zi, 1024)
        escaped = mask & (zr2 + zi2 > 4096)
        iters[escaped] = i
        mask &= ~escaped
        if not mask.any():
            break
        new_zr = zr2 - zi2 + cx
        new_zi = trunc_div(2 * zr * zi, 1024) + cy
        zr[mask] = new_zr[mask]
        zi[mask] = new_zi[mask]
    return int(iters.sum())

print(compute(200, 200, 100))

#include "array_ffi.h"
#include <stdlib.h>
#include <string.h>

/* ----- helpers ---------------------------------------------------------- */

static void ensure_cap(MiArray *a, long need) {
    if (need <= a->cap) return;
    long newcap = a->cap < 8 ? 8 : a->cap;
    while (newcap < need) newcap *= 2;
    a->data = realloc(a->data, sizeof(long) * newcap);
    a->cap = newcap;
}

/* ----- lifecycle -------------------------------------------------------- */

MiArray *mi_array_new(long cap) {
    MiArray *a = malloc(sizeof(MiArray));
    a->cap = cap < 4 ? 4 : cap;
    a->len = 0;
    a->data = malloc(sizeof(long) * a->cap);
    return a;
}

void mi_array_free(MiArray *a) {
    if (!a) return;
    free(a->data);
    free(a);
}

/* ----- access ----------------------------------------------------------- */

long mi_array_len(MiArray *a) { return a->len; }

long mi_array_get(MiArray *a, long idx) {
    if (idx < 0 || idx >= a->len) return 0;
    return a->data[idx];
}

void mi_array_set(MiArray *a, long idx, long val) {
    if (idx >= 0 && idx < a->len) a->data[idx] = val;
}

/* ----- mutating --------------------------------------------------------- */

void mi_array_push(MiArray *a, long val) {
    ensure_cap(a, a->len + 1);
    a->data[a->len++] = val;
}

long mi_array_pop(MiArray *a) {
    if (a->len == 0) return 0;
    return a->data[--a->len];
}

void mi_array_swap(MiArray *a, long i, long j) {
    if (i < 0 || i >= a->len || j < 0 || j >= a->len) return;
    long tmp = a->data[i];
    a->data[i] = a->data[j];
    a->data[j] = tmp;
}

void mi_array_reverse(MiArray *a) {
    for (long i = 0, j = a->len - 1; i < j; i++, j--) {
        long tmp = a->data[i];
        a->data[i] = a->data[j];
        a->data[j] = tmp;
    }
}

void mi_array_clear(MiArray *a) { a->len = 0; }

/* ----- slicing / copying ------------------------------------------------ */

MiArray *mi_array_slice(MiArray *a, long start, long end) {
    if (start < 0) start = 0;
    if (end > a->len) end = a->len;
    if (start >= end) return mi_array_new(0);
    long n = end - start;
    MiArray *r = mi_array_new(n);
    memcpy(r->data, &a->data[start], sizeof(long) * n);
    r->len = n;
    return r;
}

MiArray *mi_array_copy(MiArray *a) {
    return mi_array_slice(a, 0, a->len);
}

MiArray *mi_array_concat(MiArray *a, MiArray *b) {
    MiArray *r = mi_array_new(a->len + b->len);
    memcpy(r->data, a->data, sizeof(long) * a->len);
    memcpy(&r->data[a->len], b->data, sizeof(long) * b->len);
    r->len = a->len + b->len;
    return r;
}

/* ----- sorting ---------------------------------------------------------- */

static int cmp_long(const void *a, const void *b) {
    long x = *(const long *)a, y = *(const long *)b;
    return (x > y) - (x < y);
}

void mi_array_sort(MiArray *a) {
    if (a->len > 1) qsort(a->data, a->len, sizeof(long), cmp_long);
}

/* ----- search ----------------------------------------------------------- */

long mi_array_index_of(MiArray *a, long val) {
    for (long i = 0; i < a->len; i++)
        if (a->data[i] == val) return i;
    return -1;
}

long mi_array_contains(MiArray *a, long val) {
    return mi_array_index_of(a, val) >= 0 ? 1 : 0;
}

#ifndef MILANG_ARRAY_H
#define MILANG_ARRAY_H

typedef struct {
    long* data;
    long len;
    long cap;
} MiArray;

MiArray* mi_array_new(long cap);
void mi_array_free(MiArray* a);
long mi_array_len(MiArray* a);
long mi_array_get(MiArray* a, long idx);
void mi_array_set(MiArray* a, long idx, long val);
void mi_array_push(MiArray* a, long val);
long mi_array_pop(MiArray* a);
void mi_array_swap(MiArray* a, long i, long j);
void mi_array_reverse(MiArray* a);
void mi_array_clear(MiArray* a);
MiArray* mi_array_slice(MiArray* a, long start, long end);
MiArray* mi_array_copy(MiArray* a);
MiArray* mi_array_concat(MiArray* a, MiArray* b);
void mi_array_sort(MiArray* a);
long mi_array_index_of(MiArray* a, long val);
long mi_array_contains(MiArray* a, long val);

#endif

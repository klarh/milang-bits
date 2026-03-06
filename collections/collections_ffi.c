#include "collections_ffi.h"
#include <stdlib.h>
#include <string.h>

static unsigned long hash_str(const char* s) {
    unsigned long h = 5381;
    while (*s) h = h * 33 + (unsigned char)*s++;
    return h;
}

static void hm_resize(MiHashMap* m) {
    long oldcap = m->cap;
    MiHMEntry* old = m->buckets;
    m->cap = oldcap * 2;
    m->buckets = calloc(m->cap, sizeof(MiHMEntry));
    m->size = 0;
    for (long i = 0; i < oldcap; i++) {
        if (old[i].occupied) {
            mi_hm_put(m, old[i].key, old[i].val);
            free(old[i].key);
        }
    }
    free(old);
}

MiHashMap* mi_hm_new(long cap) {
    MiHashMap* m = malloc(sizeof(MiHashMap));
    m->cap = cap < 8 ? 8 : cap;
    m->size = 0;
    m->buckets = calloc(m->cap, sizeof(MiHMEntry));
    return m;
}

void mi_hm_free(MiHashMap* m) {
    if (!m) return;
    for (long i = 0; i < m->cap; i++)
        if (m->buckets[i].occupied) free(m->buckets[i].key);
    free(m->buckets);
    free(m);
}

long mi_hm_size(MiHashMap* m) { return m->size; }

void mi_hm_put(MiHashMap* m, char* key, long val) {
    if (m->size * 100 / m->cap > 70) hm_resize(m);
    unsigned long h = hash_str(key) % m->cap;
    while (m->buckets[h].occupied) {
        if (strcmp(m->buckets[h].key, key) == 0) {
            m->buckets[h].val = val;
            return;
        }
        h = (h + 1) % m->cap;
    }
    m->buckets[h].key = strdup(key);
    m->buckets[h].val = val;
    m->buckets[h].occupied = 1;
    m->size++;
}

long mi_hm_get(MiHashMap* m, char* key) {
    unsigned long h = hash_str(key) % m->cap;
    long probes = 0;
    while (m->buckets[h].occupied && probes < m->cap) {
        if (strcmp(m->buckets[h].key, key) == 0)
            return m->buckets[h].val;
        h = (h + 1) % m->cap;
        probes++;
    }
    return -1;
}

int mi_hm_has(MiHashMap* m, char* key) {
    unsigned long h = hash_str(key) % m->cap;
    long probes = 0;
    while (m->buckets[h].occupied && probes < m->cap) {
        if (strcmp(m->buckets[h].key, key) == 0) return 1;
        h = (h + 1) % m->cap;
        probes++;
    }
    return 0;
}

int mi_hm_remove(MiHashMap* m, char* key) {
    unsigned long h = hash_str(key) % m->cap;
    long probes = 0;
    while (m->buckets[h].occupied && probes < m->cap) {
        if (strcmp(m->buckets[h].key, key) == 0) {
            free(m->buckets[h].key);
            m->buckets[h].occupied = 0;
            m->size--;
            long j = (h + 1) % m->cap;
            while (m->buckets[j].occupied) {
                unsigned long k = hash_str(m->buckets[j].key) % m->cap;
                if ((j > h && (k <= h || k > j)) ||
                    (j < h && (k <= h && k > j))) {
                    m->buckets[h] = m->buckets[j];
                    m->buckets[j].occupied = 0;
                    h = j;
                }
                j = (j + 1) % m->cap;
            }
            return 1;
        }
        h = (h + 1) % m->cap;
        probes++;
    }
    return 0;
}

void mi_hm_clear(MiHashMap* m) {
    for (long i = 0; i < m->cap; i++) {
        if (m->buckets[i].occupied) {
            free(m->buckets[i].key);
            m->buckets[i].occupied = 0;
        }
    }
    m->size = 0;
}

MiDeque* mi_deque_new(long cap) {
    MiDeque* d = malloc(sizeof(MiDeque));
    d->cap = cap < 4 ? 4 : cap;
    d->data = malloc(sizeof(long) * d->cap);
    d->size = 0;
    d->head = 0;
    return d;
}

void mi_deque_free(MiDeque* d) {
    if (!d) return;
    free(d->data);
    free(d);
}

long mi_deque_size(MiDeque* d) { return d->size; }

static void deque_grow(MiDeque* d) {
    long newcap = d->cap * 2;
    long* newdata = malloc(sizeof(long) * newcap);
    for (long i = 0; i < d->size; i++)
        newdata[i] = d->data[(d->head + i) % d->cap];
    free(d->data);
    d->data = newdata;
    d->head = 0;
    d->cap = newcap;
}

void mi_deque_push_back(MiDeque* d, long val) {
    if (d->size >= d->cap) deque_grow(d);
    d->data[(d->head + d->size) % d->cap] = val;
    d->size++;
}

void mi_deque_push_front(MiDeque* d, long val) {
    if (d->size >= d->cap) deque_grow(d);
    d->head = (d->head - 1 + d->cap) % d->cap;
    d->data[d->head] = val;
    d->size++;
}

long mi_deque_pop_back(MiDeque* d) {
    if (d->size == 0) return 0;
    d->size--;
    return d->data[(d->head + d->size) % d->cap];
}

long mi_deque_pop_front(MiDeque* d) {
    if (d->size == 0) return 0;
    long val = d->data[d->head];
    d->head = (d->head + 1) % d->cap;
    d->size--;
    return val;
}

long mi_deque_peek_front(MiDeque* d) {
    if (d->size == 0) return 0;
    return d->data[d->head];
}

long mi_deque_peek_back(MiDeque* d) {
    if (d->size == 0) return 0;
    return d->data[(d->head + d->size - 1) % d->cap];
}

long mi_deque_get(MiDeque* d, long idx) {
    if (idx < 0 || idx >= d->size) return 0;
    return d->data[(d->head + idx) % d->cap];
}

void mi_deque_clear(MiDeque* d) {
    d->size = 0;
    d->head = 0;
}

static void heap_sift_up(MiHeap* h, long i) {
    while (i > 0) {
        long p = (i - 1) / 2;
        if (h->data[i] < h->data[p]) {
            long tmp = h->data[i]; h->data[i] = h->data[p]; h->data[p] = tmp;
            i = p;
        } else break;
    }
}

static void heap_sift_down(MiHeap* h, long i) {
    while (1) {
        long smallest = i;
        long l = 2 * i + 1, r = 2 * i + 2;
        if (l < h->size && h->data[l] < h->data[smallest]) smallest = l;
        if (r < h->size && h->data[r] < h->data[smallest]) smallest = r;
        if (smallest == i) break;
        long tmp = h->data[i]; h->data[i] = h->data[smallest]; h->data[smallest] = tmp;
        i = smallest;
    }
}

MiHeap* mi_heap_new(long cap) {
    MiHeap* h = malloc(sizeof(MiHeap));
    h->cap = cap < 4 ? 4 : cap;
    h->data = malloc(sizeof(long) * h->cap);
    h->size = 0;
    return h;
}

void mi_heap_free(MiHeap* h) {
    if (!h) return;
    free(h->data);
    free(h);
}

long mi_heap_size(MiHeap* h) { return h->size; }

void mi_heap_push(MiHeap* h, long val) {
    if (h->size >= h->cap) {
        h->cap *= 2;
        h->data = realloc(h->data, sizeof(long) * h->cap);
    }
    h->data[h->size] = val;
    heap_sift_up(h, h->size);
    h->size++;
}

long mi_heap_pop(MiHeap* h) {
    if (h->size == 0) return 0;
    long val = h->data[0];
    h->size--;
    if (h->size > 0) {
        h->data[0] = h->data[h->size];
        heap_sift_down(h, 0);
    }
    return val;
}

long mi_heap_peek(MiHeap* h) {
    if (h->size == 0) return 0;
    return h->data[0];
}

MiIntSet* mi_iset_new(long cap) {
    MiIntSet* s = malloc(sizeof(MiIntSet));
    s->cap = cap < 8 ? 8 : cap;
    s->data = calloc(s->cap, sizeof(long));
    s->size = 0;
    return s;
}

void mi_iset_free(MiIntSet* s) {
    if (!s) return;
    free(s->data);
    free(s);
}

long mi_iset_size(MiIntSet* s) { return s->size; }

static long iset_find(MiIntSet* s, long val) {
    unsigned long h = (unsigned long)val * 2654435761UL % s->cap;
    long probes = 0;
    while (probes < s->cap) {
        if (s->data[h] == 0 && probes > 0) return -1;
        if (s->data[h] == val) return h;
        if (s->data[h] == 0) return -1;
        h = (h + 1) % s->cap;
        probes++;
    }
    return -1;
}

static void iset_resize(MiIntSet* s) {
    long oldcap = s->cap;
    long* old = s->data;
    s->cap = oldcap * 2;
    s->data = calloc(s->cap, sizeof(long));
    s->size = 0;
    for (long i = 0; i < oldcap; i++)
        if (old[i] != 0) mi_iset_add(s, old[i]);
    free(old);
}

int mi_iset_add(MiIntSet* s, long val) {
    if (val == 0) return 0;
    if (s->size * 100 / s->cap > 70) iset_resize(s);
    unsigned long h = (unsigned long)val * 2654435761UL % s->cap;
    while (s->data[h] != 0) {
        if (s->data[h] == val) return 0;
        h = (h + 1) % s->cap;
    }
    s->data[h] = val;
    s->size++;
    return 1;
}

int mi_iset_has(MiIntSet* s, long val) {
    if (val == 0) return 0;
    return iset_find(s, val) >= 0 ? 1 : 0;
}

int mi_iset_remove(MiIntSet* s, long val) {
    if (val == 0) return 0;
    long idx = iset_find(s, val);
    if (idx < 0) return 0;
    s->data[idx] = 0;
    s->size--;
    long j = (idx + 1) % s->cap;
    while (s->data[j] != 0) {
        long v = s->data[j];
        s->data[j] = 0;
        s->size--;
        mi_iset_add(s, v);
        j = (j + 1) % s->cap;
    }
    return 1;
}

void mi_iset_clear(MiIntSet* s) {
    memset(s->data, 0, sizeof(long) * s->cap);
    s->size = 0;
}

long mi_iset_elem_at(MiIntSet* s, long idx) {
    long count = 0;
    for (long i = 0; i < s->cap; i++) {
        if (s->data[i] != 0) {
            if (count == idx) return s->data[i];
            count++;
        }
    }
    return 0;
}

char* mi_hm_key_at(MiHashMap* m, long idx) {
    long count = 0;
    for (long i = 0; i < m->cap; i++) {
        if (m->buckets[i].occupied) {
            if (count == idx) return m->buckets[i].key;
            count++;
        }
    }
    return "";
}

long mi_hm_val_at(MiHashMap* m, long idx) {
    long count = 0;
    for (long i = 0; i < m->cap; i++) {
        if (m->buckets[i].occupied) {
            if (count == idx) return m->buckets[i].val;
            count++;
        }
    }
    return 0;
}

long mi_deque_contains(MiDeque* d, long val) {
    for (long i = 0; i < d->size; i++)
        if (d->data[(d->head + i) % d->cap] == val) return 1;
    return 0;
}

void mi_deque_reverse(MiDeque* d) {
    for (long i = 0; i < d->size / 2; i++) {
        long a = (d->head + i) % d->cap;
        long b = (d->head + d->size - 1 - i) % d->cap;
        long tmp = d->data[a];
        d->data[a] = d->data[b];
        d->data[b] = tmp;
    }
}

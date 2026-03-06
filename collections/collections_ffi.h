#ifndef MI_COLLECTIONS_H
#define MI_COLLECTIONS_H

typedef struct MiHMEntry {
    char* key;
    long val;
    int occupied;
} MiHMEntry;

typedef struct {
    MiHMEntry* buckets;
    long cap;
    long size;
} MiHashMap;

MiHashMap* mi_hm_new(long cap);
void mi_hm_free(MiHashMap* m);
long mi_hm_size(MiHashMap* m);
void mi_hm_put(MiHashMap* m, char* key, long val);
long mi_hm_get(MiHashMap* m, char* key);
int mi_hm_has(MiHashMap* m, char* key);
int mi_hm_remove(MiHashMap* m, char* key);
void mi_hm_clear(MiHashMap* m);

typedef struct {
    long* data;
    long cap;
    long size;
    long head;
} MiDeque;

MiDeque* mi_deque_new(long cap);
void mi_deque_free(MiDeque* d);
long mi_deque_size(MiDeque* d);
void mi_deque_push_back(MiDeque* d, long val);
void mi_deque_push_front(MiDeque* d, long val);
long mi_deque_pop_back(MiDeque* d);
long mi_deque_pop_front(MiDeque* d);
long mi_deque_peek_front(MiDeque* d);
long mi_deque_peek_back(MiDeque* d);
long mi_deque_get(MiDeque* d, long idx);
void mi_deque_clear(MiDeque* d);

typedef struct {
    long* data;
    long cap;
    long size;
} MiHeap;

MiHeap* mi_heap_new(long cap);
void mi_heap_free(MiHeap* h);
long mi_heap_size(MiHeap* h);
void mi_heap_push(MiHeap* h, long val);
long mi_heap_pop(MiHeap* h);
long mi_heap_peek(MiHeap* h);

typedef struct {
    long* data;
    long cap;
    long size;
} MiIntSet;

MiIntSet* mi_iset_new(long cap);
void mi_iset_free(MiIntSet* s);
long mi_iset_size(MiIntSet* s);
int mi_iset_add(MiIntSet* s, long val);
int mi_iset_has(MiIntSet* s, long val);
int mi_iset_remove(MiIntSet* s, long val);
void mi_iset_clear(MiIntSet* s);
long mi_iset_elem_at(MiIntSet* s, long idx);

char* mi_hm_key_at(MiHashMap* m, long idx);
long mi_hm_val_at(MiHashMap* m, long idx);
long mi_deque_contains(MiDeque* d, long val);
void mi_deque_reverse(MiDeque* d);

#endif

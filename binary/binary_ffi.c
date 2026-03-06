#include "binary_ffi.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void buf_ensure(MiBuffer* b, long need) {
    long req = b->len + need;
    if (req <= b->cap) return;
    long newcap = b->cap < 16 ? 16 : b->cap;
    while (newcap < req) newcap *= 2;
    b->data = realloc(b->data, newcap);
    b->cap = newcap;
}

MiBuffer* mi_buf_new(long cap) {
    MiBuffer* b = malloc(sizeof(MiBuffer));
    b->cap = cap < 16 ? 16 : cap;
    b->data = calloc(b->cap, 1);
    b->len = 0;
    b->pos = 0;
    return b;
}

void mi_buf_free(MiBuffer* b) {
    if (!b) return;
    free(b->data);
    free(b);
}

long mi_buf_len(MiBuffer* b) { return b->len; }
long mi_buf_pos(MiBuffer* b) { return b->pos; }

void mi_buf_seek(MiBuffer* b, long pos) {
    if (pos < 0) pos = 0;
    if (pos > b->len) pos = b->len;
    b->pos = pos;
}

void mi_buf_reset(MiBuffer* b) { b->pos = 0; }

void mi_buf_write_u8(MiBuffer* b, long val) {
    buf_ensure(b, 1);
    b->data[b->len++] = (unsigned char)(val & 0xFF);
}

void mi_buf_write_i8(MiBuffer* b, long val) {
    mi_buf_write_u8(b, val);
}

void mi_buf_write_u16be(MiBuffer* b, long val) {
    buf_ensure(b, 2);
    b->data[b->len++] = (unsigned char)((val >> 8) & 0xFF);
    b->data[b->len++] = (unsigned char)(val & 0xFF);
}

void mi_buf_write_u16le(MiBuffer* b, long val) {
    buf_ensure(b, 2);
    b->data[b->len++] = (unsigned char)(val & 0xFF);
    b->data[b->len++] = (unsigned char)((val >> 8) & 0xFF);
}

void mi_buf_write_i16be(MiBuffer* b, long val) { mi_buf_write_u16be(b, val); }
void mi_buf_write_i16le(MiBuffer* b, long val) { mi_buf_write_u16le(b, val); }

void mi_buf_write_u32be(MiBuffer* b, long val) {
    buf_ensure(b, 4);
    b->data[b->len++] = (unsigned char)((val >> 24) & 0xFF);
    b->data[b->len++] = (unsigned char)((val >> 16) & 0xFF);
    b->data[b->len++] = (unsigned char)((val >> 8) & 0xFF);
    b->data[b->len++] = (unsigned char)(val & 0xFF);
}

void mi_buf_write_u32le(MiBuffer* b, long val) {
    buf_ensure(b, 4);
    b->data[b->len++] = (unsigned char)(val & 0xFF);
    b->data[b->len++] = (unsigned char)((val >> 8) & 0xFF);
    b->data[b->len++] = (unsigned char)((val >> 16) & 0xFF);
    b->data[b->len++] = (unsigned char)((val >> 24) & 0xFF);
}

void mi_buf_write_i32be(MiBuffer* b, long val) { mi_buf_write_u32be(b, val); }
void mi_buf_write_i32le(MiBuffer* b, long val) { mi_buf_write_u32le(b, val); }

void mi_buf_write_u64be(MiBuffer* b, long val) {
    buf_ensure(b, 8);
    for (int i = 7; i >= 0; i--)
        b->data[b->len++] = (unsigned char)((val >> (i * 8)) & 0xFF);
}

void mi_buf_write_u64le(MiBuffer* b, long val) {
    buf_ensure(b, 8);
    for (int i = 0; i < 8; i++)
        b->data[b->len++] = (unsigned char)((val >> (i * 8)) & 0xFF);
}

void mi_buf_write_bytes(MiBuffer* b, char* data, long n) {
    buf_ensure(b, n);
    memcpy(&b->data[b->len], data, n);
    b->len += n;
}

void mi_buf_write_cstring(MiBuffer* b, char* s) {
    long n = strlen(s);
    buf_ensure(b, n + 1);
    memcpy(&b->data[b->len], s, n + 1);
    b->len += n + 1;
}

long mi_buf_read_u8(MiBuffer* b) {
    if (b->pos >= b->len) return 0;
    return b->data[b->pos++];
}

long mi_buf_read_i8(MiBuffer* b) {
    long v = mi_buf_read_u8(b);
    return (v > 127) ? v - 256 : v;
}

long mi_buf_read_u16be(MiBuffer* b) {
    if (b->pos + 2 > b->len) return 0;
    long v = ((long)b->data[b->pos] << 8) | b->data[b->pos + 1];
    b->pos += 2;
    return v;
}

long mi_buf_read_u16le(MiBuffer* b) {
    if (b->pos + 2 > b->len) return 0;
    long v = b->data[b->pos] | ((long)b->data[b->pos + 1] << 8);
    b->pos += 2;
    return v;
}

long mi_buf_read_i16be(MiBuffer* b) {
    long v = mi_buf_read_u16be(b);
    return (v > 32767) ? v - 65536 : v;
}

long mi_buf_read_i16le(MiBuffer* b) {
    long v = mi_buf_read_u16le(b);
    return (v > 32767) ? v - 65536 : v;
}

long mi_buf_read_u32be(MiBuffer* b) {
    if (b->pos + 4 > b->len) return 0;
    long v = ((long)b->data[b->pos] << 24) | ((long)b->data[b->pos+1] << 16) |
             ((long)b->data[b->pos+2] << 8) | b->data[b->pos+3];
    b->pos += 4;
    return v;
}

long mi_buf_read_u32le(MiBuffer* b) {
    if (b->pos + 4 > b->len) return 0;
    long v = b->data[b->pos] | ((long)b->data[b->pos+1] << 8) |
             ((long)b->data[b->pos+2] << 16) | ((long)b->data[b->pos+3] << 24);
    b->pos += 4;
    return v;
}

long mi_buf_read_i32be(MiBuffer* b) {
    long v = mi_buf_read_u32be(b);
    return (v > 0x7FFFFFFF) ? v - 0x100000000L : v;
}

long mi_buf_read_i32le(MiBuffer* b) {
    long v = mi_buf_read_u32le(b);
    return (v > 0x7FFFFFFF) ? v - 0x100000000L : v;
}

long mi_buf_read_u64be(MiBuffer* b) {
    if (b->pos + 8 > b->len) return 0;
    long v = 0;
    for (int i = 7; i >= 0; i--)
        v |= ((long)b->data[b->pos++] << (i * 8));
    return v;
}

long mi_buf_read_u64le(MiBuffer* b) {
    if (b->pos + 8 > b->len) return 0;
    long v = 0;
    for (int i = 0; i < 8; i++)
        v |= ((long)b->data[b->pos++] << (i * 8));
    return v;
}

char* mi_buf_read_cstring(MiBuffer* b) {
    if (b->pos >= b->len) return strdup("");
    char* start = (char*)&b->data[b->pos];
    long slen = strlen(start);
    b->pos += slen + 1;
    if (b->pos > b->len) b->pos = b->len;
    return strdup(start);
}

char* mi_buf_read_bytes_str(MiBuffer* b, long n) {
    if (b->pos + n > b->len) n = b->len - b->pos;
    if (n <= 0) return strdup("");
    char* s = malloc(n + 1);
    memcpy(s, &b->data[b->pos], n);
    s[n] = 0;
    b->pos += n;
    return s;
}

long mi_buf_remaining(MiBuffer* b) {
    return b->len - b->pos;
}

static const char hex_chars[] = "0123456789abcdef";

char* mi_buf_to_hex(MiBuffer* b) {
    char* s = malloc(b->len * 2 + 1);
    for (long i = 0; i < b->len; i++) {
        s[i*2] = hex_chars[(b->data[i] >> 4) & 0xF];
        s[i*2+1] = hex_chars[b->data[i] & 0xF];
    }
    s[b->len * 2] = 0;
    return s;
}

MiBuffer* mi_buf_slice(MiBuffer* b, long start, long end) {
    if (start < 0) start = 0;
    if (end > b->len) end = b->len;
    if (start >= end) return mi_buf_new(0);
    long n = end - start;
    MiBuffer* r = mi_buf_new(n);
    memcpy(r->data, &b->data[start], n);
    r->len = n;
    return r;
}

static int hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

MiBuffer* mi_buf_from_hex(char* hex) {
    long slen = strlen(hex);
    long nbytes = slen / 2;
    MiBuffer* b = mi_buf_new(nbytes);
    for (long i = 0; i < nbytes; i++) {
        unsigned char val = (hex_digit(hex[2*i]) << 4) | hex_digit(hex[2*i+1]);
        mi_buf_write_u8(b, val);
    }
    return b;
}

long mi_buf_equals(MiBuffer* a, MiBuffer* b) {
    if (a->len != b->len) return 0;
    return memcmp(a->data, b->data, a->len) == 0 ? 1 : 0;
}

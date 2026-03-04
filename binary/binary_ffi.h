#ifndef MI_BINARY_H
#define MI_BINARY_H

typedef struct {
    unsigned char* data;
    long cap;
    long len;
    long pos;
} MiBuffer;

MiBuffer* mi_buf_new(long cap);
void mi_buf_free(MiBuffer* b);
long mi_buf_len(MiBuffer* b);
long mi_buf_pos(MiBuffer* b);
void mi_buf_seek(MiBuffer* b, long pos);
void mi_buf_reset(MiBuffer* b);

void mi_buf_write_u8(MiBuffer* b, long val);
void mi_buf_write_i8(MiBuffer* b, long val);
void mi_buf_write_u16be(MiBuffer* b, long val);
void mi_buf_write_u16le(MiBuffer* b, long val);
void mi_buf_write_i16be(MiBuffer* b, long val);
void mi_buf_write_i16le(MiBuffer* b, long val);
void mi_buf_write_u32be(MiBuffer* b, long val);
void mi_buf_write_u32le(MiBuffer* b, long val);
void mi_buf_write_i32be(MiBuffer* b, long val);
void mi_buf_write_i32le(MiBuffer* b, long val);
void mi_buf_write_u64be(MiBuffer* b, long val);
void mi_buf_write_u64le(MiBuffer* b, long val);
void mi_buf_write_bytes(MiBuffer* b, char* data, long n);
void mi_buf_write_cstring(MiBuffer* b, char* s);

long mi_buf_read_u8(MiBuffer* b);
long mi_buf_read_i8(MiBuffer* b);
long mi_buf_read_u16be(MiBuffer* b);
long mi_buf_read_u16le(MiBuffer* b);
long mi_buf_read_i16be(MiBuffer* b);
long mi_buf_read_i16le(MiBuffer* b);
long mi_buf_read_u32be(MiBuffer* b);
long mi_buf_read_u32le(MiBuffer* b);
long mi_buf_read_i32be(MiBuffer* b);
long mi_buf_read_i32le(MiBuffer* b);
long mi_buf_read_u64be(MiBuffer* b);
long mi_buf_read_u64le(MiBuffer* b);
char* mi_buf_read_cstring(MiBuffer* b);
char* mi_buf_read_bytes_str(MiBuffer* b, long n);

long mi_buf_remaining(MiBuffer* b);
char* mi_buf_to_hex(MiBuffer* b);
MiBuffer* mi_buf_slice(MiBuffer* b, long start, long end);

#endif

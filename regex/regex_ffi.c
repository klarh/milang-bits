#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include "regex_ffi.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct MiRegex {
    pcre2_code* code;
};

MiRegex* mi_re_compile(char* pattern) {
    int errorcode;
    PCRE2_SIZE erroroffset;
    pcre2_code* code = pcre2_compile(
        (PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
        PCRE2_UTF, &errorcode, &erroroffset, NULL);
    if (!code) return NULL;
    MiRegex* re = malloc(sizeof(MiRegex));
    re->code = code;
    return re;
}

void mi_re_free(MiRegex* re) {
    if (!re) return;
    pcre2_code_free(re->code);
    free(re);
}

int mi_re_test(MiRegex* re, char* subject) {
    if (!re) return 0;
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);
    int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                         PCRE2_ZERO_TERMINATED, 0, 0, md, NULL);
    pcre2_match_data_free(md);
    return rc >= 0 ? 1 : 0;
}

char* mi_re_match(MiRegex* re, char* subject) {
    if (!re) return strdup("");
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);
    int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                         PCRE2_ZERO_TERMINATED, 0, 0, md, NULL);
    if (rc < 0) {
        pcre2_match_data_free(md);
        return strdup("");
    }
    PCRE2_SIZE* ov = pcre2_get_ovector_pointer(md);
    long len = ov[1] - ov[0];
    char* s = malloc(len + 1);
    memcpy(s, subject + ov[0], len);
    s[len] = 0;
    pcre2_match_data_free(md);
    return s;
}

long mi_re_match_start(MiRegex* re, char* subject) {
    if (!re) return -1;
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);
    int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                         PCRE2_ZERO_TERMINATED, 0, 0, md, NULL);
    long result = rc >= 0 ? (long)pcre2_get_ovector_pointer(md)[0] : -1;
    pcre2_match_data_free(md);
    return result;
}

long mi_re_match_len(MiRegex* re, char* subject) {
    if (!re) return 0;
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);
    int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                         PCRE2_ZERO_TERMINATED, 0, 0, md, NULL);
    long result = 0;
    if (rc >= 0) {
        PCRE2_SIZE* ov = pcre2_get_ovector_pointer(md);
        result = ov[1] - ov[0];
    }
    pcre2_match_data_free(md);
    return result;
}

char* mi_re_capture(MiRegex* re, char* subject, long group) {
    if (!re) return strdup("");
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);
    int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                         PCRE2_ZERO_TERMINATED, 0, 0, md, NULL);
    if (rc < 0 || group >= (long)rc) {
        pcre2_match_data_free(md);
        return strdup("");
    }
    PCRE2_SIZE* ov = pcre2_get_ovector_pointer(md);
    long start = ov[group * 2];
    long end = ov[group * 2 + 1];
    long len = end - start;
    char* s = malloc(len + 1);
    memcpy(s, subject + start, len);
    s[len] = 0;
    pcre2_match_data_free(md);
    return s;
}

long mi_re_capture_count(MiRegex* re, char* subject) {
    if (!re) return 0;
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);
    int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                         PCRE2_ZERO_TERMINATED, 0, 0, md, NULL);
    pcre2_match_data_free(md);
    return rc >= 0 ? rc : 0;
}

static char* str_replace_first(const char* subject, long start, long end,
                                const char* replacement) {
    long slen = strlen(subject);
    long rlen = strlen(replacement);
    long newlen = slen - (end - start) + rlen;
    char* s = malloc(newlen + 1);
    memcpy(s, subject, start);
    memcpy(s + start, replacement, rlen);
    memcpy(s + start + rlen, subject + end, slen - end);
    s[newlen] = 0;
    return s;
}

char* mi_re_replace(MiRegex* re, char* subject, char* replacement) {
    if (!re) return strdup(subject);
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);
    int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                         PCRE2_ZERO_TERMINATED, 0, 0, md, NULL);
    if (rc < 0) {
        pcre2_match_data_free(md);
        return strdup(subject);
    }
    PCRE2_SIZE* ov = pcre2_get_ovector_pointer(md);
    char* result = str_replace_first(subject, ov[0], ov[1], replacement);
    pcre2_match_data_free(md);
    return result;
}

char* mi_re_replace_all(MiRegex* re, char* subject, char* replacement) {
    if (!re) return strdup(subject);
    long slen = strlen(subject);
    long rlen = strlen(replacement);
    long cap = slen * 2 + 64;
    char* result = malloc(cap);
    long rpos = 0;
    PCRE2_SIZE offset = 0;
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);

    while (offset <= (PCRE2_SIZE)slen) {
        int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                             slen, offset, 0, md, NULL);
        if (rc < 0) break;
        PCRE2_SIZE* ov = pcre2_get_ovector_pointer(md);
        long before = ov[0] - offset;
        long need = rpos + before + rlen + (slen - ov[1]) + 1;
        if (need > cap) {
            while (cap < need) cap *= 2;
            result = realloc(result, cap);
        }
        memcpy(result + rpos, subject + offset, before);
        rpos += before;
        memcpy(result + rpos, replacement, rlen);
        rpos += rlen;
        offset = ov[1];
        if (ov[0] == ov[1]) offset++;
    }
    long remaining = slen - offset;
    if (remaining > 0) {
        long need = rpos + remaining + 1;
        if (need > cap) result = realloc(result, need);
        memcpy(result + rpos, subject + offset, remaining);
        rpos += remaining;
    }
    result[rpos] = 0;
    pcre2_match_data_free(md);
    return result;
}

char* mi_re_find_all(MiRegex* re, char* subject) {
    if (!re) return strdup("");
    long slen = strlen(subject);
    long cap = 256;
    char* result = malloc(cap);
    long rpos = 0;
    PCRE2_SIZE offset = 0;
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);

    while (offset <= (PCRE2_SIZE)slen) {
        int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                             slen, offset, 0, md, NULL);
        if (rc < 0) break;
        PCRE2_SIZE* ov = pcre2_get_ovector_pointer(md);
        long mlen = ov[1] - ov[0];
        if (rpos > 0) {
            if (rpos + 1 >= cap) { cap *= 2; result = realloc(result, cap); }
            result[rpos++] = '|';
        }
        if (rpos + mlen >= cap) {
            while (rpos + mlen >= cap) cap *= 2;
            result = realloc(result, cap);
        }
        memcpy(result + rpos, subject + ov[0], mlen);
        rpos += mlen;
        offset = ov[1];
        if (ov[0] == ov[1]) offset++;
    }
    result[rpos] = 0;
    pcre2_match_data_free(md);
    return result;
}

char* mi_re_split(MiRegex* re, char* subject) {
    if (!re) return strdup(subject);
    long slen = strlen(subject);
    long cap = slen + 64;
    char* result = malloc(cap);
    long rpos = 0;
    PCRE2_SIZE offset = 0;
    pcre2_match_data* md = pcre2_match_data_create_from_pattern(re->code, NULL);

    while (offset <= (PCRE2_SIZE)slen) {
        int rc = pcre2_match(re->code, (PCRE2_SPTR)subject,
                             slen, offset, 0, md, NULL);
        if (rc < 0) break;
        PCRE2_SIZE* ov = pcre2_get_ovector_pointer(md);
        long before = ov[0] - offset;
        if (rpos > 0) {
            if (rpos + 1 >= cap) { cap *= 2; result = realloc(result, cap); }
            result[rpos++] = '|';
        }
        if (rpos + before >= cap) {
            while (rpos + before >= cap) cap *= 2;
            result = realloc(result, cap);
        }
        memcpy(result + rpos, subject + offset, before);
        rpos += before;
        offset = ov[1];
        if (ov[0] == ov[1]) offset++;
    }
    long remaining = slen - offset;
    if (remaining > 0 || rpos > 0) {
        if (rpos > 0) {
            if (rpos + 1 >= cap) { cap *= 2; result = realloc(result, cap); }
            result[rpos++] = '|';
        }
        if (rpos + remaining >= cap) {
            while (rpos + remaining >= cap) cap *= 2;
            result = realloc(result, cap);
        }
        memcpy(result + rpos, subject + offset, remaining);
        rpos += remaining;
    }
    result[rpos] = 0;
    pcre2_match_data_free(md);
    return result;
}

int mi_re_is_valid(char* pattern) {
    int errorcode;
    PCRE2_SIZE erroroffset;
    pcre2_code* code = pcre2_compile(
        (PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
        PCRE2_UTF, &errorcode, &erroroffset, NULL);
    if (!code) return 0;
    pcre2_code_free(code);
    return 1;
}

#ifndef AOC_SVIEW_H
#define AOC_SVIEW_H


#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>
#define SVIEW_NPOS ((size_t) (-1))

typedef int(*predicate)(int);

#define sv_create_from_literal(lit) sv_create_with_len(lit, sizeof(lit) - 1)
struct sview {
    const char* data;
    size_t len;
};


struct sview sv_create(const char* cstr);
struct sview sv_create_with_len(const char* cstr, size_t len);

int sv_empty(const struct sview s);

struct sview sv_subview(struct sview s, size_t start_idx, size_t len);

int64_t sv_split(struct sview s, char delim, struct sview* left, struct sview* right);

int sv_cmp(struct sview a, struct sview b);

int64_t sv_find(const struct sview s, predicate p);
int64_t sv_find_char(const struct sview s, const char c);

struct sview sv_trim_left(struct sview s);

uint64_t sv_parse_u64(const struct sview v);
int64_t sv_parse_i64(struct sview v);


int sv_starts_with(const struct sview a, const struct sview b);

#ifdef AOC_SVIEW_IMPLEMENTATION

int64_t sv_find_char(const struct sview s, const char c) {
    size_t i;
    for(i = 0; i < s.len; ++i)
        if(s.data[i] == c)
            return i;
    return -1;
}
int64_t sv_find(const struct sview s, predicate p) {
    size_t i;
    for(i = 0; i < s.len; ++i)
        if(p(s.data[i]))
            return i;
    return -1;
}

struct sview sv_create(const char* cstr) {
    struct sview s;
    s.data = cstr;
    s.len = strlen(cstr);
    return s;
}

struct sview sv_create_with_len(const char* cstr, size_t len) {
    struct sview s;
    s.data = cstr;
    s.len = len;

    return s;
}

int sv_empty(const struct sview s) {
    return s.len == 0;
}

struct sview sv_subview(struct sview s, size_t start_idx, size_t len) {
    struct sview sub;
    if(len == SVIEW_NPOS) {
        len = s.len - start_idx;
    }
    sub.data = s.data + start_idx;
    sub.len = len;
    return sub;
}

int64_t sv_split(struct sview s, char delim, struct sview* left, struct sview* right) {
    size_t i;

    for(i = 0; i < s.len; ++i) {
        if(s.data[i] == delim) {
            if(right) {
                right->data = s.data + i + 1;
                right->len = s.len - i - 1;
            }

            if(left) {
                left->data = s.data;
                left->len = i;
            }
            return (int64_t) i;
        }
    }

    if(left) {
        *left = s;
    }

    if(right) {
        right->data = NULL;
        right->len = 0;
    }


    return -1;
}

int sv_cmp(struct sview a, struct sview b) {
    int diff;
    size_t i;
    i = 0;
    while(i < a.len && i < b.len)  {
        diff = a.data[i] - b.data[i];
        if(diff != 0)
            return diff;

        ++i;
    }

    if(a.len < b.len) {
        return -b.data[i];
    }else if(a.len > b.len) {
        return a.data[i];
    }
    return 0;
}

struct sview sv_trim_left(struct sview s) {
    size_t off;
    for(off = 0; off < s.len; ++off) {
        if(!isspace(s.data[off])) {
            break;
        }
    }

    s.data += off;
    s.len -= off;
    return s;
}

uint64_t sv_parse_u64(const struct sview v) {
    if(v.len == 0) {
        errno = EINVAL;
        return 0;
    }
    uint64_t res = 0;
    for(size_t i = 0; i < v.len && isdigit(v.data[i]); ++i) {
        res = res * 10 + v.data[i] - '0';
    }
    return res;
}

int64_t sv_parse_i64(struct sview v) {
    if(v.len == 0) {
        errno = EINVAL;
        return 0;
    }
    int64_t fac = 1;
    if(v.data[0] == '-') {
        fac = -1;
        v = sv_subview(v, 1, SVIEW_NPOS);
    }

    const uint64_t res = sv_parse_u64(v); 
    if(res & (1ul << 63)) {
        errno = ERANGE;
        return 0;
    }
    return fac * res;
}

int sv_starts_with(const struct sview a, const struct sview b) {
    if(b.len > a.len)
        return 0;

    for(size_t i = 0; i < b.len; ++i) {
        if(b.data[i] != a.data[i])
            return 0;
    }
    return 1;
}
#endif


#endif 

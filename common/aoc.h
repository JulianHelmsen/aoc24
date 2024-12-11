#ifdef AOC_IMPLEMENTATION
#define AOC_HASHMAP_IMPLEMENTATION
#define AOC_SVIEW_IMPLEMENTATION
#define AOC_DA_IMPLEMENTATION
#endif
#include <aoc_da.h>
#include <aoc_hashmap.h>
#include <aoc_sview.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

struct Ints {
    size_t capacity;
    size_t size;
    int* data;
};


struct Ints parse_ints(struct sview list, const char delim);

char* sv_copy(struct sview s);


#ifdef AOC_IMPLEMENTATION

struct Ints parse_ints(struct sview list, const char delim) {
    struct Ints ints = { 0 };

    while(!sv_empty(list)) {
        struct sview num;
        sv_split(list, delim, &num, &list);

        const size_t old_size = ints.size;
        da_append(ints, 0);
        assert(old_size + 1 == ints.size);

        for(size_t i = num.len - 1; i < num.len; i--) {
            const char c = num.data[i];
            if(!isdigit(c)) {
                fprintf(stderr, "Can not convert '%.*s' to an integer as the character at index %lu ('%c') is not a digit.\n", (int) num.len, num.data, i, c);
                exit(1);
            }
            ints.data[ints.size - 1] = ints.data[ints.size - 1] * 10 + c - '0';
        }
    }
    return ints;
}

char* sv_copy(struct sview s) {
    char* data = malloc(sizeof(char) * (s.len + 1));
    if(data == NULL) {
        perror("malloc");
        exit(1);
    }
    memcpy(data, s.data, s.len * sizeof(char));
    data[s.len] = '\0';
    return data;
}

#endif



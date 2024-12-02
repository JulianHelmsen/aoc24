#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define AOC_SVIEW_IMPLEMENTATION
#include <aoc_sview.h>
#include <aoc_da.h>

struct Items {
    size_t capacity;
    size_t size;
    long* data;
};

int compare_int(const void* a, const void* b) {
    const long* pa = (const long*) a;
    const long* pb = (const long*) b;
    return *pa - *pb;
}

int main(const int argc, const char** argv) {
    if(argc <= 1) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    FILE* f = fopen(argv[1], "r");
    if(f == NULL) {
        perror("fopen");
        exit(1);
    }

    char* line = NULL;
    size_t capacity = 0;
    ssize_t len = 0;

    struct Items vleft = { 0 };
    struct Items vright = { 0 };
    while((len = getline(&line, &capacity, f)) != -1) {
        if(line[len - 1] == '\n') line[len - 1] = '\0';
        struct sview data = sv_create(line);
        if(data.len == 0)
            continue;

        struct sview left, right;
        if(sv_split(data, ' ', &left, &right) == -1) {
            fprintf(stderr, "failed to parse line: %s\n", line);
            continue;
        }
       
        left = sv_trim_left(left);
        right = sv_trim_left(right);
        const long lval = strtol(left.data, NULL, 10);
        if(errno == ERANGE) {
            perror("strtol");
            exit(1);
        }
        const long rval = strtol(right.data, NULL, 10);
        if(errno == ERANGE) {
            perror("strtol");
            exit(1);
        }
        da_append(vleft, lval);
        da_append(vright, rval);
    }
    if(ferror(f)) {
        fprintf(stderr, "failed to read file: %s\n", strerror(errno));
    }

    qsort(vleft.data, vleft.size, sizeof(vleft.data[0]), compare_int);
    qsort(vright.data, vright.size, sizeof(vright.data[0]), compare_int);
    long diff = 0;
    for(size_t i = 0; i < vright.size; ++i) {
        long d = vleft.data[i] - vright.data[i];
        if(d < 0)
            d = -d;
        diff += d;
        // printf("%ld, %ld\n", vleft.data[i], vright.data[i]);
    }

    printf("answer: %ld\n", diff);
    long answer2 = 0;
    size_t ridx = 0;
    for(size_t i = 0; i < vleft.size; ++i) {
        const long val = vleft.data[i];
        while(ridx < vright.size && vright.data[ridx] < val)
            ++ridx;

        size_t r = ridx;
        while(r < vright.size && vright.data[r] == val) {  
            answer2 += val;
            ++r;
        }
    }
    printf("Answer 2: %ld\n", answer2);

    fclose(f);
    return 0;
}

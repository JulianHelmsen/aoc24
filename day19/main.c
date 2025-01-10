#define AOC_IMPLEMENTATION
#include <aoc.h>

struct string {
    char* data;
    size_t capacity;
    size_t size;
};

struct string_list {
    struct string* data;
    size_t size;
    size_t capacity;
};

struct string string_from_sv(struct sview sv) {
    struct string s;
    s.size = sv.len;
    s.data = malloc(sv.len);
    if(s.data == NULL) {
        perror("malloc");
        exit(1);
    }
    memcpy(s.data, sv.data, sv.len);
    return s;
}

void string_append(struct string* this, struct string other) {
    for(size_t i = 0; i < other.size; ++i) {
        da_append(*this, other.data[i]);
    }
}

uint64_t arrangement_count(struct sview l, const struct string* towels, const size_t ntowels) {
    uint64_t* arrangment_count = calloc(l.len + 1, sizeof(uint64_t));
    if(arrangment_count == NULL) {
        perror("calloc");
        exit(1);
    }
    arrangment_count[l.len] = 1;


    for(ssize_t i = l.len - 1; i >= 0; --i) {
        // determine whether it's possible to reach any possible position from this position
        for(size_t j = 0; j < ntowels; ++j) {
            const struct sview towel = sv_create_with_len(towels[j].data, towels[j].size);
            if(i + towel.len > l.len)
                continue;

            struct sview lsub = sv_subview(l, i, towel.len);
            if(sv_cmp(lsub, towel) == 0) {
                arrangment_count[i] += arrangment_count[i + towel.len];
            }
        }
    }


    uint64_t ret = arrangment_count[0];
    free(arrangment_count);
    return ret;
}


int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;

    int it = 0;
    struct string_list towels = { 0 };
    int task1 = 0;
    uint64_t task2 = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        const int line_no = ++it;
        if(l.len == 0)
            continue;

        if(line_no == 1) {
            do {
                struct sview left;
                sv_split(l, ',', &left, &l);
                left = sv_trim_left(left);
                da_append(towels, string_from_sv(left));
            }while(l.len > 0);
        }else{
            // pattern
            uint64_t count = arrangement_count(l, towels.data, towels.size);
            task2 += count;
            if(count > 0) {
                task1 += 1;
            }
        }
    }
    printf("Task 1: %d\n", task1);
    printf("Task 2: %lu\n", task2);

    free(line);
    return 0;
}


/*
Start: T
T :> [any-towel]T
T :> [any-towel]
*/

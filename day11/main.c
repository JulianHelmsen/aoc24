#define AOC_IMPLEMENTATION
#include <aoc.h>

size_t digcount(uint64_t val) {
    if(val < 10ull) return 1;
    if(val < 100ull) return 2;
    if(val < 1000ull) return 3;
    if(val < 10000ull) return 4;
    if(val < 100000ull) return 5;
    if(val < 1000000ull) return 6;
    if(val < 10000000ull) return 7;
    if(val < 100000000ull) return 8;
    if(val < 1000000000ull) return 9;
    if(val < 10000000000ull) return 10;
    if(val < 100000000000ull) return 11;
    if(val < 1000000000000ull) return 12;
    if(val < 10000000000000ull) return 13;
    if(val < 100000000000000ull) return 14;
    if(val < 1000000000000000ull) return 15;
    if(val < 10000000000000000ull) return 16;
    if(val < 100000000000000000ull) return 17;
    if(val < 1000000000000000000ull) return 18;
    if(val < 10000000000000000000ull) return 19;
    return 20;
}

void split(int len, uint64_t v, uint64_t* restrict l, uint64_t* restrict r) {
    int pwd = 1;
    const int hlen = len >> 1;
    for(int i = 0; i < hlen; ++i)
        pwd *= 10;

    *r = v % pwd;
    *l = v / pwd;
}



struct stone {
    uint64_t val;
    int blink;
};

size_t stone_hash(const void* v) {
    const struct stone stone = *(const struct stone*) v;
    return stone.val ^ (uint64_t) stone.blink * 435513234;
}

int stone_eq(const void* a, const void* b) {
    const struct stone stone1 = *(const struct stone*) a;
    const struct stone stone2 = *(const struct stone*) b;
    return stone1.val == stone2.val && stone1.blink == stone2.blink;
}

static uint64_t count_stones(struct hashmap* map, uint64_t val, int blink) {
    if(blink == 0) {return 1;}
    struct stone s = {val, blink};
    uint64_t count = 0;
    if(hashmap_find(map, &s, &count)) {
        return count;
    }

    const int len = digcount(val);
    uint64_t res = 0;
    if(val == 0) {
        res = count_stones(map, 1, blink - 1);
    }else if(len & 1) {
        res = count_stones(map, val * 2024, blink - 1);
    }else{
        uint64_t l;
        uint64_t r;
        split(len, val, &l, &r);
        res = count_stones(map, l, blink - 1);
        res += count_stones(map, r, blink - 1);
    }
    hashmap_insert(map, &s, &res);
    return res;
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;
    struct hashmap map = hashmap_create(stone_hash, stone_eq, struct stone, uint64_t);
    struct hashmap map2 = hashmap_create(stone_hash, stone_eq, struct stone, uint64_t);

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    uint64_t task1 = 0;
    uint64_t task2 = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_trim_left(sv_create_with_len(line, len));

        if(l.len == 0)
            continue;

        while(l.len > 0) {
            struct sview num;
            sv_split(l, ' ', &num, &l);

            const uint64_t val = strtol(num.data, NULL, 10);
            task1 += count_stones(&map, val, 25);
            task2 += count_stones(&map, val, 75);
            l = sv_trim_left(l);
        }
    }
    printf("task1: %lu\n", task1);
    printf("task2: %lu\n", task2);

    free(line);
    free(map.data);
    free(map2.data);

    return 0;
}


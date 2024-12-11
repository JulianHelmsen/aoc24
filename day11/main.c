#define AOC_IMPLEMENTATION
#include <aoc.h>

struct stone {
    uint64_t val;
    int blink;
};

size_t stone_hash(const void* v) {
    const struct stone stone = *(const struct stone*) v;
    return stone.val + (uint64_t) stone.blink * 435513234;
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
    char buf[32] = { 0 };
    const int len = snprintf(buf, sizeof(buf), "%lu", val);
    uint64_t res = 0;
    if(val == 0) {
        res = count_stones(map, 1, blink - 1);
    }else if(len & 1) {
        res = count_stones(map, val * 2024, blink - 1);
    }else{
        const struct sview left = sv_create_with_len(buf, len / 2);
        const struct sview right = sv_create(buf + len / 2);
        uint64_t l = parse_u64(left);
        uint64_t r = parse_u64(right);
        res = count_stones(map, l, blink - 1);
        res += count_stones(map, r, blink - 1);
    }
    hashmap_insert(map, &s, &res);
    return res;

#if 0
    struct Vals {
        size_t capacity;
        size_t size;
        uint64_t* data;
    };

    struct Vals lists[2] = { 0 };
    da_append(lists[0], val);

    for(int i = 0; i < blink; ++i) {
        const int src = i & 1;
        const int dst = (i + 1) & 1;

        for(size_t j = 0; j < lists[src].size; ++j) {
            uint64_t stone = lists[src].data[j];
            char buf[32] = { 0 };
            const int len = snprintf(buf, sizeof(buf), "%lu", stone);

            if(stone == 0) {
                da_append(lists[dst], 1);
            }else if(len & 1) {
                da_append(lists[dst], stone * 2024);
            }else{
                const struct sview left = sv_create_with_len(buf, len / 2);
                const struct sview right = sv_create(buf + len / 2);
                uint64_t l = parse_u64(left);
                uint64_t r = parse_u64(right);
                da_append(lists[dst], l);
                da_append(lists[dst], r);
            }
        }
        lists[src].size = 0;
    }

    struct Vals res = lists[0].size > 0 ? lists[0] : lists[1];

    free(lists[0].data);
    free(lists[1].data);
    return res.size;
#endif
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <blink-count>\n", argv[0]);
        exit(1);
    }

    const int blink_count = atoi(argv[1]);
    if(blink_count == 0) {
        fprintf(stderr, "blink count must be a positive integer\n");
        exit(1);
    }

    struct hashmap map = hashmap_create(stone_hash, stone_eq, struct stone, uint64_t);

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    uint64_t task1 = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_trim_left(sv_create_with_len(line, len));

        if(l.len == 0)
            continue;

        while(l.len > 0) {
            struct sview num;
            sv_split(l, ' ', &num, &l);

            uint64_t val = strtol(num.data, NULL, 10);
            uint64_t stone_count = count_stones(&map, val, blink_count);
            task1 += stone_count;
            l = sv_trim_left(l);
        }
    }
    printf("task1: %lu\n", task1);

    free(line);
    free(map.data);
    return 0;
}


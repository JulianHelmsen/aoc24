#define AOC_IMPLEMENTATION
#include <aoc.h>

#define U64_MAX (0xFFFFFFFFFFFFFFFFUL)

struct vec2 {
    int64_t x;
    int64_t y;
};

struct vec2 parse_vec2(struct sview s) {
    int64_t x_dig = sv_find(s, isdigit);
    if(x_dig == -1) {
        fprintf(stderr, "invalid vec2: %.*s\n", (int) s.len, s.data);
        exit(1);
    }
    s = sv_subview(s, x_dig, SVIEW_NPOS);
    int64_t x = sv_parse_u64(s);
    int64_t comma = sv_split(s, ',', NULL, &s);
    if(comma == -1) {
        fprintf(stderr, "invalid vec2: %.*s\n", (int) s.len, s.data);
        exit(1);
    }
    int64_t y_dig = sv_find(s, isdigit);
    if(y_dig == -1) {
        fprintf(stderr, "invalid vec2: %.*s\n", (int) s.len, s.data);
        exit(1);
    }
    s = sv_subview(s, y_dig, SVIEW_NPOS);
    int64_t y = sv_parse_u64(s);

    return (struct vec2) {x, y};
}

uint64_t calculate_cost(struct vec2 a, struct vec2 b, const struct vec2 dst) {
    const int64_t det = (a.x * b.y - a.y * b.x);
    if(det == 0) {
        assert(false);
    }
    const int64_t invert_unscaled_u = dst.x * b.y - b.x * dst.y;
    const int64_t invert_unscaled_v = -dst.x * a.y + a.x * dst.y;

    if(invert_unscaled_u % det != 0 || invert_unscaled_v % det != 0) {
        return U64_MAX;
    }

    return (invert_unscaled_u * 3 + invert_unscaled_v) / det;
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;

    struct vec2 b1, b2 = { 0 };
    struct vec2 prize = { 0 };
    uint64_t task1 = 0;
    uint64_t task2 = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        struct sview name;
        struct sview rest;
        const int64_t idx = sv_split(l, ':', &name, &rest);
        rest = sv_trim_left(rest);

        assert(idx != -1);
        if(sv_cmp(name, sv_create("Button A")) == 0) {
            b1 = parse_vec2(rest);
        }else if(sv_cmp(name, sv_create("Button B")) == 0) {
            b2 = parse_vec2(rest);
        }else if(sv_cmp(name, sv_create("Prize")) == 0) {
            prize = parse_vec2(rest);
            const uint64_t price = calculate_cost(b1, b2, prize);
            if(price != U64_MAX)
                task1 += price;

            const struct vec2 prize_2 = {
                prize.x + 10000000000000L,
                prize.y + 10000000000000L,
            };
            const uint64_t price2 = calculate_cost(b1, b2, prize_2);
            if(price2 != U64_MAX) {
                task2 += price2;
            }
        }
    }

    free(line);

    printf("task1: %lu\n", task1);
    printf("task2: %lu\n", task2);
    return 0;
}


#define AOC_IMPLEMENTATION
#include <aoc.h>

struct Grid {
    size_t width;
    size_t height;
    char* data;
    size_t capacity;
    size_t size;
};

char at(const struct Grid g, size_t col, size_t row) {
    assert(col < g.width);
    assert(row < g.height);
    return g.data[col + g.width * row];
}

int count_dir(const struct Grid g, int64_t delta_x, int64_t delta_y) {
    const size_t LEN = 4;
    const char XMAS[5] = "XMAS";

    int cnt = 0;
    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            bool eq = true;
            for(size_t step = 0; eq && step < LEN; ++step) {
                size_t dst_x = i + delta_x * step;
                size_t dst_y = j + delta_y * step;

                if(dst_x >= g.width || dst_y >= g.height) {
                    eq = false;
                    break;
                }
                eq &= at(g, dst_x, dst_y) == XMAS[step];
            }
            if(eq)
                ++cnt;
        }
    }
    return cnt;
}

struct Answer {
    int task1;
    int task2;
};

static inline size_t hash(char a, char b) {
    return ((unsigned short) a) << 8 | (unsigned short)b;
}

int count_x(const struct Grid g) {

    bool MAP[0xFFFF] = { true };
    MAP[hash('M', 'S')] = true;
    MAP[hash('S', 'M')] = true;

    int cnt = 0;
    for(size_t j = 1; j + 1 < g.height; ++j) {
        for(size_t i = 1; i + 1 < g.width; ++i) {
            if(at(g, i, j) != 'A')
                continue;

            // check diagonal
            char tl = at(g, i - 1, j - 1);
            char tr = at(g, i + 1, j - 1);
            char bl = at(g, i - 1, j + 1);
            char br = at(g, i + 1, j + 1);
            if(MAP[hash(tl, br)] && MAP[hash(bl, tr)]) {
                ++cnt;
            }
        }
    }
    return cnt;
}


struct Answer count(const struct Grid g) {
    struct dir {
        int x;
        int y;
    };
    struct dir dirs[] = {
        {1, 0},
        {-1, 0},
        {1, 1},
        {-1, 1},
        {1, -1},
        {-1, -1},
        {0, 1},
        {0, -1},
    };

    struct Answer answer = { 0 };
    for(size_t i = 0; i < sizeof(dirs) / sizeof(dirs[0]); ++i) {
        answer.task1 += count_dir(g, dirs[i].x, dirs[i].y);
    }
    answer.task2 = count_x(g);
    return answer;
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    struct Grid g = { 0 };
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        // TODO: handle line
        g.width = l.len;
        for(size_t i = 0; i < l.len; ++i) {
            da_append(g, l.data[i]);
        }
    }

    g.height = g.size / g.width;

    struct Answer answer = count(g);
    printf("Task1: %d\n", answer.task1);
    printf("Task2: %d\n", answer.task2);

    free(line);
    return 0;
}


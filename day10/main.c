#define AOC_IMPLEMENTATION
#include <aoc.h>

struct grid {
    size_t width;
    size_t height;
    int* data;
    size_t capacity;
    size_t size;
};

struct trail_eval_result {
    size_t score;
    size_t rating;
};

static int at(const struct grid g, size_t col, size_t row) {
    assert(col < g.width);
    assert(row < g.height);
    return g.data[col + g.width * row];
}

static struct trail_eval_result trail_eval(const struct grid g, size_t c, size_t r) {
    struct coord {
        size_t x;
        size_t y;
    };

    struct coords {
        size_t capacity;
        size_t size;
        struct coord* data;
    };
    struct trail_eval_result result = { 0 };

    if(at(g, c, r) != 0)
        return result;

    const struct coord origin = {c, r};

    struct coords open = { 0 };
    da_append(open, origin);

    struct coords visited = { 0 };
    while(open.size > 0) {
        struct coord p = open.data[--open.size];
        // visit neighbours
        const int h = at(g, p.x, p.y);
        if(h == 9) {
            result.rating++;
            bool contained = false;
            for(size_t i = 0; i < visited.size; ++i) {
                if(visited.data[i].x == p.x && visited.data[i].y == p.y) {
                    contained = true;
                    break;
                }
            }
            if(contained)
                continue;
            da_append(visited, p);
            result.score++;
            continue;
        }

        const struct coord neighbours[4] = {
            {p.x - 1, p.y},
			{p.x + 1, p.y},
			{p.x, p.y - 1},
			{p.x, p.y + 1},
        };

        for(size_t i = 0; i < 4; ++i) {
            const struct coord c = neighbours[i];
            if(c.x < g.width && c.y < g.height) {
                int h2 = at(g, c.x, c.y);
                if(h + 1 == h2) {
                    // enqueue
                    da_append(open, c);
                }
            }
        }
    }
    return result;
}


int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    size_t line_no = 0;
    struct grid g = { 0 };
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        g.width = l.len;
        for(size_t i = 0; i < l.len; ++i) {
            int height = l.data[i] - '0';
            da_append(g, height);
        }
        ++line_no;
    }
    g.height = g.size / g.width;


    size_t task1 = 0;
    size_t task2 = 0;
    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            const struct trail_eval_result res = trail_eval(g, i, j);
            task1 += res.score;
            task2 += res.rating;
        }
    }

    printf("task1: %zu\n", task1);
    printf("task2: %zu\n", task2);

    free(line);
    return 0;
}


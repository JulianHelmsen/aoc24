#define AOC_IMPLEMENTATION
#include <aoc.h>

#define NULLC '\0'

struct grid {
    size_t width;
    size_t height;
    char* data;
    size_t capacity;
    size_t size;
};

struct coord {
    int64_t x;
    int64_t y;
};



char at(const struct grid g, size_t col, size_t row) {
    if(col >= g.width || row >= g.height)
        return NULLC;
    return g.data[col + g.width * row];
}

bool is_edge(const struct grid g, const char c, const struct coord pos, const struct coord dir) {
    const struct coord dst = {pos.x + dir.x, pos.y + dir.y};
    const char other = at(g, dst.x, dst.y);
    const char pc = at(g, pos.x, pos.y);
    return (pc == c || pc == -c) && other != c && other != -c;
}

struct result {
    uint64_t task2;
    uint64_t task1;
};
struct result calc_price(struct grid g) {
    struct coords {
        size_t capacity;
        size_t size;
        struct coord* data;
    };

    struct coords queue = { 0 };

    struct result res = { 0 };

    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            const char c = at(g, i, j);
            if(c > 0) {
                uint64_t A = 0;
                uint64_t B = 0;
                uint64_t B2 = 0;
                queue.size = 0;
                const struct coord start = {i, j};
                da_append(queue, start);

                while(queue.size > 0) {
                    const struct coord p = queue.data[0];
                    --queue.size;
                    for(size_t i = 0; i < queue.size; ++i) {
                        queue.data[i] = queue.data[i + 1];
                    }

                    if(at(g, p.x, p.y) <= 0)
                        continue;

                    A += 1;

                    struct coord dirs[4] = {
                        {-1, 0},
                        {0, 1},
                        {1, 0},
                        {0, -1},
                    };

                    for(size_t i = 0; i < ARRAY_SIZE(dirs); ++i) {
                        const struct coord dir = dirs[i];
                        const struct coord n = {p.x + dir.x, p.y + dir.y};
                        const char nc = at(g, n.x, n.y);
                        if(nc == c) {
                            da_append(queue, n);
                        }else if(is_edge(g, c, p, dir)) {
                            B += 1;


                            const struct coord dirs_along_edge[2] = {
                                dirs[(i + 1) % ARRAY_SIZE(dirs)],
                                dirs[(i + 3) % ARRAY_SIZE(dirs)],
                            };
                            bool all_visited = true;
                            for(size_t i = 1; i < g.width + g.height; ++i) {
                                const struct coord nx = {p.x + dirs_along_edge[0].x * i, p.y + dirs_along_edge[0].y * i};
                                if(!is_edge(g, c, nx, dir))
                                    break;
                                if(is_edge(g, c, nx, dir)) {
                                    if(at(g, nx.x, nx.y) == c) {
                                        all_visited = false;
                                        break;
                                    }
                                }
                            }
                            for(size_t i = 1; i < g.width + g.height; ++i) {
                                const struct coord ny = {p.x + dirs_along_edge[1].x * i, p.y + dirs_along_edge[1].y * i};
                                if(!is_edge(g, c, ny, dir))
                                    break;
                                if(is_edge(g, c, ny, dir)) {
                                    if(at(g, ny.x, ny.y) == c) {
                                        all_visited = false;
                                        break;
                                    }
                                }
                            }

                            if(all_visited) {
                                B2 += 1;
                            }
                        }
                    }

                    g.data[p.x + p.y * g.width] = -c;
                }

                res.task1 += B * A;
                res.task2 += B2 * A;
            }
        }
    }
    free(queue.data);
    return res;
}


int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    struct grid g = { 0 };
    size_t line_no = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0 || l.data[0] == '#')
            continue;

        // TODO: handle line
        g.width = l.len;
        for(size_t i = 0; i < l.len; ++i) {
            da_append(g, l.data[i]);
        }
        ++line_no;
    }
    g.height = g.size / g.width;

    struct result res =  calc_price(g);
    printf("task1: %lu\n", res.task1);
    printf("task2: %lu\n", res.task2);

    free(line);
    return 0;
}


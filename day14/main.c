#define AOC_IMPLEMENTATION
#include <aoc.h>
#include <unistd.h>
#include <signal.h>

#if 1
#define GRID_WIDTH ((int64_t) 101)
#define GRID_HEIGHT ((int64_t) 103)
#else
#define GRID_WIDTH 11
#define GRID_HEIGHT 7
#endif

static bool terminate_search = false;

void sig_handler(int s) {
    (void) s;
    terminate_search = true;
}

struct robot {
    int64_t x;
    int64_t y;
    int64_t dx;
    int64_t dy;
};

struct position {
    uint64_t x;
    uint64_t y;
};
struct positions {
    size_t size;
    size_t capacity;
    struct position* data;
};

void landing_pos(int steps, struct robot r, uint64_t* dst_x, uint64_t* dst_y) {
    const int64_t OFF_X = GRID_WIDTH * 1000 * steps;
    const int64_t OFF_Y = GRID_HEIGHT * 1000 * steps;

    int64_t x = r.x + r.dx * steps;
    int64_t y = r.y + r.dy * steps;

    *dst_x = (x + OFF_X) % GRID_WIDTH;
    *dst_y = (y + OFF_Y) % GRID_HEIGHT;

    assert(*dst_x < GRID_WIDTH);
    assert(*dst_y < GRID_HEIGHT);
}

int landing_at(int steps, struct robot r) {
    uint64_t mx, my;
    landing_pos(steps, r, &mx, &my);

    assert(GRID_WIDTH & 1);
    assert(GRID_HEIGHT & 1);

    if(mx < GRID_WIDTH / 2) {
        // 0 or 2
        if(my < GRID_HEIGHT / 2) {
            return 0;
        }else if(my >= 1 + GRID_HEIGHT / 2) {
            return 2;
        }
    }else if(mx >= 1 + GRID_WIDTH / 2) {
        // 1 or 3
        if(my < GRID_HEIGHT / 2) {
            return 1;
        }else if(my >= 1 + GRID_HEIGHT / 2) {
            return 3;
        }
    }

    return -1;
}

struct robot parse_robot(struct sview line) {
    assert(line.len > 2);
    struct sview pos_str, vel_str;
    sv_split(line, ' ', &pos_str, &vel_str);
    pos_str = sv_subview(pos_str, 2, SVIEW_NPOS);
    vel_str = sv_subview(vel_str, 2, SVIEW_NPOS);


    struct sview psx, psy, dsx, dsy;
    sv_split(pos_str, ',', &psx, &psy);
    sv_split(vel_str, ',', &dsx, &dsy);


    errno = 0;
    struct robot r;
    r.x = sv_parse_i64(psx);
    r.y = sv_parse_i64(psy);
    r.dx = sv_parse_i64(dsx);
    r.dy = sv_parse_i64(dsy);
    if(errno != 0) {
        perror("sv_parse_i64");
        exit(1);
    }
    return r;
}

size_t max_connected_component(const char* grid) {
    struct positions stack = { 0 };
    bool dirty[GRID_HEIGHT * GRID_WIDTH] = { false };
    size_t max = 0;

    for(size_t j = 0; j < GRID_HEIGHT; ++j)
        for(size_t i = 0; i < GRID_WIDTH; ++i) {
            da_clear(stack);
            struct position p = {i, j};
            da_append(stack, p);
            size_t cc_size = 0;

            while(stack.size > 0) {
                const struct position p = stack.data[--stack.size];
                if(p.x >= GRID_WIDTH || p.y >= GRID_HEIGHT)
                    continue;
                const char c = grid[p.x + p.y * GRID_WIDTH];
                const bool is_dirty = dirty[p.x + p.y * GRID_WIDTH];
                if(c == '0' || is_dirty)
                    continue;

                ++cc_size;
                dirty[p.x + p.y * GRID_WIDTH] = true;

                const struct position neighbours[4] = {
                    {p.x - 1, p.y},
                    {p.x + 1, p.y},
                    {p.x, p.y - 1},
                    {p.x, p.y + 1},
                };
                for(size_t k = 0; k < ARRAY_SIZE(neighbours); ++k) {
                    const struct position n = neighbours[k];
                    da_append(stack, n);
                }
            }
            if(cc_size > max)
                max = cc_size;
        }

    free(stack.data);
    return max;
}

int main(const int argc, const char** argv) {

    (void) argc;
    (void) argv;

    struct sigaction sa = {
        .sa_handler = sig_handler,
    };
    sigaction(SIGINT, &sa, NULL);

    char* line = NULL;
    ssize_t len;
    size_t n = 0;

    struct robots {
        size_t size;
        size_t capacity;
        struct robot* data;
    }robots = { 0 };

    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        da_append(robots, parse_robot(l));
    }

    free(line);

    uint64_t quadrants[4] = { 0 };

    for(size_t i = 0; i < robots.size; ++i) {
        const int quad_idx = landing_at(100, robots.data[i]);
        if(quad_idx != -1) {
            ++quadrants[quad_idx];
        }
    }
    printf("%lu, %lu, %lu, %lu\n", quadrants[0], quadrants[1], quadrants[2], quadrants[3]);
    const uint64_t task1 = quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];



    char grid[GRID_HEIGHT * GRID_WIDTH];
    size_t best_so_far = 0;
    size_t best_it = 0;
    struct positions positions = { 0 };
    for(size_t i = 0; !terminate_search; ++i) {
        da_clear(positions);

        for(size_t r  = 0; r < robots.size; ++r) {
            struct position p;
            landing_pos(i, robots.data[r], &p.x, &p.y);
            da_append(positions, p);
        }

        memset(grid, '0', sizeof(grid));
        for(size_t j = 0; j < positions.size; ++j) {
            grid[positions.data[j].x + positions.data[j].y * GRID_WIDTH]++;
        }

        size_t score = max_connected_component(grid);
        if(best_so_far > 1500) {
            printf("\rtime: %lu\tcurrent score: %lu, best score: %lu.\t\t Press CTRL+C to terminate search!", i, score, best_so_far);
        }else{
            printf("\rtime: %lu\tscore: %lu, best: %lu", i, score, best_so_far);
        }

        if(score > best_so_far) {
            best_so_far = score;
            best_it = i;
            printf("\n");
        }else{
            continue;
        }

        for(size_t j = 0; j < GRID_HEIGHT; ++j) {
            for(size_t p = 0; p < GRID_WIDTH; ++p) {
                char c = grid[GRID_WIDTH * j + p];
                fputc(c == '0' ? '*' : c, stdout);
            }
            fputc('\n', stdout);
        }
    }

    printf("\nTask1: %lu\n", task1);
    printf("Task2: %lu\n", best_it);

    free(positions.data);
    free(robots.data);
    return 0;
}


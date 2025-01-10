#define AOC_IMPLEMENTATION
#include <aoc.h>

#define TEST 0

#if TEST
#define GRID_RANGE (6)
#define GRID_SIZE (GRID_RANGE + 1)
#define T 12
#else
#define GRID_RANGE (70)
#define GRID_SIZE (GRID_RANGE + 1)
#define T 1024
#endif

struct state {
    int time;
    size_t x;
    size_t y;
};

struct states {
    struct state* data;
    size_t size;
    size_t capacity;
};

struct state_v2 {
    int time;
    size_t x;
    size_t y;
    int time_max;
};


struct states_v2 {
    struct state_v2* data;
    size_t size;
    size_t capacity;
};


int state_cmp(struct state a, struct state b, const void* ctx) {
    (void) ctx;
    const int adx = GRID_SIZE - 1 - a.x;
    const int ady = GRID_SIZE - 1 - a.y;
    const int bdx = GRID_SIZE - 1 - b.x;
    const int bdy = GRID_SIZE - 1 - b.y;
    assert(adx >= 0);
    assert(ady >= 0);
    assert(bdx >= 0);
    assert(bdy >= 0);
    const int a_score = a.time + adx + ady;
    const int b_score = b.time + bdx + bdy;
    return (b_score - a_score);
}


int steps_needed(const int* map, int time) {

    const struct state initial = {
        .time = 0,
        .x = 0,
        .y = 0
    };


    struct states pq = { 0 };
    heap_push(pq, initial, state_cmp, NULL);

    int reachable_after[GRID_SIZE * GRID_SIZE];
    for(size_t i = 0; i < GRID_SIZE * GRID_SIZE; ++i)
        reachable_after[i] = INT_MAX;

    int rtime = -1;
    while(pq.size > 0) {

        struct state s;
        heap_pop(pq, &s, state_cmp, NULL);
        reachable_after[s.x + s.y * GRID_SIZE] = s.time;

        if(s.x == GRID_SIZE - 1 && s.y == GRID_SIZE - 1) {
            // found target
            rtime = s.time;
            break;
        }

        // enqueue neighbour states
        const size_t x[4] = {
            s.x - 1,
            s.x,
            s.x + 1,
            s.x
        };
        const size_t y[4] = {
            s.y,
            s.y - 1,
            s.y,
            s.y + 1
        };

        for(size_t i = 0; i < ARRAY_SIZE(x); ++i) {
            const struct state next = {
                .time = s.time + 1,
                .x = x[i],
                .y = y[i]
            };


            if(next.x >= GRID_SIZE || next.y >= GRID_SIZE)
                continue;
            const int destruction_at = map[next.x + next.y * GRID_SIZE];
            if(destruction_at < time && destruction_at >= 0)
                continue;
            if(reachable_after[next.x + next.y * GRID_SIZE] != INT_MAX)
                continue;
            heap_push(pq, next, state_cmp, NULL);
        }

    }

    free(pq.data);
    return rtime;
}


int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    int step_no = 0;

    int map[GRID_SIZE * GRID_SIZE];
    for(size_t i = 0; i < GRID_SIZE * GRID_SIZE; ++i)
        map[i] = -1;

    struct coord {
        size_t x;
        size_t y;
    };
    struct coords {
        size_t size;
        size_t capacity;
        struct coord* data;
    }coords;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        struct sview x_str, y_str;
        int s_pos = sv_split(l, ',', &x_str, &y_str);
        assert(s_pos != -1);
        errno = 0;
        uint64_t x = sv_parse_u64(x_str);
        uint64_t y = sv_parse_u64(y_str);
        if(errno == EINVAL) {
            perror("sv_parse_u64");
            exit(1);
        }
        struct coord c;
        c.x = x;
        c.y = y;
        da_append(coords, c);

        assert(x < GRID_SIZE);
        assert(y < GRID_SIZE);

        map[x + y * GRID_SIZE] = step_no;
        step_no += 1;
    }
    free(line);


    const int task1 = steps_needed(map, T);
    printf("Task1: %d\n", task1);

    int t = 0;
    while(true) {
        const int distance = steps_needed(map, t);
        if(distance < 0) {
            break;
        }
        t += 1;
    }
    printf("Task2: \"%zu,%zu\"\n", coords.data[t - 1].x, coords.data[t - 1].y);
    free(coords.data);
    return 0;
}


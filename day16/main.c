#define AOC_IMPLEMENTATION
#include <aoc.h>


struct grid {
    size_t width;
    size_t height;
    char* data;
    size_t capacity;
    size_t size;
};

struct vec2 {
    int x;
    int y;
};

struct state {
    struct vec2 pos;
    int64_t cost;
    int dir;
};

struct state_list {
    size_t size;
    size_t capacity;
    struct state* data;
};

const struct vec2 dirs[4] = {
    {1, 0},
    {0, 1},
    {-1, 0},
    {0, -1},
};


int h(struct state s, const struct vec2 end) {
    int dx = s.pos.x - end.x;
    int dy = s.pos.y - end.y;
    if(dx < 0) dx = -dx;
    if(dy < 0) dy = -dy;
    const int manhattan = dx + dy;

    // count number of turns
    int turns = 0;
    switch(s.dir){
    case 0: // east
        turns += s.pos.y != end.y;
        turns += s.pos.x > end.x;
        break;
    case 1: // south
        turns += s.pos.x != end.x;
        turns += s.pos.y > end.y;
        break;
    case 2: // west
        turns += s.pos.y != end.y;
        turns += s.pos.x < end.x;
        break;
    case 3: // north
        turns += s.pos.x != end.x;
        turns += s.pos.y < end.y;
        break;

    }
    return manhattan + turns * 1000;
}

int state_cmp(struct state a, struct state b, const struct vec2 end)  {
    const int est_a = a.cost + h(a, end);
    const int est_b = b.cost + h(b, end);
    return est_b - est_a;
}


struct task_result {
    int64_t task1;
    int64_t task2;
};

struct task_result find_cheapest_path(struct grid g, size_t start_x, size_t start_y, size_t end_x, size_t end_y) {
    const struct state initial = {
        .pos = {start_x, start_y},
        .cost = 0,
        .dir = 0,
    };
    const struct vec2 end = {end_x, end_y};

    struct state_list prio_queue = { 0 };
    heap_push(prio_queue, initial, state_cmp, end);
    struct task_result result = {-1, -1};

    uint64_t* costs = malloc(g.width * g.height * ARRAY_SIZE(dirs) * sizeof(uint64_t));
#define cost_at(x, y, dir) costs[(x + y * g.width) * ARRAY_SIZE(dirs) + dir]
    if(costs == NULL) {
        perror("malloc");
        exit(1);
    }
    memset(costs, 0xFF, sizeof(uint64_t) * g.width * g.height * ARRAY_SIZE(dirs));
    int64_t cheapest = -1;
    while(prio_queue.size > 0) {
        struct state s;
        heap_pop(prio_queue, &s, state_cmp, end);

        const char c = g.data[s.pos.x + s.pos.y * g.width];
        assert(c != '#');
        if(c != '.') {
            if(c & (1 << s.dir)) {
                continue;
            }
        }

        // mark as visited
        if(c == '.')
            g.data[s.pos.x + s.pos.y * g.width] = 0;
        g.data[s.pos.x + s.pos.y * g.width] |= 1 << s.dir;


        cost_at(s.pos.x, s.pos.y, s.dir) = s.cost;



        // check endpoint
        if(s.pos.x == (int) end_x && s.pos.y == (int) end_y) {
            if(cheapest != -1 && s.cost > cheapest) {
                break;
            }
            cheapest = s.cost;
        }

        struct state rl = {
            .pos = s.pos,
            .cost = s.cost + 1000,
            .dir = (s.dir + 1) % ARRAY_SIZE(dirs)
        };

        struct state rr = {
            .pos = s.pos,
            .cost = s.cost + 1000,
            .dir = (s.dir - 1 + ARRAY_SIZE(dirs)) % ARRAY_SIZE(dirs)
        };

        const struct vec2 dir = dirs[s.dir];
        const struct vec2 dst = {
            .x = s.pos.x + dir.x,
            .y = s.pos.y + dir.y,
        };

        heap_push(prio_queue, rl, state_cmp, end);
        heap_push(prio_queue, rr, state_cmp, end);

        if(dst.x >= 0 && dst.x < (int) g.width && dst.y >= 0 && dst.y < (int) g.height) {
            if(g.data[dst.x + dst.y * g.width] != '#') {
                const struct state forward = {
                    .pos = dst,
                    .cost = s.cost + 1,
                    .dir = s.dir
                };
                heap_push(prio_queue, forward, state_cmp, end);
            }
        }
    }
    free(prio_queue.data);
    result.task1 = cheapest;
    if(cheapest == -1)
        return result;

    // backtracking for part 2
    struct state_list best_path_states = { 0 };
    for(size_t i = 0; i < 4; ++i)
        if(cost_at(end.x, end.y, i) == (uint64_t) cheapest) {
            struct state s = {
                .pos = end,
                .dir = (int) i,
                .cost = cheapest
            };
            da_append(best_path_states, s);
        }

    while(best_path_states.size > 0) {
        struct state b = best_path_states.data[--best_path_states.size];
        g.data[b.pos.x + b.pos.y * g.width] = 0xFF; //mark as best path
        if(b.pos.x == (int) start_x && b.pos.y == (int) start_y)
            continue;

        const struct state predecessors[] = {
            (struct state) {
                .pos = {
                    b.pos.x - dirs[b.dir].x,
                    b.pos.y - dirs[b.dir].y,
                },
                .dir = b.dir,
                .cost = b.cost - 1,
            },
            (struct state) {
                .pos = b.pos,
                .dir = (b.dir + 1) % ARRAY_SIZE(dirs),
                .cost = b.cost - 1000,
            },
            (struct state) {
                .pos = b.pos,
                .dir = (b.dir - 1 + ARRAY_SIZE(dirs)) % ARRAY_SIZE(dirs),
                .cost = b.cost - 1000,
            },
        };

        for(size_t i = 0; i < ARRAY_SIZE(predecessors); ++i) {
            const struct state pred = predecessors[i];
            if(cost_at(pred.pos.x, pred.pos.y, pred.dir) == (uint64_t) pred.cost) {
                da_append(best_path_states, pred);
            }
        }
    }

    result.task2 = 0;
    for(size_t i = 0; i < g.width * g.height; ++i)
        result.task2 += (unsigned char) g.data[i] == 0xFF;

    free(best_path_states.data);
    free(costs);

    return result;
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;

    size_t line_no = 0;
    struct grid g =  { 0 };
    size_t start_x = 0, start_y = 0;
    size_t end_x = 0, end_y = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;


        g.width = l.len;
        for(size_t i = 0; i < l.len; ++i) {
            if(l.data[i] == 'S') {
                start_x = i;
                start_y = line_no;
                da_append(g, '.');
            } else if(l.data[i] == 'E') {
                end_x = i;
                end_y = line_no;
                da_append(g, '.');
            }else{
                da_append(g, l.data[i]);
            }

        }
        ++line_no;
    }
    g.height = g.size / g.width;

    struct task_result res = find_cheapest_path(g, start_x, start_y, end_x, end_y);
    if(res.task1 == -1) {
        printf("Task1: path not found\n");
        exit(1);
    }
    printf("Task1: %ld\n", res.task1);
    printf("Task2: %ld\n", res.task2);


    free(g.data);
    free(line);
    return 0;
}


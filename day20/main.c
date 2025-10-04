#define AOC_IMPLEMENTATION
#include <aoc.h>


int absi(int a) {return a < 0 ? -a : a; }

static char visited[1 << 20] = { 0 };
static int distances[1 << 20] = { 0 };

struct state {
    bool has_boost;
    int x;
    int y;
    int transition_count;
};

struct state_search {
    struct state* items;
    size_t front;
    size_t back;
    size_t capacity;
};

struct grid {
    char* data;
    size_t capacity;
    size_t size;
    size_t width;
    size_t height;
};

#define in_grid(g, s) (s.x >= 0 && (size_t) s.x < g.width && s.y >= 0 && (size_t) s.y < g.height)
#define grid_at(g, s) (in_grid(g, s) ? (g).data[(g).width * (s).y + (s).x] : '#')

static int shortest = -1;

void append_neighbours(struct state_search* states, struct grid g, struct state s) {
    const int dx[] = {-1, 0, 1, 0};
    const int dy[] = {0, 1, 0, -1};

    for(int i = 0; i < 4; ++i) {
        struct state next = {
            .x = s.x + dx[i],
            .y = s.y + dy[i],
            .has_boost = s.has_boost,
            .transition_count = s.transition_count + 1
        };
        if(in_grid(g, next) && grid_at(g, next) == '#') {
            if(!next.has_boost) continue;
            next.has_boost = false;
        }

        queue_push_back(states, next);
    }
}

int shortest_path(struct grid g, struct state start) {
    struct state_search states = { 0 };
    start.has_boost = false;
    queue_push_back(&states, start);

    memset(visited, 0, g.width * g.height);
    start.has_boost = false;
    while(!queue_empty(&states)) {
        struct state s = queue_pop_front(&states);
        if(in_grid(g, s) && visited[s.x + s.y * g.width]) continue;
        distances[s.x + s.y * g.width] = s.transition_count;
        visited[s.x + s.y * g.width] = 1;
        if(grid_at(g, s) == 'E')
            return s.transition_count;

        append_neighbours(&states, g, s);
    }

    return -1;
}

int task1(struct grid g) {

    int count = 0;
    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            const char cell = g.data[i + j * g.width];
            if(cell != '#') continue;

            const int dx[] = {-1, 0, 1, 0};
            const int dy[] = {0, 1, 0, -1};
            for(int a = 0; a < 4; ++a) {
                const int p1x = i + dx[a];
                const int p1y = j + dy[a];
                if(p1x < 0 || p1x >= (int) g.width || p1y < 0 || p1y >= (int) g.height) continue;
                for(int b = 0; b < 4; b++) {
                    const int p2x = i + dx[b];
                    const int p2y = j + dy[b];
                    if(p1x == p2x && p1y == p2y) continue;
                    if(p2x < 0 || p2x >= (int) g.width || p2y < 0 || p2y >= (int) g.height) continue;
                    if(g.data[p1x + p1y * g.width] == '#') continue;
                    if(g.data[p2x + p2y * g.width] == '#') continue;

                    const int d_to_1 = distances[p1x + p1y * g.width];
                    const int d_to_2 = distances[p2x + p2y * g.width];
                    // const int total_distance = d_to_1 + 2 + (shortest - d_to_2);
                    if(d_to_1 < d_to_2) continue;
                    int saved = abs(d_to_2 - d_to_1) - 2;
                    if(saved >= 100) count += 1;
                    // printf("%d\n", total_distance);

                }
            }
        }
    }
    return count;
}

int task2(struct grid g) {
    int count = 0;
    for(size_t y = 0; y < g.height; ++y) {
        for(size_t x = 0; x < g.width; ++x) {
            if(g.data[x + y * g.width] == '#') continue;
            for(size_t y2 = 0; y2 < g.height; ++y2) {
                for(size_t x2 = 0; x2 < g.width; ++x2) {
                    if(x == x2 && y == y2) continue;
                    if(g.data[x2 + y2 * g.width] == '#') continue;
                    const int dist = absi((int) x2 - x) + absi((int) y2 - y);
                    if(dist > 20) continue;

                    const int d_to_1 = distances[x + y * g.width];
                    const int d_to_2 = distances[x2 + y2 * g.width];
                    if(d_to_1 < d_to_2) continue;
                    int saved = abs(d_to_2 - d_to_1) - dist;
                    // printf("saved: %d\n", saved);
                    if(saved >= 100) count += 1;

                }
            }
        }
    }
    return count;
}



void print(struct grid g) {
    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            const size_t idx = i + j * g.width;
            if(g.data[idx] == '#') {
                printf("######");
            } else {
                printf(" %04d ", distances[i + j * g.width]);
            }
        }
        printf("\n");
    }
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    struct grid g = { 0 };

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    struct state start;
    start.has_boost = true;
    start.x = -1;
    start.y = -1;
    start.transition_count = 0;

    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        g.width = l.len;
        for(size_t i = 0; i < l.len; ++i) {
            char c = l.data[i];
            if(c == 'S') {
                c = '.';
                start.x = i;
                start.y = g.size / g.width;
            }

            da_append(g, c);
        }
    }

    g.height = g.size / g.width;

    shortest = shortest_path(g, start);
    printf("Shortest-non-cheating-path: %d\n", shortest);

    // print(g);
    int t1 = task1(g);
    printf("Task1: %d\n", t1);

    int t2 = task2(g);
    printf("Task2: %d\n", t2);

    free(g.data);
    free(line);
    return 0;
}


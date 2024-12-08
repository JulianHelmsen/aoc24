#define AOC_IMPLEMENTATION
#include <aoc.h>

struct position {
    int64_t x;
    int64_t y;
};

struct position_list {
    struct position* data;
    size_t capacity;
    size_t size;
};

struct position_list find_anti_nodes1(const struct position* pos, size_t npos, int64_t width, int64_t height) {
    struct position_list result = { 0 };
    for(size_t i = 0; i < npos; ++i) {
        const struct position a = pos[i];
        for(size_t j = i + 1; j < npos; ++j) {
            const struct position b = pos[j];

            const int64_t dx = b.x - a.x;
            const int64_t dy = b.y - a.y;

            // a1 = b + (b - a)
            // a2 = a - (b - a)

            const struct position a1 = {b.x + dx , b.y + dy};
            bool outside = a1.x < 0 || a1.y < 0 || a1.x >= (int64_t) width || a1.y >= (int64_t) height;
            if(!outside) {
                da_append(result, a1);
            }

            const struct position a2 = {a.x - dx, a.y - dy};
            bool outside2 = a2.x < 0 || a2.y < 0 || a2.x >= (int64_t) width || a2.y >= (int64_t) height;
            if(!outside2) {
                da_append(result, a2);
            }
        }
    }

    return result;
}


struct position_list find_anti_nodes2(const struct position* pos, size_t npos, int64_t width, int64_t height) {
    struct position_list result = { 0 };
    for(size_t i = 0; i < npos; ++i) {
        const struct position a = pos[i];
        for(size_t j = i + 1; j < npos; ++j) {
            const struct position b = pos[j];

            const int64_t dx = b.x - a.x;
            const int64_t dy = b.y - a.y;

            // a1 = b + (b - a)
            // a2 = a - (b - a)

            for(size_t i = 0; ; ++i) {
                const struct position a1 = {b.x + dx * i , b.y + dy * i};
                bool outside = a1.x < 0 || a1.y < 0 || a1.x >= (int64_t) width || a1.y >= (int64_t) height;
                if(!outside) {
                    da_append(result, a1);
                }else{
                    break;
                }
            }

            for(size_t i = 0; ; ++i) {
                const struct position a2 = {a.x - dx * i, a.y - dy * i};
                bool outside2 = a2.x < 0 || a2.y < 0 || a2.x >= (int64_t) width || a2.y >= (int64_t) height;
                if(!outside2) {
                    da_append(result, a2);
                }else{
                    break;
                }
            }
        }
    }

    return result;
}


void append_unique(struct position_list* l, const struct position* append, size_t nappend) {
    for(size_t i = 0; i < nappend; ++i) {
        const struct position p2 = append[i];
        bool contained = false;
        for(size_t j = 0; j < l->size; ++j) {
            const struct position p = l->data[j];
            if(p.x == p2.x && p.y == p2.y) {
                contained = true;
                break;
            }
        }
        
        if(!contained)
            da_append(*l, p2);
    }
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;

    struct position_list antenna_list[128] = { 0 };

    size_t width = 0;
    int64_t line_no = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        width = l.len;
        for(size_t i = 0; i < l.len; ++i) {
            const char c = l.data[i];
            if(c != '.' && c != '#') {
                struct position a = {
                    (int64_t) i, line_no
                };
                da_append(antenna_list[(size_t) c], a);
            }
        }
        line_no++;
    }

    struct position_list result1 = { 0 };
    struct position_list result2 = { 0 };

    for(size_t i = 0; i < sizeof(antenna_list) / sizeof(antenna_list[0]); ++i) {
        const struct position_list antennas = antenna_list[i];
        struct position_list l = find_anti_nodes1(antennas.data, antennas.size, width, line_no);
        append_unique(&result1, l.data, l.size);
        struct position_list l2 = find_anti_nodes2(antennas.data, antennas.size, width, line_no);
        append_unique(&result2, l2.data, l2.size);
        free(l.data);
        free(l2.data);
    }

    for(size_t i = 0; i < sizeof(antenna_list) / sizeof(antenna_list[0]); ++i)
        free(antenna_list[i].data);

    free(result1.data);
    free(result2.data);
    printf("Task1: %lu\n", result1.size);
    printf("Task2: %lu\n", result2.size);

    free(line);
    return 0;
}


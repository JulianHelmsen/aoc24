#define AOC_IMPLEMENTATION
#include <aoc.h>

const char dir_bits[4] = {
    1, 2, 4, 8
};

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

void print_grid(FILE* f, const struct Grid g, size_t x, size_t y) {
    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            if(i == x && j == y) {
                fprintf(f, "^");
            }else{
                char c = at(g, i, j);
                if(c != '.' && c != '#') {
                    const bool vert = (at(g, i, j) & (dir_bits[0] | dir_bits[2])) != 0;
                    const bool hor = (at(g, i, j) & (dir_bits[1] | dir_bits[3])) != 0;
                    if(vert && hor) {
                        c = '+';
                    }else if(vert) {
                        c = '|';
                    }else if(hor) {
                        c = '-';
                    }else{
                        c = '?';
                    }
                }
                fprintf(f, "%c", c);
            }
        }
        fprintf(f, "\n");
    }
}



/**
 * Computes the length of the path of the guard starting in the upwards direction.
 * if the guard loops the function returns -1
 */
int compute_path(struct Grid g, size_t guard_x, size_t guard_y) {
    const int dirs[4][2] = {
        {0, -1},
        {1, 0},
        {0, 1},
        {-1, 0}
    };
    int dir = 0;

    int path_len = 1;
    g.data[guard_x + guard_y * g.width] = dir_bits[dir];

    while(guard_x < g.width && guard_y < g.height) {
        const size_t dst_x = guard_x + dirs[dir][0];
        const size_t dst_y = guard_y + dirs[dir][1];


        if(dst_x >= g.width || dst_y >= g.height) {
            goto defer;
        }else if(at(g, dst_x, dst_y) == '#') {
            // rotate
            dir = (dir + 1) % 4;
        }else{
            guard_x = dst_x;
            guard_y = dst_y;
            if(g.data[guard_x + guard_y * g.width] == '.') {
                g.data[guard_x + guard_y * g.width] = '\0';
                ++path_len;
            }
            if(g.data[guard_x + guard_y * g.width] & dir_bits[dir]) {
                path_len = -1;
                goto defer;
            }
            g.data[guard_x + guard_y * g.width] |= dir_bits[dir];
        }
    }

defer:
    // cleanup
    for(size_t i = 0; i < g.width * g.height; ++i)
        if(g.data[i] <= 0xF)
            g.data[i] = '.';
    return path_len;
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    size_t guard_x = 0;
    size_t guard_y = 0;
    size_t line_no = 0;
    struct Grid g =  { 0 };
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        g.width = l.len;
        for(size_t i = 0; i < l.len; ++i) {
            if(l.data[i] == '^') {
                guard_x = i;
                guard_y = line_no;
            }
            da_append(g, l.data[i]);
        }
        ++line_no;
    }
    g.height = g.size / g.width;

    // compute path
    const int path_len = compute_path(g, guard_x, guard_y);
    int num_loops = 0;

    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            if(at(g, i, j) == '.' && (i != guard_x || j != guard_y)) {
                g.data[i + j * g.width] = '#';

                const int len = compute_path(g, guard_x, guard_y);
                if(len == -1) {
                    ++num_loops;
                }

                g.data[i + j * g.width] = '.';
            }
        }
    }
    printf("answer1: %d\n", path_len);
    printf("answer2: %d\n", num_loops);

    free(g.data);
    free(line);
    return 0;
}


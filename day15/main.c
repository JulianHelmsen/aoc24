#define AOC_IMPLEMENTATION
#include <aoc.h>

struct grid {
    size_t width;
    size_t height;
    char* data;
    size_t capacity;
    size_t size;
};

void dir(char op, int* dx, int* dy) {
    switch(op) {
        case '^':
            *dx = 0;
            *dy = -1;
            break;
        case '<':
            *dx = -1;
            *dy = 0;
            break;
        case 'v':
            *dx = 0;
            *dy = 1;
            break;
        case '>':
            *dx = 1;
            *dy = 0;
            break;
        default:
            fprintf(stderr, "Invalid char: %c = (char) %d\n)", op, (int) op);
            assert(false && "unreachable");
    }
}

bool can_push(struct grid g, size_t x, size_t y, int dx, int dy) {
    const char curr = g.data[x + y * g.width];
    const char dst_x = x + dx;
    const char dst_y = y + dy;
    if(curr == '.')
        return true;
    if(curr == '#')
        return false;
    if(curr == 'O')
        return can_push(g, dst_x, dst_y, dx, dy);

    if(curr == '[') {
        if(dx == -1) {
            return can_push(g, dst_x, dst_y, dx, dy);
        }else{
            return can_push(g, dst_x + 1, dst_y, dx, dy) && can_push(g, dst_x, dst_y, dx, dy);
        }
    }

    if(curr == ']') {
        if(dx == 1) {
            return can_push(g, dst_x, dst_y, dx, dy);
        }else{
            return can_push(g, dst_x - 1, dst_y, dx, dy) && can_push(g, dst_x, dst_y, dx, dy);
        }
    }
    assert(false);
    return false;
}
bool attempt_push(struct grid g, size_t x, size_t y, int dx, int dy);

bool attempt_push_single(struct grid g, size_t x, size_t y, int dx, int dy) {

    const char dst_x = x + dx;
    const char dst_y = y + dy;

    const char c = g.data[x + y * g.width];
    assert(c == '[' || c == ']');

    bool success = attempt_push(g, dst_x, dst_y, dx, dy);
    assert(success);

    g.data[dst_x + dst_y * g.width] = c;
    g.data[x + y * g.width] = '.';

    return true;
}


bool attempt_push(struct grid g, size_t x, size_t y, int dx, int dy) {

    if(!can_push(g, x, y, dx, dy))
        return false;

    const char dst_x = x + dx;
    const char dst_y = y + dy;

    const char c = g.data[x + y * g.width];
    if(c == '.')
        return true;
    if(c == 'O') {
        attempt_push(g, dst_x, dst_y, dx, dy);
    }else if(c == '[') {
        if(dx == 1) {
            attempt_push_single(g, x + 1, y, dx, dy);
            attempt_push_single(g, x, y, dx, dy);
        }else if(dx == -1) {
            attempt_push_single(g, x, y, dx, dy);
            attempt_push_single(g, x + 1, y, dx, dy);
        }else{
            attempt_push_single(g, x, y, dx, dy);
            attempt_push_single(g, x + 1, y, dx, dy);
        }
    }else if(c == ']') {
        if(dx == -1) {
            attempt_push_single(g, x - 1, y, dx, dy);
            attempt_push_single(g, x, y, dx, dy);
        }else if(dx == 1) {
            attempt_push_single(g, x, y, dx, dy);
            attempt_push_single(g, x - 1, y, dx, dy);
        }else{
            attempt_push_single(g, x, y, dx, dy);
            attempt_push_single(g, x - 1, y, dx, dy);
        }
    }

    g.data[dst_x + dst_y * g.width] = c;
    g.data[x + y * g.width] = '.';

    return true;
}

void do_step(const char op, size_t*  restrict rx, size_t* restrict ry, struct grid g) {
    const size_t ox = *rx;
    const size_t oy = *ry;
    (void) ox;
    (void) oy;
    if(op == '^') {
        int x = 3;
        (void) x;
    }

    int dx, dy;
    dir(op, &dx, &dy);

    // boundaries
    assert(*rx > 0);
    assert(*ry > 0);
    assert(*rx + 1 < g.width);
    assert(*ry + 1 < g.height);

    const size_t dst_x = *rx + dx;
    const size_t dst_y = *ry + dy;

    const char c = g.data[dst_x + dst_y * g.width];
    if(c != '.') {
        if(!attempt_push(g, dst_x, dst_y, dx, dy))
            return;
    }

    *rx = dst_x;
    *ry = dst_y;
    assert(*rx > 0);
    assert(*ry > 0);
    assert(*rx + 1 < g.width);
    assert(*ry + 1 < g.height);
}

void print_grid(FILE* f, const struct grid g, size_t rx, size_t ry) {
    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            if(i == rx && j == ry) {
                fprintf(f, "@");
            }else{
                fprintf(f, "%c", g.data[i + j * g.width]);
            }
        }
        fprintf(f, "\n");
    }
}
int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    struct grid g = { 0 };
    struct grid g2 = { 0 };
    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    size_t robot_x, robot_y;
    size_t robot_x2, robot_y2;
    bool map = true;
    size_t line_no = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0) {
            map = false;
            g.height = g.size / g.width;
            g2.height = g2.size / g2.width;
            assert(g2.height == g.height);
            continue;
        }


        if(map) {
            g.width = l.len;
            g2.width = l.len * 2;
            for(size_t i = 0; i < l.len; ++i) {
                const char c = l.data[i];
                if(c == '@') {
                    robot_x = i;
                    robot_y = line_no;
                    robot_x2 = i * 2;
                    robot_y2 = line_no;
                    da_append(g, '.');
                    da_append(g2, '.');
                    da_append(g2, '.');
                }else{
                    da_append(g, c);
                    if(c == '#' || c == '.') {
                        da_append(g2, c);
                        da_append(g2, c);
                    }else if(c == 'O') {
                        da_append(g2, '[');
                        da_append(g2, ']');
                    }

                }

            }
            ++line_no;
        }else{
            // print_grid(stdout, g2, robot_x2, robot_y2);
            for(size_t i = 0; i < l.len; ++i) {
                const char op = l.data[i];
                do_step(op, &robot_x, &robot_y, g);
                do_step(op, &robot_x2, &robot_y2, g2);
                // printf("Grid: %c\n", op);
                // print_grid(stdout, g2, robot_x2, robot_y2);
            }
        }
    }

    size_t task1 = 0;
    for(size_t j = 0; j < g.height; ++j) {
        for(size_t i = 0; i < g.width; ++i) {
            if(g.data[i + j * g.width] == 'O')
                task1 += 100 * j + i;
        }
    }
    size_t task2 = 0;
    for(size_t j = 0; j < g2.height; ++j) {
        for(size_t i = 0; i < g2.width; ++i) {
            if(g2.data[i + j * g2.width] == '[')
                task2 += 100 * j + i;
        }
    }

    printf("Task1: %lu\n", task1);
    printf("Task2: %lu\n", task2);

    free(g.data);
    free(line);
    return 0;
}


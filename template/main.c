#define AOC_IMPLEMENTATION
#include <aoc.h>


int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        // TODO: handle line
        printf("%.*s\n", (int) l.len, l.data);
    }

    free(line);
    return 0;
}


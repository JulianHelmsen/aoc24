#define AOC_IMPLEMENTATION
#include <aoc.h>

int not_digit(int c) {
    return !isdigit(c);
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    int accu = 0;
    int accu2 = 0;
    int factor = 1;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;


        while(l.len > 4) {
            struct sview original = l;
            (void) original;

            if(memcmp(l.data, "do()", 4) == 0) {
                factor = 1;
                l = sv_subview(l, 4, SVIEW_NPOS);
            }else if (memcmp(l.data, "don't()", 7) == 0) {
                factor = 0;
                l = sv_subview(l, 7, SVIEW_NPOS);
            }else if(memcmp(l.data, "mul(", 4) == 0) {
                l = sv_subview(l, 4, SVIEW_NPOS);
                int64_t end_idx = sv_find(l, not_digit);
                if(end_idx <= 0 || end_idx >= 4) {
                    continue;
                }
                struct sview first_num = sv_subview(l, 0, end_idx);
                l = sv_subview(l, end_idx, SVIEW_NPOS);
                if(sv_empty(l) || l.data[0] != ',')
                    continue;
                l = sv_subview(l, 1, SVIEW_NPOS);

                end_idx = sv_find(l, not_digit);
                if(end_idx <= 0 || end_idx >= 4) {
                    continue;
                }
                struct sview second_num = sv_subview(l, 0, end_idx);
                l = sv_subview(l, end_idx, SVIEW_NPOS);
                if(sv_empty(l) || l.data[0] != ')')
                    continue;
                l = sv_subview(l, 1, SVIEW_NPOS);
                // printf("%.*s: %.*s * %.*s\n", (int) original.len, original.data, (int) first_num.len, first_num.data, (int) second_num.len, second_num.data);

                const long v1 = strtol(first_num.data, NULL, 10);
                const long v2 = strtol(second_num.data, NULL, 10);
                if(errno == ERANGE) {
                    perror("strtol");
                    exit(1);
                }
                accu2 += v1 * v2 * factor;
                accu += v1 * v2;
            }else{
                l = sv_subview(l, 1, SVIEW_NPOS);
            }
        }

    }
    printf("task1: %d\n", accu);
    printf("task2: %d\n", accu2);

    free(line);
    return 0;
}


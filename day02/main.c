#define AOC_IMPLEMENTATION
#include <aoc.h>
const int dec = 1;
const int inc = 2;
const int both = dec | inc;

struct Items {
    int* data;
    size_t size;
    size_t capacity;
};

bool is_safe(const int* vals, size_t n) {
    int flags = 0;

    for(size_t i = 1; i < n; ++i) {
        const int v1 = vals[i - 1];
        const int v2 =  vals[i];

        int diff = v2 - v1;
        if(diff > 0) {
            flags |= inc;
        }else if(diff < 0) {
            flags |= dec;
            diff = -diff;
        }else{
            return 0; // neither inc or dec
        }

        if(diff > 3) {
            return 0;
        }

        if(flags == both) {
            return 0;
        }
    }
    return flags;
}



bool is_safe2(const int* vals, size_t n) {
    for(size_t ign = 0; ign <= n; ++ign) {
        int flags = 0;
        bool failed = false;
        size_t prev_idx = ign == 0 ? 1 : 0;
        size_t curr_idx = prev_idx + 1;
        if(curr_idx == ign)
            ++curr_idx;
        assert(prev_idx < curr_idx);
        while(curr_idx < n) {
            assert(prev_idx < curr_idx);
            const int v1 = vals[prev_idx];
            const int v2 = vals[curr_idx];

            int diff = v2 - v1;
            if(diff > 0) {
                flags |= inc;
            }else if(diff < 0) {
                flags |= dec;
                diff = -diff;
            }else{
                failed = true;
                break;
            }

            if(diff > 3) {
                failed = true;
                break;
            }

            if(flags == both) {
                failed = true;
                break;
            }

            // skip
            ++prev_idx;
            ++curr_idx;
            if(prev_idx == ign) ++prev_idx;
            if(curr_idx == ign) ++curr_idx;
        }
        if(!failed)
            return true;
    }
    return false;
}


int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    int task1 = 0;
    int task2 = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_trim_left(sv_create_with_len(line, len));

        if(l.len == 0)
            continue;

        // handle line
        struct Items items = { 0 };
        while(!sv_empty(l)) {
            struct sview num;
            sv_split(l, ' ', &num, &l);
            int v = atoi(num.data);
            da_append(items, v);
        }

        if(is_safe(items.data, items.size)) {
            task1++;
        }

        if(is_safe2(items.data, items.size)) {
            task2++;
        }

        free(items.data);
    }
    free(line);

    printf("Task1: %d\n", task1);
    printf("Task2: %d\n", task2);

    return 0;
}


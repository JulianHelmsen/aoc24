#define AOC_IMPLEMENTATION
#include <aoc.h>




uint64_t task1(char* disk, size_t n) {
    uint64_t checksum = 0;

    size_t offset = 0;
    size_t last_idx = n & 1 ? n - 1 : n - 2;
    size_t first_idx = 0;

    while(first_idx <= last_idx) {
        assert(last_idx % 2 == 0);
        if(first_idx & 1) {
            // empty blocks
            // move 1 of the last blocks to this position
            if(disk[first_idx] > 0) {
                disk[first_idx]--; // one gap less
                disk[last_idx]--; // one block in last file less
                size_t fid = last_idx / 2;
                checksum += fid * offset;
                offset++;

                if(disk[last_idx] == 0) {
                    last_idx -= 2;
                }
            }else{
                first_idx++;
            }
        }else{
            // its a file
            if(disk[first_idx] >= 0) {
                size_t fid = first_idx / 2;
                checksum += fid * offset;
                offset++;
                disk[first_idx]--;
                if(disk[first_idx] == 0) {
                    first_idx++;
                }
            }


        }
    }
    // first == last
    return checksum;
}

uint64_t task2(char* disk, size_t n) {
    uint64_t checksum = 0;
    size_t* offsets = calloc(sizeof(size_t), n);
    if(offsets == NULL) {
        perror("calloc");
        exit(1);
    }

    size_t last_idx = n & 1 ? n - 1 : n - 2;

    // calculate offsets of empty space
    size_t offset = 0;
    for(size_t i = 0; i < n; ++i) {
        offsets[i] = offset;
        offset += disk[i];
    }

    for(int64_t j = last_idx; j >= 0; j -= 2) {
        const size_t f_len = disk[j];
        const size_t fid = j / 2;

        // iterate over every empty sport
        bool found = false;
        for(size_t i = 1; i < (size_t) j; i += 2) {
            size_t gap = disk[i];
            if(gap >= f_len) {
                // place file here
                for(size_t k = 0; k < f_len; ++k) {
                    checksum += offsets[i] * fid;
                    ++offsets[i];
                }
                disk[i] -= f_len;
                found = true;
                break;
            }
        }
        
        if(!found) {
            // no spot to the left found
            // => place here
            for(size_t i = 0; i < f_len; ++i) {
                checksum += fid * offsets[j];
                offsets[j]++;
            }
        }
    }

    free(offsets);
    return checksum;
}


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

        // convert ascii to int
        for(size_t i = 0; i < l.len; ++i)
            line[i] -= '0';

        char* l2 = sv_copy(l);

        printf("task1: %lu\n", task1(line, l.len));
        printf("task2: %lu\n", task2(l2, l.len));
        free(l2);
    }

    free(line);
    return 0;
}


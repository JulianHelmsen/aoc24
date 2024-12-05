#define AOC_IMPLEMENTATION
#define _GNU_SOURCE
#include <aoc.h>

struct Ordering {
    unsigned char smaller;
    unsigned char larger;
};

struct OrderingList {
    size_t size;
    size_t capacity;
    struct Ordering* data;
};

struct Pages {
    unsigned char* data;
    size_t size;
    size_t capacity;
};

bool verify(const unsigned char* page, const size_t page_count, const struct Ordering* ordering, const size_t ordering_size) {
    for(size_t i = 0; i < page_count; ++i) {
        for(size_t j = i + 1; j < page_count; ++j) {
            // ordering(page[i]) < ordering(page[j]) => rule j|i does not exist
            for(size_t k = 0; k < ordering_size; ++k) {
                const struct Ordering o = ordering[k];
                if(o.smaller == page[j] && o.larger == page[i])
                    return false;
            }
        }
    }
    return true;
}

int compare(const void* a, const void* b, void* user_data) {
    const unsigned char l = *((const unsigned char*) a);
    const unsigned char r = *((const unsigned char*) b);
    const struct OrderingList* list = (const struct OrderingList*) user_data;
    for(size_t i = 0; i < list->size; ++i) {
        const struct Ordering o = list->data[i];
        if(l == o.smaller && r == o.larger)
            return -1;
        else if(l == o.larger && r == o.smaller)
            return 1;
    }
    return 0;
}

void sort(unsigned char* pages, size_t page_count, struct OrderingList ordering) {
    qsort_r(pages, page_count, sizeof(unsigned char), compare, &ordering);
}

int main(const int argc, const char** argv) {
    (void) argc;
    (void) argv;

    char* line = NULL;
    ssize_t len;
    size_t n = 0;
    struct OrderingList list = { 0 };


    int task1 = 0;
    int task2 = 0;
    while((len = getline(&line, &n, stdin)) != -1) {
        if(line[len - 1] == '\n')
            line[--len] = '\0';

        struct sview l = sv_create_with_len(line, len);

        if(l.len == 0)
            continue;

        struct sview left, right;
        if(sv_split(l, '|', &left, &right) != -1) {
            const unsigned char vl = strtol(left.data, NULL, 10);
            const unsigned char vr = strtol(right.data, NULL, 10);
            if(errno == ERANGE) {
                fprintf(stderr, "strtol: '%s' for line '%.*s'\n", strerror(errno), (int) l.len, l.data);
                exit(1);
            }
            const struct Ordering ordering = (struct Ordering) {vl, vr};
            da_append(list, ordering);
        }else if(!sv_empty(l)) {
            // update
            //
            struct Pages pages = { 0 };

            struct sview num;
            while(!sv_empty(l)) {
                sv_split(l, ',', &num, &l);
                const unsigned char page = strtol(num.data, NULL, 10);
                if(errno == ERANGE) {
                    fprintf(stderr, "strtol: '%s' for line '%.*s'\n", strerror(errno), (int) l.len, l.data);
                    exit(1);
                }
                da_append(pages, page);
            }

            if(verify(pages.data, pages.size, list.data, list.size)) {
                task1 += pages.data[pages.size / 2];
            }else{
                sort(pages.data, pages.size, list);
                task2 += pages.data[pages.size / 2];
            }
            free(pages.data);
        }
    }
    free(line);
    free(list.data);

    printf("task1: %d\n", task1);
    printf("task2: %d\n", task2);

    return 0;
}


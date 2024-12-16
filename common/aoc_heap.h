#ifndef AOC_HEAP_H
#define AOC_HEAP_H

#include "aoc_da.h"

#define aoc_heap_parent(i) ((i) / 2)
#define aoc_heap_left(i) ((i) * 2)
#define aoc_heap_right(i) ((i) * 2 + 1)

#define heap_push(h, val, cmp, ctx) do{ \
        da_append((h), (val)); \
        da_append((h), (h).data[(h.size) - 1]); \
        const size_t AOC_HEAP_TMP_LOC = (h).size - 1; \
        --(h).size;\
        size_t aoc_heap_pos = (h).size; \
        while(aoc_heap_pos != 1) {\
            const size_t heap_parent_pos = aoc_heap_parent(aoc_heap_pos);\
            const int cmp_res = cmp((h).data[AOC_HEAP_TMP_LOC], (h).data[heap_parent_pos - 1], ctx);\
            if(cmp_res <= 0)\
                break;\
            (h).data[aoc_heap_pos - 1] = (h).data[heap_parent_pos - 1];\
            (h).data[heap_parent_pos - 1] = (h).data[AOC_HEAP_TMP_LOC];\
            aoc_heap_pos = aoc_heap_parent(aoc_heap_pos);\
        }\
    }while(0)

#define heap_pop(h, dst, cmp, ctx) do{\
        assert((h).size > 0);\
        assert(dst != NULL);\
        *(dst) = (h).data[0];\
        const size_t AOC_HEAP_TMP_LOC = (h).size - 1;\
        (h).data[0] = (h).data[--(h).size];\
        size_t heap_pos = 1;\
        while(aoc_heap_right(heap_pos) < (h).size) {\
            const int cmp_res = cmp((h).data[aoc_heap_left(heap_pos) - 1], (h).data[aoc_heap_right(heap_pos) - 1], ctx);\
            size_t smaller_pos;\
            if(cmp_res >= 0) { \
                smaller_pos = aoc_heap_left(heap_pos);\
            }else{\
                smaller_pos = aoc_heap_right(heap_pos);\
            }\
            const int fcmp = cmp((h).data[heap_pos - 1], (h).data[smaller_pos - 1], ctx);\
            if(fcmp >= 0) { \
                break;\
            }\
            (h).data[AOC_HEAP_TMP_LOC] = (h).data[heap_pos - 1];\
            (h).data[heap_pos - 1] = (h).data[smaller_pos - 1];\
            (h).data[smaller_pos - 1] = (h).data[AOC_HEAP_TMP_LOC];\
            heap_pos = smaller_pos;\
        }\
        if(aoc_heap_left(heap_pos) < (h).size) {\
            const int fcmp = cmp((h).data[heap_pos - 1], (h).data[aoc_heap_left(heap_pos) - 1], ctx);\
            if(fcmp >= 0) { \
                break;\
            }\
            (h).data[AOC_HEAP_TMP_LOC] = (h).data[heap_pos - 1];\
            (h).data[heap_pos - 1] = (h).data[aoc_heap_left(heap_pos) - 1];\
            (h).data[aoc_heap_left(heap_pos) - 1] = (h).data[AOC_HEAP_TMP_LOC];\
        }\
    }while(0)



#endif 

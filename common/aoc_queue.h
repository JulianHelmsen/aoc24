#ifndef MYLIBC_QUEUE_H
#define MYLIBC_QUEUE_H

#include <stddef.h>
#include <string.h>
#include <assert.h>

#ifndef MIN_QUEUE_CAPACITY
#define MIN_QUEUE_CAPACITY (10)
#endif 

#ifndef QUEUE_SCALING
#define QUEUE_SCALING (2)
#endif

#define QUEUE_MEMBERS(type) \
    size_t capacity; \
    size_t front; \
    size_t back; \
    type* items


#define queue_size(pq) ( \
        (pq)->front == (pq)->back ? 0 : ( \
            (pq)->front < (pq)->back ? (pq)->back - (pq)->front : \
            (pq)->capacity - (pq)->front + (pq)->back \
        ) \
    )

#define queue_full(pq) (queue_size(pq) + 1 >= (pq)->capacity)

#define queue_grow(pq) do { \
        const size_t new_cap = (pq)->capacity == 0 ? MIN_QUEUE_CAPACITY : (pq)->capacity * QUEUE_SCALING;\
        char* new_mem = malloc(new_cap * sizeof(*(pq)->items));\
        if(new_mem == NULL) {\
            perror("malloc");\
            exit(1);\
        }\
        if((pq)->back < (pq)->front) {\
            queue_tmemcpy(new_mem, (pq)->capacity, (pq)->items, (pq)->back, sizeof(*(pq)->items));\
            queue_tmemcpy(new_mem, (pq)->front, &(pq)->items[(pq)->front], (pq)->capacity - (pq)->front, sizeof(*(pq)->items));\
            (pq)->back = (pq)->capacity + (pq)->back;\
        }else{\
            queue_tmemcpy(new_mem, (pq)->front, &(pq)->items[(pq)->front], (pq)->back - (pq)->front, sizeof(*(pq)->items));\
        }\
        free((pq)->items);\
        (pq)->capacity = new_cap;\
        (pq)->items = (void*) new_mem;\
    }while(0)

#define queue_push_front(pq, val) do { \
        if(queue_full(pq)) queue_grow(pq);\
        assert(!queue_full((pq)));\
        if(--(pq)->front >= (pq)->capacity)\
            (pq)->front = (pq)->capacity - 1;\
        (pq)->items[(pq)->front] = (val);\
        assert((pq)->back != (pq)->front);\
    }while(0)

#define queue_push_back(pq, val) do { \
        if(queue_full(pq)) queue_grow(pq);\
        assert(!queue_full(pq));\
        (pq)->items[(pq)->back++] = (val);\
        if((pq)->back == (pq)->capacity)\
            (pq)->back = 0;\
        assert((pq)->back != (pq)->front);\
    }while(0)

#define queue_pop_front(pq) ( \
    (pq)->front = (pq)->front + 1 != (pq)->capacity ? (pq)->front + 1 : 0, \
        (pq)->front > 0 ? (pq)->items[(pq)->front - 1] : (pq)->items[(pq)->capacity - 1])


#define queue_pop_back(pq) ((pq)->back = (pq)->back - 1 < (pq)->capacity ? (pq)->back - 1 : (pq)->capacity - 1, \
        (pq)->items[(pq)->back])

#define queue_free(pq) do {\
        (pq)->front = 0;\
        (pq)->back = 0;\
        free((pq)->items);\
        (pq)->capacity = 0;\
        (pq)->items = NULL;\
    }while(0)


#define queue_front(pq) (pq)->items[(pq)->front]

#define queue_back(pq) ((pq)->back == 0 ? (pq)->items[(pq)->capacity - 1] : (pq)->items[(pq)->back - 1])

#define queue_empty(pq) ((pq)->front == (pq)->back)
// internal
inline static void queue_tmemcpy(void* restrict  dst, size_t d_off, const void* restrict src, size_t ncount, size_t elem_size) {
    char* cdst = dst;
    cdst += d_off * elem_size;
    memcpy(cdst, src, ncount * elem_size);
}

#endif // MYLIBC_QUEUE_H

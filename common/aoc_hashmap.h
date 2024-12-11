#ifndef AOC_HASHMAP_H
#define AOC_HASHMAP_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef HASHMAP_MIN_CAPACITY
#define HASHMAP_MIN_CAPACITY 1024
#endif

// WARNING: This only works for data types with fixed size. Strings therefore do not work.
size_t shash(const void* v);
size_t ihash(const void* v);
int ieq(const void* a, const void* b);
int seq(const void* a, const void* b);

typedef size_t(*hash_func)(const void* e);
typedef int (*eq_func)(const void* a, const void* b);


enum entry_state {
    EMPTY = 0, USED, TOMBSTONE
};

struct hashmap_entry {
    enum entry_state state;
};

struct hashmap {
    hash_func hash;
    eq_func eq;

    size_t size;
    size_t occupied;
    size_t capacity;
    size_t value_size;
    size_t key_size;

    char* data;
};


#define hashmap_create(hash, eq, key_type, value_type) hashmap_create_impl(hash, eq, sizeof(key_type), sizeof(value_type))


void hashmap_reserve(struct hashmap* map, size_t capacity);
void hashmap_insert(struct hashmap* map, const void* pk, void* v);
int hashmap_find(const struct hashmap* map, const void* pk, void* pv);
int hashmap_remove(struct hashmap* map, const void* pk);

#ifdef AOC_HASHMAP_IMPLEMENTATION

void hashmap_reserve(struct hashmap* map, size_t capacity) {
    assert(map->size == 0 && "Map must be empty to reserve");
    const size_t ent_size = sizeof(struct hashmap_entry) + map->key_size + map->value_size;
    map->capacity = capacity;
    map->data = calloc(ent_size, capacity);
    if(map->data == NULL) {
        perror("calloc");
        exit(1);
    }
}


struct hashmap hashmap_create_impl(hash_func hash, eq_func eq, size_t ksize, size_t vsize) {
    struct hashmap map;
    map.hash = hash;
    map.size = 0;
    map.occupied = 0;
    map.eq = eq;
    map.capacity = 0;
    map.value_size = vsize;
    map.key_size = ksize;
    map.data = NULL;
    return map;
}

void* hashmap_find_bucket(const struct hashmap* map, const void* k) {
    if(map->size == 0)
        return NULL;
    size_t hash;
    size_t it;
    char* addr;
    const struct hashmap_entry* ent;
    const void* key;
    const size_t ent_size = sizeof(struct hashmap_entry) + map->key_size + map->value_size;

    it = 0;
    while(1) {
        ++it;
        hash = (map->hash(k) + it + it * it) % map->capacity;
        
        addr = map->data + hash * ent_size;
        ent = (struct hashmap_entry*) addr;
        key = addr + sizeof(struct hashmap_entry);

        if(ent->state == EMPTY)
            break;
        if(ent->state == USED) {
            if(map->eq(k, key)) {
                return addr;
            }
        }
    } 

    return NULL;
    
}

void hashmap_resize(struct hashmap* map) {
    const size_t ent_size = sizeof(struct hashmap_entry) + map->key_size + map->value_size;
    size_t i;

    char* addr;
    struct hashmap_entry* ent;
    void* key;
    void* value;

    struct hashmap newmap = *map;
    newmap.capacity = newmap.capacity * 2;
    if(newmap.capacity < HASHMAP_MIN_CAPACITY)
        newmap.capacity = HASHMAP_MIN_CAPACITY;
    newmap.size = 0;
    newmap.data = calloc(ent_size, newmap.capacity);
    if(newmap.data == NULL) {
        perror("calloc");
        exit(1);
    }

    if(map->data != NULL) {
        for(i = 0; i < map->capacity; ++i) {
            addr = map->data + i * ent_size;
            ent = (struct hashmap_entry*) addr;
            if(ent->state == USED) {
                key = addr + sizeof(struct hashmap_entry);
                value = addr + sizeof(struct hashmap_entry) + map->key_size;

                hashmap_insert(&newmap, key, value);
            }
        }
    }

    free(map->data);
    *map = newmap;

}

void hashmap_insert(struct hashmap* map, const void* k, void* v) {
    char* bucket = (char*) hashmap_find_bucket(map, k);
    const size_t ent_size = sizeof(struct hashmap_entry) + map->key_size + map->value_size;
    size_t it;
    char* addr;
    void* key;
    void* value;
    size_t hash;
    struct hashmap_entry* ent;

    if(bucket != NULL) {
        void* val = bucket + sizeof(struct hashmap_entry) + map->key_size;
        memcpy(val, v, map->value_size);
        return;
    }

    if(2 * map->occupied >= map->capacity) {
        hashmap_resize(map);
    }

    it = 0;

    while(1) {
        ++it;
        hash = (map->hash(k) + it + it * it) % map->capacity;
        
        addr = map->data + hash * ent_size;
        ent = (struct hashmap_entry*) addr;

        if(ent->state == EMPTY || ent->state == TOMBSTONE) {
            ent->state = USED;
            key = addr + sizeof(struct hashmap_entry);
            value = addr + sizeof(struct hashmap_entry) + map->key_size;
            memcpy(key, k, map->key_size);
            memcpy(value, v, map->value_size);
            map->size++;
            map->occupied++;
            return;
        }
    } 

}

int hashmap_remove(struct hashmap* map, const void* k) {
    char* bucket = (char*) hashmap_find_bucket(map, k);
    if(bucket == NULL)
        return 0;
    struct hashmap_entry* ent = (struct hashmap_entry*) bucket;
    ent->state = TOMBSTONE;
    map->size--;
    return 1;
}

int hashmap_find(const struct hashmap* map, const void* k, void* v) {
    char* bucket = (char*) hashmap_find_bucket(map, k);
    if(bucket == NULL)
        return 0;

    void* val = bucket + sizeof(struct hashmap_entry) + map->key_size;
    memcpy(v, val, map->value_size);
    return 1;
}

size_t shash(const void* v) {
    return *(size_t*) v;
}

size_t ihash(const void* v) {
    return (size_t) (*(int*) v);
}

int ieq(const void* a, const void* b) {
    return memcmp(a, b, sizeof(int)) == 0;
}
int seq(const void* a, const void* b) {
    return memcmp(a, b, sizeof(size_t)) == 0;
}
#endif
#endif 

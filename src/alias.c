#include "alias.h"
#include <string.h>

// fnv-1a hash function: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
size_t hash(char* str) {
    // NB: Assuming 64-bit architecture.
    size_t hash = 0xCBF29CE484222325;
    u_int8_t c;
    while ((c = (u_int8_t) *str++) != '\0') {
        hash ^= c;
        hash *= 0x100000001B3;
    }
    return hash;
}

// Find a bucket with the given key or an empty bucket.
AliasBucket* find_bucket(AliasMap* map, char* key) {
    size_t index = hash(key) % map->capacity;
    AliasBucket* tombstone = NULL;

    while (true) {
        AliasBucket* bucket = &map->buckets[index];
        if (bucket->key == NULL) {
            if (bucket->value.size == 0) {
                // Empty bucket, so no need to keep looking.
                return tombstone != NULL ? tombstone : bucket;
            } else {
                // Tombstone, so keep looking.
                if (tombstone == NULL) {
                    tombstone = bucket;
                }
            }
        } else if (strcmp(bucket->key, key) == 0) {
            return bucket;
        }
        index = (index + 1) % map->capacity;
    }
}

AliasMap make_alias_map(void) {
    return (AliasMap){.size = 0, .capacity = 32, .buckets = calloc(32, sizeof(AliasBucket))};
}

void add_alias(AliasMap* map, char* key, TokenList value) {
    if (map->size >= map->capacity / 2) {
        // Over 50% full, we need to resize, which means we need to rehash.
        map->capacity = map->capacity == 0 ? 32 : map->capacity * 2;
        AliasMap temp_map = {
            .size = map->size, .capacity = map->capacity, .buckets = calloc(map->capacity, sizeof(AliasBucket))};

        for (size_t i = 0; i < map->capacity; i++) {
            if (map->buckets[i].key != NULL) {
                AliasBucket* bucket = find_bucket(&temp_map, map->buckets[i].key);
                bucket->key = map->buckets[i].key;
                bucket->value = map->buckets[i].value;
                bucket->tombstone = false;
            }
        }
        free(map->buckets);
        map->buckets = temp_map.buckets;
    }
    AliasBucket* bucket = find_bucket(map, key);
    if (bucket->key == NULL) {
        map->size++;
    }
    bucket->key = key;
    bucket->value = value;
    bucket->tombstone = false;
}

TokenList* get_alias(AliasMap* map, char* key) {
    AliasBucket* bucket = find_bucket(map, key);
    return bucket->key == NULL ? NULL : &bucket->value;
}

bool remove_alias(AliasMap* map, char* key) {
    AliasBucket* bucket = find_bucket(map, key);
    if (bucket->key == NULL) {
        return false;
    }
    free(bucket->key);
    free(bucket->value.tokens->start);
    free_token_list(&bucket->value);
    bucket->key = NULL;
    bucket->tombstone = true;
    map->size--;
    return true;
}

void free_alias_map(AliasMap* map) {
    free(map->buckets);
    map->size = 0;
    map->capacity = 0;
    map->buckets = NULL;
}
#pragma once

#include "token.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct AliasBucket {
    char* key;
    TokenList value;
    bool tombstone;
} AliasBucket;

/*
 * An open-addressed hash table with linear probing.
 */
typedef struct AliasMap {
    size_t size;
    size_t capacity;
    AliasBucket* buckets;
} AliasMap;

AliasMap make_alias_map(void);
void add_alias(AliasMap* map, char* key, TokenList value);
TokenList* get_alias(AliasMap* map, char* key);
bool remove_alias(AliasMap* map, char* key);
void free_alias_map(AliasMap* map);

#pragma once

#include "token.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct StringList {
    size_t size;
    size_t capacity;
    char** strings;
} StringList;

StringList make_string_list(void);
void add_string(StringList* list, char* string);
void free_string_list(StringList* list);

typedef struct AliasBucket {
    char* key;
    TokenList value;
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
void save_alias_map(AliasMap* map);
AliasMap load_alias_map(StringList* allocations);

void perform_alias_substitution(AliasMap* map, TokenList* tokens, AliasMap* seen_names);

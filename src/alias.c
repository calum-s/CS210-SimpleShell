#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "alias.h"
#include "token.h"

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
    free_token_list(&bucket->value);
    bucket->key = NULL;
    map->size--;
    return true;
}

void free_alias_map(AliasMap* map) {
    free(map->buckets);
    map->size = 0;
    map->capacity = 0;
    map->buckets = NULL;
}

void save_alias_map(AliasMap* map) {
    char file_path[256];
    snprintf(file_path, 256, "%s/shellconfig/.aliases", getenv("HOME"));

    FILE* file = fopen(file_path, "w");
    if (file == NULL) {
        perror("alias: failed to open alias file");
        return;
    }

    for (size_t i = 0; i < map->capacity; i++) {
        if (map->buckets[i].key != NULL) {
            fprintf(file, "%s ", map->buckets[i].key);
            for (size_t j = 0; j < map->buckets[i].value.size; j++) {
                fprintf(
                    file, "%.*s ", (int) map->buckets[i].value.tokens[j].length, map->buckets[i].value.tokens[j].start);
            }
            fprintf(file, "\n");
        }
    }
    fclose(file);
}

AliasMap load_alias_map(StringList* allocations) {
    AliasMap map = make_alias_map();
    char file_path[256];
    snprintf(file_path, 256, "%s/shellconfig/.aliases", getenv("HOME"));

    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        if (errno == ENOENT) {
            return map;
        }
        perror("alias: failed to open alias file");
        return map;
    }

    char* line = NULL;
    size_t line_size = 0;
    while (getline(&line, &line_size, file) != -1) {
        TokenList tokens = tokenize(line);
        if (tokens.size > 0) {
            char* key = malloc(tokens.tokens[0].length + 1);
            strncpy(key, tokens.tokens[0].start, tokens.tokens[0].length);
            key[tokens.tokens[0].length] = '\0';
            remove_token(&tokens, 0);

            if (tokens.size == 0) {
                fprintf(stderr, "alias: Corrupted alias file. Aborting early.\n");
                free(line);
                free(key);
                free_token_list(&tokens);
                fclose(file);
                return map;
            }

            add_alias(&map, key, tokens);
        }
        add_string(allocations, line);
        line = NULL;
    }
    free(line);

    fclose(file);
    return map;
}

void perform_alias_substitution(AliasMap* map, TokenList* tokens, AliasMap* seen_names) {
    if (tokens->size > 0) {
        bool substitution_success = true;
        while (substitution_success) {
            TokenList* aliased = NULL;
            char* token0 = malloc(tokens->tokens[0].length + 1);
            strncpy(token0, tokens->tokens[0].start, tokens->tokens[0].length);
            token0[tokens->tokens[0].length] = '\0';
            if ((aliased = get_alias(map, token0)) != NULL) {
                if (get_alias(seen_names, token0) != NULL) {
                    fprintf(stderr, "alias: Circular alias detected: %s was already expanded.\n", token0);
                    substitution_success = false;
                    free(token0);
                } else {
                    remove_token(tokens, 0);
                    for (size_t i = 0; i < aliased->size; i++) {
                        insert_token(tokens, aliased->tokens[i], i);
                    }
                    // We're using the map as a hash set, so we don't care about the value.
                    // NB: The empty TokenList does not allocate any memory, so we don't need to free it.
                    TokenList empty = make_token_list();
                    add_alias(seen_names, token0, empty);
                    substitution_success = true;
                }
            } else {
                substitution_success = false;
                free(token0);
            }
        }
    }
}

void add_string(StringList* list, char* string) {
    if (list->size == list->capacity) {
        list->capacity = list->capacity == 0 ? 32 : list->capacity * 2;
        list->strings = realloc(list->strings, list->capacity * sizeof(char*));
    }

    list->strings[list->size++] = string;
}

void free_string_list(StringList* list) {
    for (size_t i = 0; i < list->size; i++) {
        free(list->strings[i]);
    }
    free(list->strings);

    list->strings = NULL;
    list->size = 0;
    list->capacity = 0;
}

StringList make_string_list(void) { return (StringList){0}; }

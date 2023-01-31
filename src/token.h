#pragma once

#include <stdint.h>
#include <stdlib.h>

struct Token {
    char* start;
    size_t length;
};

typedef struct Token Token;

struct TokenList {
    Token *tokens;
    size_t size;
    size_t capacity;
};

typedef struct TokenList TokenList;

TokenList make_token_list();
void add_token(TokenList* list, Token token);
void remove_token(TokenList* list, size_t index);
void free_token_list(TokenList* list);
void print_token_list(TokenList* list);
TokenList tokenize(char* input);

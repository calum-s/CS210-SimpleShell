#include "token.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool is_delimeter(char c) {
    const char* delimeters = " \t\n;|&><";
    const char* i = delimeters;
    while (*i != '\0') {
        if (*i == c) {
            return true;
        }
        ++i;
    }
    return false;
}

TokenList tokenize(char* source) {
    TokenList list = make_token_list();
    char* current = source;
    char* start = source;

    while (*current != '\0') {
        if (is_delimeter(*current)) {
            if (current != start) {
                Token token;
                token.start = start;
                token.length = (size_t) (current - start);
                add_token(&list, token);
            }
            start = current + 1;
        }
        current++;
    }

    if (current != start) {
        Token token;
        token.start = start;
        token.length = (size_t) (current - start);
        add_token(&list, token);
    }
    return list;
}

TokenList make_token_list(void) {
    TokenList list;
    list.size = 0;
    list.capacity = 0;
    list.tokens = NULL;
    return list;
}

void add_token(TokenList* list, Token token) {
    if (list->size == list->capacity) {
        list->capacity = list->capacity == 0 ? 32 : list->capacity * 2;
        list->tokens = realloc(list->tokens, list->capacity * sizeof(Token));
    }
    list->tokens[list->size++] = token;
}

void insert_token(TokenList* list, Token token, size_t index) {
    assert(index <= list->size);
    if (list->size == list->capacity) {
        list->capacity = list->capacity == 0 ? 32 : list->capacity * 2;
        list->tokens = realloc(list->tokens, list->capacity * sizeof(Token));
    }
    for (size_t i = list->size; i > index; i--) {
        list->tokens[i] = list->tokens[i - 1];
    }
    list->tokens[index] = token;
    list->size++;
}

void remove_token(TokenList* list, size_t index) {
    if (index >= list->size) {
        return;
    }
    for (size_t i = index; i < list->size - 1; i++) {
        list->tokens[i] = list->tokens[i + 1];
    }
    list->size--;
}

void free_token_list(TokenList* list) {
    free(list->tokens);
    list->size = 0;
    list->capacity = 0;
    list->tokens = NULL;
}

void print_token_list(TokenList* list) {
    printf("[");
    for (size_t i = 0; i < list->size; i++) {
        printf("'%.*s',", (int) list->tokens[i].length, list->tokens[i].start);
    }
    printf("]\n");
}

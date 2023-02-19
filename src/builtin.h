#pragma once

#include <stdbool.h>

#include "token.h"

void builtin_cd(int argc, char** argv);
void builtin_exit(int argc, char** argv);
void builtin_getpath(int argc, char** argv);
void builtin_setpath(int argc, char** argv);

static const struct {
    char* name;
    size_t min_args;
    size_t max_args;
    void (*call)(int argc, char** argv);
} BUILTIN_TABLE[] = {
    {"cd", 1, 2, &builtin_cd},
    {"exit", 1, 1, &builtin_exit},
    {"getpath", 1, 1, &builtin_getpath},
    {"setpath", 2, 2, &builtin_setpath},
};

// Try to execute builtin, returning false if no such builtin was found.
bool try_execute_builtin(TokenList* token);

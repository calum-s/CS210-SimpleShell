#pragma once

#include <limits.h>
#include <stdbool.h>

#include "alias.h"
#include "file.h"
#include "token.h"

typedef struct BuiltinState {
    AliasMap aliases;
    AliasMap seen_names;
    CircularBuffer history;
    StringList allocations;
    bool exited;
} BuiltinState;

void builtin_cd(int argc, char** argv, BuiltinState*);
void builtin_exit(int argc, char** argv, BuiltinState*);
void builtin_getpath(int argc, char** argv, BuiltinState*);
void builtin_setpath(int argc, char** argv, BuiltinState*);
void builtin_alias(int argc, char** argv, BuiltinState*);
void builtin_unalias(int argc, char** argv, BuiltinState*);
void builtin_history(int argc, char** argv, BuiltinState*);
void builtin_historyinvoke(int argc, char** argv, BuiltinState*);

static const struct {
    char* name;
    size_t min_args;
    size_t max_args;
    void (*call)(int argc, char** argv, BuiltinState*);
} BUILTIN_TABLE[] = {
    {"cd", 1, 2, &builtin_cd},
    {"exit", 1, 1, &builtin_exit},
    {"getpath", 1, 1, &builtin_getpath},
    {"setpath", 2, 2, &builtin_setpath},
    {"alias", 1, SIZE_MAX, &builtin_alias},
    {"unalias", 2, 2, &builtin_unalias},
    {"history", 1, 1, &builtin_history},
    {"!", 2, 2, &builtin_historyinvoke},
    {"!!", 1, 1, &builtin_historyinvoke},
};

// Try to execute builtin, returning false if no such builtin was found.
bool try_execute_builtin(TokenList* token, BuiltinState* state);

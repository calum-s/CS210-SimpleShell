// For built-in commands

#include "builtin.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "token.h"

// Cmd's have to be listed in the same order found in builtin.h
const char* BUILTINS[] = {"cd", "exit", "getpath", "setpath"};

// Check if command is built-in
Builtin is_builtin(Token token) {
    for (size_t i = 0; i < sizeof(BUILTINS) / sizeof(char*); i++) {
        if (strncmp(token.start, BUILTINS[i], token.length) == 0) {
            return (Builtin) i;
        }
    }
    return CMD_NONE;
}

// Execute built-in command
void execute_builtin(Builtin builtin, TokenList* tokens) {
    char** args = (char**) malloc((tokens->size + 1) * sizeof(char*));
    for (size_t i = 0; i < tokens->size; i++) {
        Token token = tokens->tokens[i];

        args[i] = malloc(token.length + 1);
        strncpy(args[i], token.start, token.length);
        args[i][token.length] = '\0';
    }
    args[tokens->size] = NULL;

    switch (builtin) {
    case CMD_EXIT: {
        exit(0);
        break;
    }

    case CMD_CD: {
        if (tokens->size > 2) {
            fprintf(stderr, "Error: too many arguments passed\n");
            break;
        }

        if (chdir(tokens->size == 2 ? args[1] : getenv("HOME")) < 0) {
            if (errno == ENOENT) {
                fprintf(stderr, "cd: Path '%s' does not exist.\n", tokens->size == 2 ? args[1] : getenv("HOME"));
            } else {
                perror("cd");
            }
        };
        break;
    }
    case CMD_GETPATH: {
        char* path = getenv("PATH");
        printf("%s\n", path);
        break;
    }
    case CMD_SETPATH: {
        if (tokens->size > 2) {
            fprintf(stderr, "Error: too many arguments passed\n");
            break;
        }
        if (tokens->size == 2) {
            if (setenv("PATH", args[1], 1) < 0) {
                perror("setpath");
            };
        }
        break;
    }
    default: {
        fprintf(stderr, "Builtin command not found\n");
        abort();
    }
    }

    for (size_t i = 0; i < tokens->size; i++) {
        free(args[i]);
    }
    free(args);
}

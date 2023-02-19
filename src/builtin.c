// For built-in commands

#include "builtin.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "token.h"

bool try_execute_builtin(TokenList* tokens) {
    for (size_t i = 0; i < sizeof(BUILTIN_TABLE) / sizeof(BUILTIN_TABLE[0]); i++) {
        if (strncmp(BUILTIN_TABLE[i].name, tokens->tokens[0].start, tokens->tokens[0].length) == 0) {
            // Is a builtin

            if (tokens->size > BUILTIN_TABLE[i].max_args || tokens->size < BUILTIN_TABLE[i].min_args) {
                bool word = tokens->size > BUILTIN_TABLE[i].max_args;
                fprintf(stderr,
                        "%s: Too %s arguments. Expected at %s %zu.\n",
                        BUILTIN_TABLE[i].name,
                        word ? "many" : "few",
                        word ? "most" : "least",
                        word ? BUILTIN_TABLE[i].max_args - 1 : BUILTIN_TABLE[i].min_args - 1);
                return true;
            }

            // Convert the args to the array of null terminated char*s. We do
            // this so that the args can be used easily with syscalls.
            char** args = (char**) malloc((tokens->size + 1) * sizeof(char*));
            for (size_t j = 0; j < tokens->size; j++) {
                Token token = tokens->tokens[j];

                args[j] = malloc(token.length + 1);
                strncpy(args[j], token.start, token.length);
                args[j][token.length] = '\0';
            }
            args[tokens->size] = NULL;

            BUILTIN_TABLE[i].call((int) tokens->size, args);

            for (size_t j = 0; j < tokens->size; j++) {
                free(args[j]);
            }
            free(args);
            return true;
        }
    }
    return false;
}
// Try to execute builtin, returning false if no such builtin was found.
void builtin_cd(int argc, char** argv) {
    if (chdir(argc == 2 ? argv[1] : getenv("HOME")) < 0) {
        if (errno == ENOENT) {
            fprintf(stderr, "cd: Path '%s' does not exist.\n", argc == 2 ? argv[1] : getenv("HOME"));
        } else {
            perror("cd");
        }
    };
}

void builtin_getpath(int argc, char** argv) {
    (void) argc, (void) argv;
    char* path = getenv("PATH");
    printf("%s\n", path);
}

void builtin_setpath(int argc, char** argv) {
    malloc(1023);
    (void) argc;
    if (setenv("PATH", argv[1], 1) < 0) {
        perror("setpath");
    }
}

void builtin_exit(int argc, char** argv) {
    (void) argc, (void) argv;
    exit(0);
}

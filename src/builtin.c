// For built-in commands

#include "builtin.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "alias.h"
#include "command.h"
#include "file.h"
#include "token.h"

bool try_execute_builtin(TokenList* tokens, BuiltinState* state) {
    for (size_t i = 0; i < sizeof(BUILTIN_TABLE) / sizeof(BUILTIN_TABLE[0]); i++) {
        if (strlen(BUILTIN_TABLE[i].name) == tokens->tokens[0].length &&
            strncmp(BUILTIN_TABLE[i].name, tokens->tokens[0].start, tokens->tokens[0].length) == 0) {
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

            BUILTIN_TABLE[i].call((int) tokens->size, args, state);

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
void builtin_cd(int argc, char** argv, BuiltinState* state) {
    (void) state;
    if (chdir(argc == 2 ? argv[1] : getenv("HOME")) < 0) {
        if (errno == ENOENT) {
            fprintf(stderr, "cd: Path '%s' does not exist.\n", argc == 2 ? argv[1] : getenv("HOME"));
        } else {
            perror("cd");
        }
    }
}

void builtin_getpath(int argc, char** argv, BuiltinState* state) {
    (void) argc, (void) argv, (void) state;
    char* path = getenv("PATH");
    printf("%s\n", path);
}

void builtin_setpath(int argc, char** argv, BuiltinState* state) {
    (void) argc, (void) state;
    if (setenv("PATH", argv[1], 1) < 0) {
        perror("setpath");
    }
}

void builtin_exit(int argc, char** argv, BuiltinState* state) {
    (void) argc, (void) argv;
    state->exited = true;
}

void builtin_alias(int argc, char** argv, BuiltinState* state) {
    if (argc == 2) {
        fprintf(stderr, "alias: Expected either 0 or 2+ arguments, got 1.\n");
        return;
    }

    if (argc == 1) {
        if (state->aliases.size == 0) {
            printf("No aliases defined.\n");
            return;
        }
        for (size_t i = 0; i < state->aliases.capacity; i++) {
            AliasBucket* alias = &state->aliases.buckets[i];

            if (alias->key == NULL)
                continue;

            printf("%s=", alias->key);
            for (size_t j = 0; j < alias->value.size; j++) {
                printf("%.*s ", (int) alias->value.tokens[j].length, alias->value.tokens[j].start);
            }
            printf("\n");
        }
        return;
    }

    // FIXME: Pointless retokenization because we don't have access to the original
    //        TokenList to copy.
    char* key = malloc(strlen(argv[1]) + 1);
    strcpy(key, argv[1]);

    TokenList* old_alias;
    if ((old_alias = get_alias(&state->aliases, key))) {
        fprintf(stderr, "alias: [warn] overwriting old alias for '%s': ", key);
        for (size_t i = 0; i < old_alias->size; i++) {
            fprintf(stderr, "%.*s ", (int) old_alias->tokens[i].length, old_alias->tokens[i].start);
        }
        fprintf(stderr, "\n");

        remove_alias(&state->aliases, key);
    }

    size_t buffer_capacity = 64;
    size_t buffer_size = 0;
    char* value = calloc(buffer_capacity, sizeof(char));

    for (int i = 2; i < argc; i++) {
        size_t arg_size = strlen(argv[i]);
        while (buffer_size + arg_size + 2 > buffer_capacity) {
            buffer_capacity *= 2;
            value = realloc(value, buffer_capacity);
        }

        strcat(value, argv[i]);
        strcat(value, " ");
        buffer_size += arg_size + 1;
    }

    value[buffer_size - 1] = '\0';
    add_alias(&state->aliases, key, tokenize(value));
    add_string(&state->allocations, value);
}

void builtin_unalias(int argc, char** argv, BuiltinState* state) {
    (void) argc;
    if (!remove_alias(&state->aliases, argv[1])) {
        fprintf(stderr, "unalias: No such alias '%s'\n", argv[1]);
    }
}

void builtin_history(int argc, char** argv, BuiltinState* state) {
    (void) argc, (void) argv, (void) state;
    print_history(&state->history);
}

void builtin_historyinvoke(int argc, char** argv, BuiltinState* state) {
    (void) argc, (void) argv;

    long input;

    if (argv[1] == NULL) {
        input = (long) state->history.count;
    } else {
        char* end_ptr;
        long parsed = strtol(argv[1], &end_ptr, 10);

        if (errno == ERANGE || errno == EINVAL) {
            perror("history");
            return;
        } else if (*end_ptr != '\0') {
            fprintf(stderr, "history: Invalid input '%s'.\n", argv[1]);
            return;
        } else if (parsed < 0) {
            input = (long) state->history.count + parsed + 1;
            if (input <= 0) {
                fprintf(stderr, "history: Input %ld not in range. \n", parsed);
                return;
            }
        } else if (parsed == 0 || parsed > (long) state->history.count) {
            fprintf(stderr, "history: Input %ld not in range. \n", parsed);
            return;
        } else {
            input = parsed;
        }
    }

    Command* command;
    if ((command = get_command(&state->history, (size_t) input - 1)) == NULL) {
        fprintf(stderr, "history: Error finding command\n");
        return;
    }
    TokenList tokens = tokenize(command->name);
    assert(tokens.size > 0);

    if (get_alias(&state->seen_names, tokens.tokens[0].start) != NULL) {
        fprintf(stderr,
                "history: [warn] Recursive history invocation detected, %s was already seen.\n",
                tokens.tokens[0].start);
        free_token_list(&tokens);
        return;
    } else {
        char* name = strdup(tokens.tokens[0].start);
        add_alias(&state->seen_names, name, make_token_list());
    }

    printf("%s\n", command->name);

    perform_alias_substitution(&state->aliases, &tokens, &state->seen_names);

    if (!try_execute_builtin(&tokens, state)) {
        start_external(&tokens);
    }
    free_token_list(&tokens);
}

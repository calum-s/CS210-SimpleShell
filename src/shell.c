// Project: Simple Shell
// Authors: Kyle Pereria, Calum Scott, Karim Moukaouame, Max Hagan
// Date: 31/01/2023

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"
#include "file.h"
#include "token.h"

int main(void) {
    // SHELL EXECUTION SPEC

    struct sigaction act = {
        .sa_handler = sigint,
        .sa_flags = 0,
    };
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);

    char* home = getenv("HOME");

    // Set current working directory to home variable
    chdir(home);

    char* old_path = malloc(strlen(getenv("PATH")) + 1);
    strcpy(old_path, getenv("PATH"));

    // creation of buffers for file names

    char history_file[100];
    char aliases_file[100];
    snprintf(history_file, 100, "%s%s", home, "/shellconfig/.hist_list");
    snprintf(aliases_file, 100, "%s%s", home, "/shellconfig/.aliases");
    open_file(history_file);
    open_file(aliases_file);

    StringList allocations = make_string_list();
    BuiltinState state = {
        .allocations = make_string_list(),
        .aliases = load_alias_map(&allocations),
        .exited = false,
        .history = load_circular_buffer(history_file),
    };
    state.allocations = allocations;

    while (!state.exited) {
        // Get current working path
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("ss: getcwd:");
            strncpy(cwd, "?", sizeof(cwd));
        }

        // Get current user
        char* usr = getlogin();

        // Print prompt
        printf("%s $ %s > ", usr, cwd);

        // Read user input
        char* input = malloc(64);
        size_t input_size = 64;
        size_t offset = 0;
        bool force_continue = false;

        while (1) {
            if (fgets(input + offset, (int) (input_size - offset), stdin) == NULL) {
                if (!feof(stdin)) {
                    // Happens on SIGINT which should cause line to be
                    // discarded.
                    printf("\n");
                } else {
                    // Happens on EOF which should cause shell to exit.
                    printf("\n");
                    state.exited = true;
                }
                force_continue = true;
                break;
            }

            offset = strlen(input);
            if (input[offset - 1] != '\n' && !feof(stdin)) {
#ifdef CONFORMANT
                // Spec sheet requires max input of 512 bytes.
                if (input_size >= 512) {
                    fprintf(stderr, "ss: input too long\n");

                    // Discard input until newline.
                    while (fgets(input, (int) input_size, stdin) != NULL) {
                        if (input[strlen(input) - 1] == '\n') {
                            break;
                        }
                    }

                    force_continue = true;
                    break;
                }
#endif
                input_size *= 2;
                input = realloc(input, input_size);
            } else {
                break;
            }
        }

        if (force_continue) {
            free(input);
            continue;
        }

        TokenList tokens = tokenize(input);
        if (tokens.size == 0) {
            free_token_list(&tokens);
            free(input);
            continue;
        }

        state.seen_names = make_alias_map();

        perform_alias_substitution(&state.aliases, &tokens, &state.seen_names);

        // Strip trailing newline
        input[strlen(input) - 1] = '\0';
        if (*tokens.tokens[0].start != '!') {
            add_to_circular_buffer(&state.history, input);
        }

        if (!try_execute_builtin(&tokens, &state)) {
            start_external(&tokens);
        }

        for (size_t i = 0; i < state.seen_names.capacity; i++) {
            if (state.seen_names.buckets[i].key != NULL) {
                free(state.seen_names.buckets[i].key);
            }
        }
        free_alias_map(&state.seen_names);

        free_token_list(&tokens);
        free(input);
    }

    write_to_file(&state.history,
                  history_file); // here so that it remembers the command even if it is not builtin / valid

    save_alias_map(&state.aliases);
    for (size_t i = 0; i < state.aliases.capacity; i++) {
        if (state.aliases.buckets[i].key != NULL) {
            free(state.aliases.buckets[i].key);
            free_token_list(&state.aliases.buckets[i].value);
        }
    }
    free_alias_map(&state.aliases);
    free_string_list(&state.allocations);

    setenv("PATH", old_path, 1);
    printf("ss: Restored PATH to %s\n", old_path);
    free(old_path);

    return 0;
}

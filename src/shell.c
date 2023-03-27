// Project: Simple Shell
// Authors: Kyle Pereria, Calum Scott, Karim Moukaouame, Max Hagan, Peter King
// Date: 31/01/2023

#include <errno.h>
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

    // TODO: Find the user home directory from the environment
    char* home = getenv("HOME");

    // Set current working directory to home variable
    chdir(home);

    // TODO: Save the current path

    // TODO: Load history + aliases

    BuiltinState state = {
        .aliases = make_alias_map(),
    };

    // creation of buffers for file names

    char historyFile[100];
    char aliasesFile[100];
    snprintf(historyFile, 100, "%s%s", home, "/shellconfig/history.txt");
    snprintf(aliasesFile, 100, "%s%s", home, "/shellconfig/aliases.txt");
    open_file(historyFile);
    open_file(aliasesFile);
    CircularBuffer bufferFile = init_buffer_from_file(historyFile);

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
                    force_continue = true;
                    break;
                }
                exit(0);
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

        write_to_circular_buffer(&bufferFile, tokens.tokens[0].start);

        char* token0 = malloc(tokens.tokens[0].length + 1);
        strncpy(token0, tokens.tokens[0].start, tokens.tokens[0].length);
        token0[tokens.tokens[0].length] = '\0';

        TokenList* aliased;
        if ((aliased = get_alias(&state.aliases, token0)) != NULL) {
            remove_token(&tokens, 0);
            for (size_t i = 0; i < aliased->size; i++) {
                insert_token(&tokens, aliased->tokens[i], i);
            }
        }

        free(token0);

        if (!try_execute_builtin(&tokens, &state)) {
            start_external(&tokens);
        }

        // TODO: While the command is a history invocation or alias then replace
        // it with the appropriate command from history or the aliased command
        // respectively

        // TODO: If command is built-in invoke appropriate function

        free_token_list(&tokens);
        free(input);
    }

    // TODO: Save history
    write_to_file(historyFile, &bufferFile); // here so that it remembers the command even if it is not builtin / valid

    // TODO: Save aliases

    // TODO: Restore original path

    return 0;
}

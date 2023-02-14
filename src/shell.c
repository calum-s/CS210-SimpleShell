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

    // TODO: Load history

    // TODO: Load aliases

    while (1) {
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
            if (fgets(input + offset, (int) (input_size - offset), stdin) ==
                NULL) {
                if (!feof(stdin)) {
                    // Happens on SIGINT which should cause line to be
                    // discarded.
                    printf("\n");
                    free(input);
                    force_continue = true;
                    break;
                }
                free(input);
                exit(0);
            }

            offset = strlen(input);
            if (input[offset - 1] != '\n' && !feof(stdin)) {
                input_size *= 2;
                input = realloc(input, input_size);
            } else {
                break;
            }
        }
        if (force_continue) continue;

        TokenList tokens = tokenize(input);
        if (tokens.size == 0) {
            free_token_list(&tokens);
            free(input);
            continue;
        }

        Builtin cmd;
        if ((cmd = is_builtin(tokens.tokens[0])) != CMD_NONE) {
            execute_builtin(cmd, &tokens);
        } else {
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

    // TODO: Save aliases

    // TODO: Restore original path

    return 0;
}

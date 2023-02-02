// Project: Simple Shell
// Authors: Kyle Pereria, Calum Scott, Karim Moukaouame, Max Hagan, Peter King
// Date: 31/01/2023

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"

int main(void) {

    // SHELL EXECUTION SPEC

    // TODO: Find the user home directory from the environment
    char* home = getenv("HOME");

    // Set current working directory to home variable
    chdir(home);

    // TODO: Save the current path

    // TODO: Load history

    // TODO: Load aliases

    while(1) {
    // TODO: Do while shell has not terminated
        // TODO: Display prompt
        printf("> ");

        // TODO: Read and parse user input

        // Read user input
        char* input = malloc(64);
        size_t input_size = 64;
        size_t offset = 0;

        while (1) {
            if (fgets(input + offset, (int) (input_size - offset), stdin) == NULL) {
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

        TokenList tokens = tokenize(input);
        if (tokens.size == 0) {
            continue;
        }
        if (tokens.size == 1 && strncmp(tokens.tokens[0].start, "exit", 4) == 0) {
            free_token_list(&tokens);
            free(input);
            exit(0);
        }

        print_token_list(&tokens);

        // TODO: While the command is a history invocation or alias then replace it with the
        // appropriate command from history or the aliased command respectively

        // TODO: If command is built-in invoke appropriate function
        // TODO: Else execute command as an external process
        
        free_token_list(&tokens);
        free(input);
    }

    // TODO: Save history

    // TODO: Save aliases

    // TODO: Restore original path

    return 0;
}

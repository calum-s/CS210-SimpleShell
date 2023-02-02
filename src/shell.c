// Project: Simple Shell
// Authors: Kyle Pereria, Calum Scott, Karim Moukaouame, Max Hagan, Peter King
// Date: 31/01/2023

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

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
        char input[1024];
        if (fgets(input, 1024, stdin) == NULL) {
            exit(0);
        }


        // TODO: While the command is a history invocation or alias then replace it with the
        // appropriate command from history or the aliased command respectively

        // TODO: If command is built-in invoke appropriate function
        // TODO: Else execute command as an external process
    }

    // TODO: Save history

    // TODO: Save aliases

    // TODO: Restore original path

    return 0;
}

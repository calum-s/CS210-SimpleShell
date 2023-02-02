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

    printf("Home: %s \n", home);

    // TODO: Save the current path

    // TODO: Load history + aliases

    // creation of buffers for file names
    char historyFile[100];
    char aliasesFile[100];

    sprintf(historyFile, "%s/shellconfig/history.txt", home); // concat of home and file name
    sprintf(aliasesFile, "%s/shellconfig/aliases.txt", home);

    int history = openFile(historyFile);
    int aliases = openFile(aliasesFile);


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
            if (fgets(input + offset, (int) (input_size - offset), stdin) == NULL) {
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

                    free(input);
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

        if (force_continue)
            continue;

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

int openFile(const char* fileName) {
    FILE* file = fopen(fileName, "r"); // opens the file in read mode

    if (file == NULL) { // check if the file is valid/exists
        int check;
        char* dirname = "shellconfig"; // set the directory name for first-time creation
 
        check = mkdir(dirname,0777);

        if (!check) // check if the directory was created
            printf("Directory created \n");
        else {
            FILE* file = fopen(fileName, "a"); // create the file if the directory was created already
            return 0;
        }

        printf("Error opening file \n");
        return 0;
    }

    // read file contents
    char line[100];
    
    while (fgets(line, sizeof(line), file)) { // read the file line by line and print it (testing)
        printf("%s", line);
    }


    fclose(file);
    return 0;
}


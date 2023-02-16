#include "command.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void start_external(TokenList* token_list) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("ss: start_external: fork");
        return;
    }

    if (pid == 0) {
        // exec wants a list of null terminated strings, so we need to
        // re-allocate
        char** args = (char**) malloc((token_list->size + 1) * sizeof(char*));
        for (size_t i = 0; i < token_list->size; i++) {
            Token token = token_list->tokens[i];

            args[i] = malloc(token.length + 1);
            strncpy(args[i], token.start, token.length);
            args[i][token.length] = '\0';
        }
        // The list of strings is also null terminated
        args[token_list->size] = NULL;

        if (execvp(args[0], args) < 0) {
            if (errno == ENOENT) {
                fprintf(stderr, "ss: unknown command: '%s'\n", args[0]);
            } else {
                perror("ss: start_external: execvp");
            }
            for (size_t i = 0; i < token_list->size; i++) { free(args[i]); }
            free(args);
            exit(1);
        }
    } else {
        int stat;
        while (wait(&stat) < 0) {
            if (errno == EINTR) {
                // signal will be passed to child, we should continue waiting.
                continue;
            }
            perror("ss: start_external: wait");
            break;
        }
    }
}

void sigint(__attribute__((unused)) int _signum) {
    // Shells are not terminated by Ctrl+C
}

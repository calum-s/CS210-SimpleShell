// For built-in commands

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "builtin.h"
#include "token.h"

const char* BUILTINS[] = {
    "cd",
    "exit"
};

// Check if command is built-in
Builtin is_builtin(Token token) {
    for(size_t i = 0; i < sizeof(BUILTINS) / sizeof(char*); i++){
        if (strncmp(token.start, BUILTINS[i], token.length) == 0){
            return i;
        }
    }
    return CMD_NONE;
}

// Execute built-in command
void execute_builtin(Builtin builtin, TokenList* token) {
    switch (builtin){
        case CMD_EXIT: {
            exit(0);
            break;
        }

        default: {
            fprintf(stderr,"dumbass");
            abort();
        }
    }
}
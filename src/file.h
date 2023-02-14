#pragma once

#include "token.h"

typedef struct Command {
    // struct Command *next;
    char* commandName;
    int commandNumber;
} command;

void openFile(const char* fileName);
void addToFile(const char* fileName, char* token);
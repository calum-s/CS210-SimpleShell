#pragma once

#include "token.h"

#define MAX_COMMAND_NAME_LENGTH 100

typedef struct Command {
    // struct Command *next;
    char commandName[MAX_COMMAND_NAME_LENGTH];
    int commandNumber;
} command;

void openFile(const char* fileName);
void writeToFile(const char* fileName, const char* commandName);
command readFromFile(FILE* fileName);
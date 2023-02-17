#pragma once

#include <stdio.h>

#include "token.h"

#define MAX_COMMAND_NAME_LENGTH 100

struct Command {
    char commandName[MAX_COMMAND_NAME_LENGTH];
    int commandNumber;
};

typedef struct Command Command;

void openFile(const char* fileName);
void writeToFile(const char* fileName, const char* commandName);
Command readFromFile(FILE* fileName);
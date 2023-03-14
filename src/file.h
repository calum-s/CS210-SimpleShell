#pragma once

#include <stdio.h>

#include "token.h"

#define MAX_COMMAND_NAME_LENGTH 512

struct Command {
    char commandName[MAX_COMMAND_NAME_LENGTH];
    int commandNumber;
};

typedef struct Command Command;

void open_file(const char* fileName);
void write_to_file(const char* fileName, const char* commandName);
Command read_from_file(FILE* fileName);

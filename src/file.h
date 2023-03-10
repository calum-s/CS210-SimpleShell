#pragma once

#include <stdio.h>

#include "token.h"

#define MAX_COMMAND_NAME_LENGTH 512
#define MAX_NUM_COMMANDS 20

struct Command {
    char commandName[MAX_COMMAND_NAME_LENGTH];
    int commandNumber;
};

typedef struct Command Command;

struct Command history[MAX_NUM_COMMANDS];

void open_file(const char* fileName);
void write_to_file(char* fileName, char* commandName);
Command read_from_file(FILE* fileName);
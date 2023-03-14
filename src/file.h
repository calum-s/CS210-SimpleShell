#pragma once

#include <stdio.h>

#include "token.h"

#define MAX_COMMAND_NAME_LENGTH 512
#define MAX_NUM_COMMANDS 20

struct Command {
    char commandName[MAX_COMMAND_NAME_LENGTH];
    int commandNumber;
};

struct CircularBuffer {
    struct Command buffer[MAX_NUM_COMMANDS];
    int start;
    int end;
    int count;
};

typedef struct Command Command;
typedef struct CircularBuffer CircularBuffer;

void open_file(const char* fileName);
void write_to_file(const char* fileName, const CircularBuffer* buffer);
Command read_from_file(FILE* fileName);
CircularBuffer init_buffer_from_file(char* fileName);
void write_to_circular_buffer(CircularBuffer* buffer, char* commandName);

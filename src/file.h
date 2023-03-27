#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "token.h"

#define MAX_COMMAND_NAME_LENGTH 512
#define MAX_NUM_COMMANDS 20

struct Command {
    char name[MAX_COMMAND_NAME_LENGTH];
};
typedef struct Command Command;

struct CircularBuffer {
    Command buffer[MAX_NUM_COMMANDS];
    size_t start;
    size_t count;
};
typedef struct CircularBuffer CircularBuffer;

void open_file(char* fileName);
Command* get_command(CircularBuffer* buffer, size_t command_number);
void add_to_circular_buffer(CircularBuffer* buffer, char* command_name);
CircularBuffer load_circular_buffer(char* file_name);

bool read_from_file(FILE* file, Command* command);
void write_to_file(CircularBuffer* buffer, char* file_name);
void print_history(CircularBuffer* buffer);

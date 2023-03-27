#include "file.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

bool read_from_file(FILE* file, Command* command) {
    size_t cmd_num;
    return fscanf(file, "%zu %[^\n]", &cmd_num, command->name) == 2;
}

void print_history(CircularBuffer* buffer) {
    for (size_t i = 0; i < buffer->count; i++) {
        Command* command = get_command(buffer, i);
        printf("%zu %s\n", i + 1, command->name);
    }
}

Command* get_command(CircularBuffer* buffer, size_t index) {
    if (index >= buffer->count) {
        return NULL;
    }

    return &buffer->buffer[(buffer->start + index) % MAX_NUM_COMMANDS];
}

void open_file(char* fileName) {
    FILE* file = fopen(fileName, "r");

    if (file == NULL) { // check if the file is valid/exists
        char dirname[100];
        snprintf(dirname,
                 100,
                 "%s%s",
                 getenv("HOME"),
                 "/shellconfig"); // set the directory name for first-time creation in working directory

        if (mkdir(dirname, 0775) < 0 && errno != EEXIST) { // check if the directory was created
            perror("mkdir");
            return;
        }

        file = fopen(fileName, "a"); // creates file if it does not already exist
    }

    fclose(file);
}

void write_to_file(CircularBuffer* buffer, char* file_name) {
    FILE* file = fopen(file_name, "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    for (size_t i = 0; i < buffer->count; i++) { // iterates over every command and writes it to the file
        Command* command = get_command((CircularBuffer*) buffer, i);
        fprintf(file, "%zu %s\n", i + 1, command->name);
    }

    fclose(file);
}

void add_to_circular_buffer(CircularBuffer* buffer, char* command_name) {
    // Add the new element to the end of the buffer
    Command new_command;
    strncpy(new_command.name, command_name, MAX_COMMAND_NAME_LENGTH - 1);
    buffer->buffer[(buffer->start + buffer->count) % MAX_NUM_COMMANDS] = new_command;
    if (buffer->count == MAX_NUM_COMMANDS) {
        buffer->start = (buffer->start + 1) % MAX_NUM_COMMANDS;
    } else {
        buffer->count++;
    }
}

CircularBuffer load_circular_buffer(char* file_name) { // initializes the buffer from the file
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        perror("Error opening file\n");
        return (CircularBuffer){0};
    }

    CircularBuffer buffer = {0};
    Command command;

    while (read_from_file(file, &command)) {
        add_to_circular_buffer(&buffer, command.name);
    }

    fclose(file);

    return buffer;
}

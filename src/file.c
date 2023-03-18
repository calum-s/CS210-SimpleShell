#include "file.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// TODO: Detect command history and store in struct in incremements of 1.

// Commands are accessed in the format of the command !<number> where <number>
// is the command number in the history. If the command number is not found, the
// shell should print an error message and return to the prompt. for reading
// from the file the program should iterate over the elements in the struct, and
// return the string values (command itself) struct format is as follows: {int
// commandNumber, char* command}

Command read_from_file(FILE* file) {
    Command command;
    if (fscanf(file, "%d %[^\n]", &command.commandNumber, command.commandName) == 2) {
        return command;
    } else {
        command.commandNumber = 0;
        return command;
    }
}

void open_file(const char* fileName) {
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

void write_to_file(const char* fileName, const CircularBuffer* buffer) {
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    for (int i = 0; i < buffer->count; i++) { // iterates over every command and writes it to the file
        Command command = buffer->buffer[(buffer->start + i) % MAX_NUM_COMMANDS];
        fprintf(file, "%d %s\n", command.commandNumber, command.commandName);
    }

    fclose(file);
}

void write_to_circular_buffer(CircularBuffer* buffer, char* commandName) {
    if (buffer->count == MAX_NUM_COMMANDS) { // checks if the buffer is full

        for (int i = 1; i < MAX_NUM_COMMANDS; i++) { // if the buffer is full, shift the commands back one position
            buffer->buffer[i - 1] = buffer->buffer[i];
            buffer->buffer[i - 1].commandNumber--;
        }
        buffer->end--;
        if (buffer->end < 0) {
            buffer->end = MAX_NUM_COMMANDS - 1;
        }
        buffer->count--;
    }

    // Add the new element to the end of the buffer
    Command newCommand;
    snprintf(newCommand.commandName, MAX_COMMAND_NAME_LENGTH, "%s", commandName);
    newCommand.commandNumber = buffer->count + 1;
    buffer->buffer[buffer->end] = newCommand;
    buffer->end = (buffer->end + 1) % MAX_NUM_COMMANDS;
    buffer->count++;
}

CircularBuffer init_buffer_from_file(char* fileName) { // initializes the buffer from the file
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error opening file\n");
        return (CircularBuffer){0};
    }
    
    CircularBuffer buffer = {0};
    Command command;

    while (fscanf(file, "%d %s\n", &command.commandNumber, command.commandName) == 2) {
        // Add the command to the circular buffer
        if (buffer.count == MAX_NUM_COMMANDS) {
            // If the buffer is full, overwrite the oldest element
            buffer.start = (buffer.start + 1) % MAX_NUM_COMMANDS;
            buffer.count--;
        }
        buffer.buffer[buffer.end] = command;
        buffer.end = (buffer.end + 1) % MAX_NUM_COMMANDS;
        buffer.count++;
    }

    fclose(file);

    // for(int i = 0; i < MAX_NUM_COMMANDS; i++) {
    //     printf("buffer[%d]: %d %s \n", i, buffer.buffer[i].commandNumber, buffer.buffer[i].commandName);
    // }

    return buffer;
}
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

void write_to_file(char* fileName, char* commandName) {
    
    FILE* file = fopen(fileName, "r+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }

    // Read in all commands from the file
    Command commands[MAX_NUM_COMMANDS];
    int commandCount = 0;
    Command command;

    while ((command = read_from_file(file)).commandNumber != 0) {
        commands[commandCount++] = command;
    }

    // Shift all commands up by one position if the maximum number of commands has been reached
    if (commandCount == MAX_NUM_COMMANDS) {
        for (int i = 1; i < MAX_NUM_COMMANDS; i++) {
            commands[i - 1] = commands[i];
            commands[i - 1].commandNumber--;
        }
        commandCount--;
    }

    // Add the new command to the end of the list
    Command newCommand;
    strncpy(newCommand.commandName, commandName, MAX_COMMAND_NAME_LENGTH);
    newCommand.commandNumber = commandCount + 1;
    commands[commandCount++] = newCommand;

    // Write all commands back to the file
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < commandCount; i++) {
        Command command = commands[i];
        fprintf(file, "%d %s\n", command.commandNumber, command.commandName);
    }

    fclose(file);
}

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

    if (file == NULL) {                // check if the file is valid/exists
        char* dirname = "shellconfig"; // set the directory name for first-time creation

        if (mkdir(dirname, 0775) < 0 && errno != EEXIST) { // check if the directory was created
            perror("mkdir");
            return;
        }
        file = fopen(fileName, "a");
    }

    Command allCommands;

    while ((allCommands = read_from_file(file)).commandNumber != 0) {
        printf("Read struct num %d from file.\n", allCommands.commandNumber);
        printf("Name: %s\n", allCommands.commandName);
    }

    fclose(file);
}

void write_to_file(const char* fileName, const char* commandName) {
    FILE* infile = fopen(fileName, "r+");

    if (infile == NULL) {
        printf("Error opening file\n");
        return;
    }

    int commandNumber = 1;

    if (infile != NULL) {
        // Read the last command number from the file
        while (read_from_file(infile).commandNumber != 0) {
            commandNumber++;
        }
        fclose(infile);
    }

    if (commandNumber > 20) {
        printf("Error: Commands file at capacity, cannot add to file (do "
               "something with this?) \n");
        return;
    }

    fprintf(infile, "%d %s\n", commandNumber, commandName);
    printf("Wrote struct num %d to file.\n", commandNumber);

    fclose(infile);
}

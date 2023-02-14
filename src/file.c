#include "file.h"

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

command readFromFile(FILE* file) {
    command allCommands;
    int numFieldsRead = fscanf(file, "%d %[^\n]", &allCommands.commandNumber,
                               allCommands.commandName);

    if (numFieldsRead != 2) {
        allCommands.commandNumber = 0;
    }

    return allCommands;
}

void openFile(const char* fileName) {
    FILE* file = fopen(fileName, "r");

    if (file == NULL) {  // check if the file is valid/exists
        int check;
        char* dirname =
            "shellconfig";  // set the directory name for first-time creation

        check = mkdir(dirname, 0777);

        if (!check) {  // check if the directory was created
            printf("Directory created \n");
            return;
        } else {
            FILE* file = fopen(
                fileName,
                "a");  // create the file if the directory was created already
            fclose(file);
            return;
        }
    }

    command allCommands;
    while ((allCommands = readFromFile(file)).commandNumber != 0) {
        printf("Read struct num %d from file.\n", allCommands.commandNumber);
        printf("Name: %s\n", allCommands.commandName);
    }

    fclose(file);
}

void writeToFile(const char* fileName, const char* commandName) {
    FILE* infile = fopen(fileName, "r");
    int commandNumber = 1;

    if (infile != NULL) {
        // Read the last command number from the file
        command lastCommand = readFromFile(infile);
        while (lastCommand.commandNumber != 0) {
            commandNumber = lastCommand.commandNumber + 1;
            lastCommand = readFromFile(infile);
        }
        fclose(infile);
    }

    FILE* outfile = fopen(fileName, "a");
    if (outfile == NULL) {
        printf("Error opening file.\n");
        return;
    }

    fprintf(outfile, "%d %s\n", commandNumber, commandName);
    printf("Wrote struct num %d to file.\n", commandNumber);

    fclose(outfile);
}

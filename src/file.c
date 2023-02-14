#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "file.h"

//TODO: Detect command history and store in struct in incremements of 1.

// Commands are accessed in the format of the command !<number> where <number> is the command number in the history.
// If the command number is not found, the shell should print an error message and return to the prompt.
// for reading from the file the program should iterate over the elements in the struct, and return the string values (command itself)
// struct format is as follows: {int commandNumber, char* command}

void openFile(const char* fileName) {
    FILE* file = fopen(fileName, "r"); // opens the file in read mode
    struct Command commands;

    if (file == NULL) { // check if the file is valid/exists
        int check;
        char* dirname = "shellconfig"; // set the directory name for first-time creation
 
        check = mkdir(dirname,0777);

        if (!check) // check if the directory was created
            printf("Directory created \n");
        else {
            FILE* file = fopen(fileName, "a"); // create the file if the directory was created already
            fclose(file);
            return;
        }

        return;
    }

    int result;
    command *new = malloc(sizeof(commands));
    result = fread(new, sizeof(*new), 1, file);

        if (1 != result) {
            if (feof(file)) {
                return;
            }
            perror("Could not read from file");
            return;
        }

        printf("Read struct num %d from file.\n", new->commandNumber);
        // printf("Name: %s\n", new->commandName);

    fclose(file);
    return;
}

void addToFile(const char* fileName, char* token) {
    FILE* outfile = fopen(fileName, "a");
    command allCommands = {token, allCommands.commandNumber + 1};
     
    // write struct to file
    fwrite (&allCommands, sizeof(command), 1, outfile);

    fclose(outfile);

    printf("wrote to file \n");

}
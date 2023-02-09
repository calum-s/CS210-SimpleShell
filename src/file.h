#pragma once

#include "token.h"

struct Command {
    char* commandName;
    int commandNumber;
};

int openFile(const char* fileName);
void addToFile(char fileName, char* token);
#pragma once
#include "token.h"

enum Builtin {
    CMD_CD,
    CMD_EXIT,
    CMD_GETPATH,
    CMD_NONE

};
typedef enum Builtin Builtin;

Builtin is_builtin(Token token);
void execute_builtin(Builtin builtin, TokenList* token);
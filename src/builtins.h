#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"
#include <string>

void handle_echo(const Command& cmd);
void handle_type(const Command& cmd);
void handle_pwd(const Command& cmd);
void handle_cd(const Command& cmd);
void handle_exit(const Command& cmd);

#endif
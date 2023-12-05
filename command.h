#ifndef COMMAND_H
#define COMMAND_H

#include "parser.h"

const char* handle_ping_command(const char* argument);
const char* handle_set_command(const char *key, const char *value);
const char* handle_get_command(const char *key);
const char* handle_del_command(Command cmd);

#endif // COMMAND_H

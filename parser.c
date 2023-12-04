#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"


Command parse_command(const char *input) {
    Command cmd;
    char command[256];

    if (sscanf(input, "%s %s %s", command, cmd.key, cmd.value) < 1) {
        cmd.type = CMD_UNKNOWN;
        return cmd;
    }

    if (strcmp(command, "PING") == 0) {
        cmd.type = CMD_PING;
    } else if (strcmp(command, "SET") == 0) {
        cmd.type = CMD_SET;
    } else if (strcmp(command, "GET") == 0) {
        cmd.type = CMD_GET;
    } else if (strcmp(command, "DEL") == 0) {
        cmd.type = CMD_DEL;
    } else {
        cmd.type = CMD_UNKNOWN;
    }

    return cmd;
}
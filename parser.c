#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"


Command parse_command(const char *input) {
    Command cmd;
    memset(&cmd, 0, sizeof(Command));
    char *line = strtok(input, "\r\n");
    while (line != NULL) {
        if (line[0] == '*') {
        } else if (line[0] == '$') {
        } else {
            if (cmd.type == CMD_UNKNOWN) {
                if (strcmp(line, "PING") == 0) {
                    cmd.type = CMD_PING;
                }
                //
            } else {
                strncpy(cmd.argument, line, sizeof(cmd.argument) - 1);
            }
        }
        line = strtok(NULL, "\r\n");
    }

    return cmd;
}

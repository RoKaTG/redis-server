#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"


Command parse_command(const char *input) {
    Command cmd;
    memset(&cmd, 0, sizeof(Command));

    char *line = strtok(input, "\r\n");
    int line_count = 0;
    
    while (line != NULL) {
        if (line[0] == '*') {
        } else if (line[0] == '$') {
        } else {
            line_count++;
            if (line_count == 1) {
                if (strcmp(line, "PING") == 0) cmd.type = CMD_PING;
            } else if (line_count == 2) {
                strncpy(cmd.argument, line, sizeof(cmd.argument) - 1);
            }
        }
        line = strtok(NULL, "\r\n");
    }
    if (cmd.type == CMD_PING && line_count == 1) {
        memset(cmd.argument, 0, sizeof(cmd.argument));
    }

    return cmd;
}


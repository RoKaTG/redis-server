#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

Command parse_command(const char *input) {
    Command cmd;
    memset(&cmd, 0, sizeof(Command));

    char *line = strtok(input, "\r\n");
    int line_count = 0;
    int key_index = 0;

    while (line != NULL) {
        if (line[0] == '*') {
            sscanf(line, "*%d", &cmd.num_keys);
        } else if (line[0] == '$') {
        } else {
            line_count++;
            if (line_count == 1) {
                if (strcmp(line, "PING") == 0) {
                    cmd.type = CMD_PING;
                } else if (strcmp(line, "SET") == 0) {
                    cmd.type = CMD_SET;
                } else if (strcmp(line, "GET") == 0) {
                    cmd.type = CMD_GET;
                } else if (strcmp(line, "DEL") == 0) {
                    cmd.type = CMD_DEL;
                } else if (strcmp(line, "EXISTS") == 0) {
                    cmd.type = CMD_EXISTS;
                } else { 
                    cmd.type = CMD_UNKNOWN;
                }
            }
            else if (line_count == 2 && cmd.type == CMD_PING) {
                strncpy(cmd.argument, line, sizeof(cmd.argument) - 1);
            } else if (line_count == 2 && (cmd.type == CMD_SET || cmd.type == CMD_GET)) {
                strncpy(cmd.key, line, sizeof(cmd.key) - 1);
            } else if (line_count == 3 && cmd.type == CMD_SET) {
                strncpy(cmd.value, line, sizeof(cmd.value) - 1);
            } else if (cmd.type == CMD_DEL && key_index < MAX_KEYS) {
                strncpy(cmd.keys[key_index], line, sizeof(cmd.keys[0]) - 1);
                key_index++;
            } else if (cmd.type == CMD_EXISTS && key_index < MAX_KEYS) {
                strncpy(cmd.keys[key_index], line, sizeof(cmd.keys[0]) - 1);
                key_index++;
            }
        }
        line = strtok(NULL, "\r\n");
    }

    if (cmd.type == CMD_DEL || cmd.type == CMD_EXISTS) {
        cmd.num_keys = key_index;
    }

    return cmd;
}

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    CMD_PING,
    CMD_SET,
    CMD_GET,
    CMD_DEL,
    CMD_UNKNOWN
} CommandType;

typedef struct {
    CommandType type;
    char key[256];
    char value[256];
} Command;

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

int main() {
    char input[512];

    strcpy(input, "SET key1 value1");

    Command cmd = parse_command(input);
    printf("Command: %d, Key: %s, Value: %s\n", cmd.type, cmd.key, cmd.value);

    return 0;
}

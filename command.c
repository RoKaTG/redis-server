#include <string.h>
#include <stdio.h>

#include "command.h"


const char* handle_ping_command(const char* argument) {
    static char response[256];

    if (argument == NULL || strcmp(argument, "") == 0) {
        strcpy(response, "PONG");
    } else {
        snprintf(response, sizeof(response), "%s", argument);
    }

    return response;
}

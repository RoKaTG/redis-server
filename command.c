#include <string.h>
#include <stdio.h>

#include "command.h"


const char* handle_ping_command(const char* argument) {
    static char response[1024];

    if (argument == NULL || strcmp(argument, "") == 0) {
        strcpy(response, "+PONG\r\n");
    } else {
        snprintf(response, sizeof(response), "$%zu\r\n%s\r\n", strlen(argument), argument);
    }

    return response;
}

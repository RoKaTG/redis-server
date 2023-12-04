#include <string.h>
#include <stdio.h>

#include "command.h"
#include "hashmap.h"

extern hashmap *global_map;


const char* handle_ping_command(const char* argument) {
    static char response[1024];
    if (argument == NULL || strcmp(argument, "") == 0) {
        strcpy(response, "+PONG\r\n");
    } else {
        snprintf(response, sizeof(response), "$%zu\r\n%s\r\n", strlen(argument), argument);
    }

    return response;
}

const char* handle_set_command(const char *key, const char *value) {
    static char response[256];

    hashmap_set(global_map, key, value);

    strcpy(response, "+OK\r\n");
    return response;
}

const char* handle_get_command(const char *key) {
    static char response[1024];

    char *value = hashmap_get(global_map, key);
    if (value) {
        snprintf(response, sizeof(response), "$%zu\r\n%s\r\n", strlen(value), value);
    } else {
        strcpy(response, "$-1\r\n");
    }
    return response;
}

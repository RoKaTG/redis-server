#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

/*const char* handle_del_command(Command cmd) {
    static char response[256];
    int count = 0;
    for (int i = 0; i < cmd.num_keys; ++i) {
        if (hashmap_remove(global_map, cmd.keys[i])) {
            count++;
        } else if (!hashmap_remove(global_map, cmd.keys[i]) && count > -1) {
            count--;
        }
    }
    snprintf(response, sizeof(response), ":%d\r\n", count + 1);
    return response;
}*/

const char* handle_del_command(Command cmd) {
    static char response[256];
    int count = 0;

    printf("Début de la suppression des clés. Nombre total de clés : %d\n", cmd.num_keys);

    for (int i = 0; i < cmd.num_keys; ++i) {
        printf("Tentative de suppression de la clé : %s\n", cmd.keys[i]);
        if (hashmap_remove(global_map, cmd.keys[i])) {
            count++;
            printf("Clé supprimée : %s\n", cmd.keys[i]);
        } else {
            printf("Clé non trouvée : %s\n", cmd.keys[i]);
        }
    }

    printf("Nombre total de clés supprimées : %d\n", count);
    snprintf(response, sizeof(response), ":%d\r\n", count);
    return response;
}

int handle_exists_command( Command cmd) {
    int count = 0;
    for (int i = 0; i < cmd.num_keys; ++i) {
        if (hashmap_get(global_map, cmd.keys[i]) != NULL) {
            count++;
        }
    }
    return count;
}

int handle_append_command(const char *key, const char *value) {
    char *existing_value = hashmap_get(global_map, key);
    size_t new_length = strlen(value) + (existing_value ? strlen(existing_value) : 0);

    char *new_value = malloc(new_length + 1);
    if (existing_value) {
        strcpy(new_value, existing_value);
    }
    strcat(new_value, value);

    hashmap_set(global_map, key, new_value);
    int appended_length = strlen(new_value);
    free(new_value);

    return appended_length;
}

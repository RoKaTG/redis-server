#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"

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

/*int handle_incr_command(const char *key) {
    char *existing_value = hashmap_get(global_map, key);
    long long int value = (existing_value) ? atoll(existing_value) : 0;
    value++;

    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", value);
    hashmap_set(global_map, key, new_value);

    return value;
}*/

/*const char* handle_incr_command(const char *key) {
    static char response[256];

    char *value = hashmap_get(global_map, key);
    if (value) {
        char *endptr;
        long val = strtol(value, &endptr, 10);
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }

        val++;
        snprintf(response, sizeof(response), ":%ld\r\n", val);

        char new_value[256];
        snprintf(new_value, sizeof(new_value), "%ld", val);
        hashmap_set(global_map, key, new_value);
    } else {
        hashmap_set(global_map, key, "1");
        strcpy(response, ":1\r\n");
    }

    return response;
}*/

const char* handle_incr_command(Command cmd) {
    char *value = hashmap_get(global_map, cmd.key);
    long long number;

    if (value && sscanf(value, "%lld", &number) == 1) {
        number++;
        char updated_value[256];
        snprintf(updated_value, sizeof(updated_value), "%lld", number);
        hashmap_set(global_map, cmd.key, updated_value);
        static char response[256];
        snprintf(response, sizeof(response), ":%lld\r\n", number);
        return response;
    } else if (!value) {
        hashmap_set(global_map, cmd.key, "1");
        return ":1\r\n";
    } else {
        return "-ERR value is not an integer or out of range\r\n";
    }
}

const char* handle_randomkey_command(hashmap *h) {
    static char response[256];
    char* key = get_random_key(h);

    if (key) {
        snprintf(response, sizeof(response), "$%zu\r\n%s\r\n", strlen(key), key);
    } else {
        strcpy(response, "$-1\r\n");
    }

    return response;
}

bool hashmap_is_empty(hashmap *h) {
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        if (h->entries[i] != NULL) {
            return false;
        }
    }
    return true;
}

char* get_random_key(hashmap *h) {
    if (hashmap_is_empty(h)) {
        return NULL;
    }

    srand(time(NULL));
    while (1) {
        int index = rand() % HASHMAP_SIZE;
        if (h->entries[index] != NULL) {
            return h->entries[index]->key;
        }
    }
}

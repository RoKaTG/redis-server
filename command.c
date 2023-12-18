#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"

extern hashmap *global_map;
extern expiration *expiration_map;


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

const char* handle_exists_command(Command cmd) {
    static char response[256];
    int count = 0;
    for (int i = 0; i < cmd.num_keys; ++i) {
        if (hashmap_get(global_map, cmd.keys[i]) != NULL) {
            count++;
        }
    }
    snprintf(response, sizeof(response), ":%d\r\n", count);
    return response;
}

const char* handle_append_command(const char *key, const char *value) {
    static char response[256];
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

    snprintf(response, sizeof(response), ":%d\r\n", appended_length);
    return response;
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

const char* handle_expire_command(const char *key, int seconds) {
    static char response[256];
    long long current_time_ms = time(NULL) * 1000LL;
    long long expiration_time_ms = current_time_ms + ((long long)seconds * 1000LL);

    if (hashmap_get(global_map, key) == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    expiration_map_set(expiration_map, key, expiration_time_ms);

    snprintf(response, sizeof(response), ":%d\r\n", 1);
    return response;
}


const char* handle_pexpire_command(const char *key, int ms) {
    static char response[256];
    long long current_time_ms = time(NULL) * 1000LL;
    long long expiration_time_ms = current_time_ms + ms;

    if (hashmap_get(global_map, key) == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    expiration_map_set(expiration_map, key, expiration_time_ms);

    snprintf(response, sizeof(response), ":%d\r\n", 1);
    return response;
}

const char* handle_persist_command(const char *key) {
    static char response[256];

    if (hashmap_get(global_map, key) == NULL || !expiration_map_exists(expiration_map, key)) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    expiration_map_remove(expiration_map, key);
    snprintf(response, sizeof(response), ":%d\r\n", 1);
    return response;
}

const char* handle_ttl_command(const char *key) {
    static char response[256];
    long long current_time_ms = time(NULL) * 1000LL;

    if (hashmap_get(global_map, key) == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", -2);
        return response;
    }

    long long expiration_time_ms = get_expiration_time_ms(expiration_map, key);
    if (expiration_time_ms == 0) {
        snprintf(response, sizeof(response), ":%d\r\n", -1);
        return response;
    }

    int ttl = (int)((expiration_time_ms - current_time_ms) / 1000LL);
    snprintf(response, sizeof(response), ":%d\r\n", ttl > 0 ? ttl : -1);
    return response;
}

const char* handle_pttl_command(const char *key) {
    static char response[256];
    long long current_time_ms = time(NULL) * 1000LL;

    if (hashmap_get(global_map, key) == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", -2);
        return response;
    }

    long long expiration_time_ms = get_expiration_time_ms(expiration_map, key);
    if (expiration_time_ms == 0) {
        snprintf(response, sizeof(response), ":%d\r\n", -1);
        return response;
    }

    int pttl = (int)(expiration_time_ms - current_time_ms);
    snprintf(response, sizeof(response), ":%d\r\n", pttl > 0 ? pttl : -1);
    return response;
}

int match_pattern(const char *pattern, const char *key) {
    while (*pattern && *key) {
        if (*pattern == '*') {
            return 1;
        }
        if (*pattern != '?' && *pattern != *key) {
            return 0;
        }
        pattern++;
        key++;
    }
    if (*pattern == '\0' && *key == '\0') {
        return 1;
    } else if (*pattern == '*' || *pattern == '\0') {
        return 1;
    }
    return 0;
}

const char* handle_keys_command(const char *pattern) {
    static char response[10240]; 
    char *response_ptr = response;
    int count = 0;

    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        hashmap_entry *entry = global_map->entries[i];
        while (entry != NULL) {
            if (strstr(entry->key, pattern) != NULL) {
                count++;
            }
            entry = entry->next;
        }
    }

    response_ptr += sprintf(response_ptr, "*%d\r\n", count);

    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        hashmap_entry *entry = global_map->entries[i];
        while (entry != NULL) {
            if (strstr(entry->key, pattern) != NULL) {
                response_ptr += sprintf(response_ptr, "$%zu\r\n%s\r\n", strlen(entry->key), entry->key);
            }
            entry = entry->next;
        }
    }

    return response;
}

const char* handle_rename_command(const char *key, const char *newkey) {
    static char response[256];

    char *value = hashmap_get(global_map, key);
    if (value == NULL) {
        strcpy(response, "-ERR no such key\r\n");
        return response;
    }

    hashmap_set(global_map, newkey, value);
    hashmap_remove(global_map, key);

    strcpy(response, "+OK\r\n");
    return response;
}

const char* handle_copy_command(const char *source, const char *destination) {
    static char response[256];

    char *value = hashmap_get(global_map, source);
    if (value == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    if (hashmap_get(global_map, destination) != NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    hashmap_set(global_map, destination, value);
    snprintf(response, sizeof(response), ":%d\r\n", 1);
    return response;
}

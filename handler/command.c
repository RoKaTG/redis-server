#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "../utils/utils.h"

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

const char* handle_incr_command(const char *key) {
    static char response[256];

    char *value = hashmap_get(global_map, key);
    long long number;

    if (value == NULL) {
        number = 0;
    } else {
        char *endptr;
        number = strtoll(value, &endptr, 10);
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }
    }

    number++;
    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", number);
    hashmap_set(global_map, key, new_value);

    snprintf(response, sizeof(response), ":%lld\r\n", number);
    return response;
}

const char* handle_decr_command(const char *key) {
    static char response[256];

    char *value = hashmap_get(global_map, key);
    long long number;

    if (value == NULL) {
        number = 0;
    } else {
        char *endptr;
        number = strtoll(value, &endptr, 10);
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }
    }

    number--;
    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", number);
    hashmap_set(global_map, key, new_value);

    snprintf(response, sizeof(response), ":%lld\r\n", number);
    return response;
}

const char* handle_incrby_command(const char *key, const char *increment) {
    static char response[256];

    char *value = hashmap_get(global_map, key);
    long long number;

    if (value == NULL) {
        number = 0;
    } else {
        char *endptr;
        number = strtoll(value, &endptr, 10);
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }
    }

    long long increment_value;
    char *endptr;
    increment_value = strtoll(increment, &endptr, 10);
    if (*endptr != '\0') {
        return "-ERR increment is not an integer or out of range\r\n";
    }

    number += increment_value;
    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", number);
    hashmap_set(global_map, key, new_value);

    snprintf(response, sizeof(response), ":%lld\r\n", number);
    return response;
}

const char* handle_decrby_command(const char *key, const char *increment) {
    static char response[256];

    char *value = hashmap_get(global_map, key);
    long long number;

    if (value == NULL) {
        number = 0;
    } else {
        char *endptr;
        number = strtoll(value, &endptr, 10);
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }
    }

    long long increment_value;
    char *endptr;
    increment_value = strtoll(increment, &endptr, 10);
    if (*endptr != '\0') {
        return "-ERR increment is not an integer or out of range\r\n";
    }

    number -= increment_value;
    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", number);
    hashmap_set(global_map, key, new_value);

    snprintf(response, sizeof(response), ":%lld\r\n", number);
    return response;
}

const char* handle_helper_command() {
    static char help_text[4096];

    const char* help_content =
        "Commands:\n"
        "PING: Teste la connectivité au serveur.\n"
        "SET key value: Définit la valeur d'une clé.\n"
        "GET key: Récupère la valeur d'une clé.\n"
        "DEL key [key ...]: Supprime une ou plusieurs clés.\n"
        "EXISTS key [key ...]: Vérifie si une ou plusieurs clés existent.\n"
        "APPEND key value: Ajoute une valeur à la fin de la clé.\n"
        "RANDOMKEY: Retourne une clé aléatoire.\n"
        "EXPIRE key seconds: Définit un délai d'expiration sur une clé.\n"
        "PEXPIRE key milliseconds: Définit un délai d'expiration sur une clé en millisecondes.\n"
        "PERSIST key: Supprime le délai d'expiration d'une clé.\n"
        "TTL key: Obtient le temps restant avant expiration d'une clé en secondes.\n"
        "PTTL key: Obtient le temps restant avant expiration d'une clé en millisecondes.\n"
        "KEYS pattern: Trouve toutes les clés correspondant au motif donné.\n"
        "RENAME key newkey: Renomme une clé.\n"
        "COPY source destination: Copie la valeur d'une clé vers une autre.\n"
        "INCR key: Incrémente la valeur numérique d'une clé.\n"
        "DECR key: Décrémente la valeur numérique d'une clé.\n"
        "INCRBY key increment: Incrémente la valeur numérique d'une clé par un montant donné.\n"
        "DECRBY key decrement: Décrémente la valeur numérique d'une clé par un montant donné.\n";

    // Calculez la longueur de help_content, y compris le caractère de fin de chaîne '\0'
    int help_content_length = strlen(help_content);

    // Format de la réponse en vrac (multi-string)
    snprintf(help_text, sizeof(help_text), "*2\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", help_content_length, help_content, help_content_length, help_content);

    return help_text;
}

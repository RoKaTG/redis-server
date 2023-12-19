#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../parser/parser.h"
#include "../struct/hashmap.h"
#include "../struct/expiration.h"
#include "../cJSON/cJSON.h"


void save_to_file(hashmap *h, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return;

    cJSON *root = cJSON_CreateObject();

    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        hashmap_entry *entry = h->entries[i];
        while (entry) {
            cJSON_AddStringToObject(root, entry->key, entry->value);
            entry = entry->next;
        }
    }

    char *json_data = cJSON_Print(root);
    fprintf(file, "%s", json_data);
    free(json_data);
    cJSON_Delete(root);
    fclose(file);
}


void load_from_file(hashmap *h, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *json_data = malloc(file_size + 1);
    if (!json_data) {
        fclose(file);
        return;
    }

    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0'; 

    cJSON *root = cJSON_Parse(json_data);
    cJSON *current_element = NULL;
    cJSON_ArrayForEach(current_element, root) {
        const char *key = current_element->string;
        const char *value = cJSON_GetStringValue(current_element);
        hashmap_set(h, key, value);
    }

    cJSON_Delete(root);
    free(json_data);
    fclose(file);
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
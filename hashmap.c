#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

#define HASHMAP_SIZE 1024

unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }
    return value % HASHMAP_SIZE;
}

hashmap* hashmap_create() {
    hashmap *h = malloc(sizeof(hashmap));
    h->entries = malloc(sizeof(hashmap_entry*) * HASHMAP_SIZE);

    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        h->entries[i] = NULL;
    }

    return h;
}

void hashmap_set(hashmap *h, const char *key, const char *value) {
    unsigned int slot = hash(key);
    hashmap_entry *entry = h->entries[slot];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            strncpy(entry->value, value, sizeof(entry->value));
            return;
        }
        entry = entry->next;
    }

    entry = malloc(sizeof(hashmap_entry));
    strncpy(entry->key, key, sizeof(entry->key));
    strncpy(entry->value, value, sizeof(entry->value));
    entry->next = h->entries[slot];
    h->entries[slot] = entry;
}

char* hashmap_get(hashmap *h, const char *key) {
    unsigned int slot = hash(key);
    hashmap_entry *entry = h->entries[slot];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void hashmap_free(hashmap *h) {
    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        hashmap_entry *entry = h->entries[i];
        while (entry != NULL) {
            hashmap_entry *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(h->entries);
    free(h);
}

int hashmap_remove(hashmap *h, const char *key) {
    unsigned int slot = hash(key);
    hashmap_entry *entry = h->entries[slot];
    hashmap_entry *prev = NULL;

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            if (prev == NULL) {
                h->entries[slot] = entry->next;
            } else {
                prev->next = entry->next;
            }
            free(entry);
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }
    return 0;
}

expiration* expiration_map_create() {
    expiration *map = malloc(sizeof(expiration));
    map->entries = malloc(sizeof(expiration_entry*) * HASHMAP_SIZE);
    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        map->entries[i] = NULL;
    }
    return map;
}

void expiration_map_set(expiration *map, const char *key, long long expiration_time) {
    int slot = hash(key);
    expiration_entry *entry = map->entries[slot];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            entry->expiration_time_ms = expiration_time;
            return;
        }
        entry = entry->next;
    }

    entry = malloc(sizeof(expiration_entry));
    strncpy(entry->key, key, sizeof(entry->key));
    entry->expiration_time_ms = expiration_time;
    entry->next = map->entries[slot];
    map->entries[slot] = entry;
}

void expiration_map_remove(expiration *map, const char *key) {
    int slot = hash(key);
    expiration_entry *entry = map->entries[slot];
    expiration_entry *prev = NULL;

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            if (prev == NULL) {
                map->entries[slot] = entry->next;
            } else {
                prev->next = entry->next;
            }
            free(entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

void check_and_remove_expired_keys(hashmap *h, expiration *map) {
    long long current_time_ms = time(NULL) * 1000LL;

    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        expiration_entry *entry = map->entries[i];
        expiration_entry *prev = NULL;

        while (entry != NULL) {
            if (entry->expiration_time_ms <= current_time_ms) {
                hashmap_remove(h, entry->key);

                if (prev == NULL) {
                    map->entries[i] = entry->next;
                } else {
                    prev->next = entry->next;
                }

                expiration_entry *temp = entry;
                entry = entry->next;
                free(temp);
            } else {
                prev = entry;
                entry = entry->next;
            }
        }
    }
}

void expiration_map_free(expiration *map) {
    if (map == NULL) return;

    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        expiration_entry *entry = map->entries[i];
        while (entry != NULL) {
            expiration_entry *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(map->entries);
    free(map);
}

int expiration_map_exists(expiration *map, const char *key) {
    int slot = hash(key);
    expiration_entry *entry = map->entries[slot];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

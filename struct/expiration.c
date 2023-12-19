#include <stdlib.h>
#include <string.h>

#include "expiration.h"
#include "hashmap.h"

/**
 * Create a new expiration map.
 *
 * @return A pointer to the newly created expiration map.
 */
expiration* expiration_map_create() {
    expiration *map = malloc(sizeof(expiration));
    map->entries = malloc(sizeof(expiration_entry*) * HASHMAP_SIZE);
    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        map->entries[i] = NULL;
    }
    return map;
}

/**
 * Set the expiration time for a given key in the expiration map.
 *
 * @param map The expiration map.
 * @param key The key to set expiration for.
 * @param expiration_time The expiration time in milliseconds.
 */
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

/**
 * Remove a key from the expiration map.
 *
 * @param map The expiration map.
 * @param key The key to remove.
 */
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

/**
 * Check and remove expired keys from the hashmap and expiration map.
 *
 * @param h The hashmap.
 * @param map The expiration map.
 */
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

/**
 * Free memory used by the expiration map.
 *
 * @param map The expiration map to free.
 */
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

/**
 * Check if a key exists in the expiration map.
 *
 * @param map The expiration map.
 * @param key The key to check.
 * @return 1 if the key exists, 0 otherwise.
 */
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

/**
 * Get the expiration time in milliseconds for a given key.
 *
 * @param map The expiration map.
 * @param key The key to get expiration time for.
 * @return The expiration time in milliseconds, or 0 if the key does not exist.
 */
long long get_expiration_time_ms(expiration *map, const char *key) {
    int slot = hash(key);
    expiration_entry *entry = map->entries[slot];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            return entry->expiration_time_ms;
        }
        entry = entry->next;
    }
    return 0;
}

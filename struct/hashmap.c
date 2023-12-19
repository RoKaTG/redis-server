#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

#define HASHMAP_SIZE 1024

/**
 * Hash function to calculate the slot index for a given key.
 *
 * @param key The input key.
 * @return The slot index in the hashmap.
 */
unsigned int hash(const char *key) {
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }
    return value % HASHMAP_SIZE;
}

/**
 * Create a new hashmap and initialize it.
 *
 * @return A pointer to the newly created hashmap.
 */
hashmap* hashmap_create() {
    hashmap *h = malloc(sizeof(hashmap));
    h->entries = malloc(sizeof(hashmap_entry*) * HASHMAP_SIZE);

    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        h->entries[i] = NULL;
    }

    return h;
}

/**
 * Set a key-value pair in the hashmap.
 *
 * @param h The hashmap.
 * @param key The key to set.
 * @param value The value to associate with the key.
 */
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

/**
 * Get the value associated with a key from the hashmap.
 *
 * @param h The hashmap.
 * @param key The key to look up.
 * @return The value associated with the key, or NULL if the key is not found.
 */
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

/**
 * Free the memory used by the hashmap.
 *
 * @param h The hashmap to free.
 */
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

/**
 * Remove a key-value pair from the hashmap.
 *
 * @param h The hashmap.
 * @param key The key to remove.
 * @return 1 if the key was removed, 0 if the key was not found.
 */
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

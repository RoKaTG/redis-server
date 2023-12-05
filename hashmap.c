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


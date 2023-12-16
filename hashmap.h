#ifndef HASHMAP_H
#define HASHMAP_H

#define HASHMAP_SIZE 1024

#include <time.h>

typedef struct hashmap_entry {
    char key[256];
    char value[256];
    struct hashmap_entry *next;
} hashmap_entry;


typedef struct {
    hashmap_entry **entries;
} hashmap;

typedef struct expiration_entry {
    char key[256];
    time_t expiration_time;
    struct expiration_entry *next;
} expiration_entry;

typedef struct {
    expiration_entry **entries;
} expiration;


hashmap* hashmap_create();
void hashmap_set(hashmap *h, const char *key, const char *value);
char* hashmap_get(hashmap *h, const char *key);
void hashmap_free(hashmap *h);
int hashmap_remove(hashmap *h, const char *key);

expiration* expiration_map_create();
void expiration_map_set(expiration *map, const char *key, time_t expiration_time);
void expiration_map_remove(expiration *map, const char *key);
void check_and_remove_expired_keys(hashmap *h, expiration *map);
void expiration_map_free(expiration *map);

#endif // HASHMAP_H


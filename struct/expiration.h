#pragma once

#include "hashmap.h"



/*****************STRUCT USED FOR TIME"STAMP" LIFE MANAGEMENT******************/
typedef struct expiration_entry {
    char key[256];
    long long expiration_time_ms;
    struct expiration_entry *next;
} expiration_entry;

typedef struct {
    expiration_entry **entries;
} expiration;


/*****************FUNCTION USED TO MANAGE THE LIFE TIME OF A KEY******************/
expiration* expiration_map_create();
void expiration_map_set(expiration *map, const char *key, long long expiration_time);
void expiration_map_remove(expiration *map, const char *key);
void check_and_remove_expired_keys(hashmap *h, expiration *map);
void expiration_map_free(expiration *map);
int expiration_map_exists(expiration *map, const char *key);

long long get_expiration_time_ms(expiration *map, const char *key);


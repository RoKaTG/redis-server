#pragma once

#include <time.h>

#define HASHMAP_SIZE 1024



/*****************STRUCT USED TO CREATE & MANAGE AN HASHMAP WITH OUR KEYs VALUEs******************/
typedef struct hashmap_entry {
    char key[256];
    char value[256];
    struct hashmap_entry *next;
} hashmap_entry;

typedef struct {
    hashmap_entry **entries;
} hashmap;


/*****************FUNCTION USED TO MANAGE & CREATE AN HASHMAP******************/
unsigned int hash(const char *key);
hashmap* hashmap_create();
void hashmap_set(hashmap *h, const char *key, const char *value);
char* hashmap_get(hashmap *h, const char *key);
void hashmap_free(hashmap *h);
int hashmap_remove(hashmap *h, const char *key);

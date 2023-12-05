#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct hashmap_entry {
    char key[256];
    char value[256];
    struct hashmap_entry *next;
} hashmap_entry;


typedef struct {
    hashmap_entry **entries;
} hashmap;


hashmap* hashmap_create();
void hashmap_set(hashmap *h, const char *key, const char *value);
char* hashmap_get(hashmap *h, const char *key);
void hashmap_free(hashmap *h);
int hashmap_remove(hashmap *h, const char *key);

#endif // HASHMAP_H


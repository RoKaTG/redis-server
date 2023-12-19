#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#include "../struct/hashmap.h"
#include "../cJSON/cJSON.h"

void save_to_file(hashmap *h, const char *filename);
void load_from_file(hashmap *h, const char *filename);

bool hashmap_is_empty(hashmap *h);
char* get_random_key(hashmap *h);
int match_pattern(const char *pattern, const char *key);

#endif // UTILS_H

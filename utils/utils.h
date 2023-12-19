#pragma once

#include <stdbool.h>

#include "../struct/hashmap.h"
#include "../cJSON/cJSON.h"



/*****************FUNCTION USED FOR CJSON HANDLING & SAVING DATA******************/
void save_to_file(hashmap *h, const char *filename);
void load_from_file(hashmap *h, const char *filename);


/*****************FUNCTION USED FOR SOME COMMAND.C FUNCTION******************/
bool hashmap_is_empty(hashmap *h);
char* get_random_key(hashmap *h);
int match_pattern(const char *pattern, const char *key);
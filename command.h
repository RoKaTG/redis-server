#ifndef COMMAND_H
#define COMMAND_H

#include "parser.h"
#include "hashmap.h"

#include <stdbool.h>

const char* handle_ping_command(const char* argument);
const char* handle_set_command(const char *key, const char *value);
const char* handle_get_command(const char *key);
const char* handle_del_command(Command cmd);
const char* handle_exists_command(Command cmd);

const char* handle_append_command(const char *key, const char *value);

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

bool hashmap_is_empty(hashmap *h);
char* get_random_key(hashmap *h);
const char* handle_randomkey_command(hashmap *h);

const char* handle_expire_command(const char *key, int seconds);
const char* handle_pexpire_command(const char *key, int ms);
const char* handle_persist_command(const char *key);
const char* handle_ttl_command(const char *key);
const char* handle_pttl_command(const char *key);

int match_pattern(const char *pattern, const char *key);

const char* handle_keys_command(const char *pattern);
const char* handle_rename_command(const char *key, const char *newkey);

#endif // COMMAND_H

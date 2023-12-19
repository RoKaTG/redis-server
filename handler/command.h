#pragma once

#include "../parser/parser.h"
#include "../struct/hashmap.h"
#include "../struct/expiration.h"



/******************BASE******************/

/******************BASICS KEYs MANIPULATION******************/
const char* handle_ping_command(const char* argument);
const char* handle_set_command(const char *key, const char *value);
const char* handle_get_command(const char *key);
const char* handle_del_command(Command cmd);


/******************INTERMEDIATE******************/

/******************JUST KEYs SEARCHING******************/
const char* handle_exists_command(Command cmd);
const char* handle_randomkey_command(hashmap *h);
const char* handle_keys_command(const char *pattern);


/******************KEYs & VALUEs MANIPULATION******************/
const char* handle_append_command(const char *key, const char *value);
const char* handle_rename_command(const char *key, const char *newkey);
const char* handle_copy_command(const char *source, const char *destination);


/******************VALUEs MANIPULATION ON INTEGER******************/
const char* handle_incr_command(const char *key);
const char* handle_decr_command(const char *key);
const char* handle_incrby_command(const char *key, const char *increment);
const char* handle_decrby_command(const char *key, const char *increment);


/******************USING TIMERs******************/
const char* handle_expire_command(const char *key, int seconds);
const char* handle_pexpire_command(const char *key, int ms);
const char* handle_persist_command(const char *key);
const char* handle_ttl_command(const char *key);
const char* handle_pttl_command(const char *key);


/*****************PERK******************/
const char* handle_helper_command();
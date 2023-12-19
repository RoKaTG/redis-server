#pragma once

#define MAX_KEYS 100



/*****************ENUMERATION FOR EVERY COMMANDs******************/
typedef enum {
    CMD_PING,
    CMD_SET,
    CMD_GET,
    CMD_DEL,
    CMD_EXISTS,
    CMD_APPEND,
    CMD_RANDOMKEY,
    CMD_EXPIRE,
    CMD_PEXPIRE,
    CMD_PERSIST,
    CMD_TTL,
    CMD_PTTL,
    CMD_KEYS,
    CMD_RENAME,
    CMD_COPY,
    CMD_INCR,
    CMD_DECR,
    CMD_INCRBY,
    CMD_DECRBY,
    CMD_HELPER,
    CMD_UNKNOWN 
} CommandType;


/*****************COMMAND STRUCTURE FOR COMMAND ARGUMENTs MANAGEMENT******************/
typedef struct {
    CommandType type;
    char key[256];
    char value[256];
    char argument[256];
    int num_keys;
    char keys[MAX_KEYS][256];
} Command;


Command parse_command(const char *input);



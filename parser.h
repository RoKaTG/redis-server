#ifndef PARSER_H
#define PARSER_H

#define MAX_KEYS 10

typedef enum {
    CMD_PING,
    CMD_SET,
    CMD_GET,
    CMD_DEL,
    CMD_EXISTS,
    CMD_UNKNOWN 
} CommandType;


typedef struct {
    CommandType type;
    char key[256];
    char value[256];
    char argument[256];
    int num_keys;
    char keys[MAX_KEYS][256];
} Command;


Command parse_command(const char *input);

#endif // PARSER_H


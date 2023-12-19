#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"
#include "../utils/utils.h"

extern hashmap *global_map;
extern expiration *expiration_map;


/**
 * Handle a PING command and generate a response.
 *
 * @param argument The argument received with the PING command (can be NULL or empty).
 * @return A Redis protocol formatted response string.
 */
const char* handle_ping_command(const char* argument) {
    static char response[1024];

    // Check if the argument is NULL or empty
    if (argument == NULL || strcmp(argument, "") == 0) {
        // If no argument provided, respond with "+PONG"
        strcpy(response, "+PONG\r\n");
    } else {
        // If an argument is provided, respond with the argument
        snprintf(response, sizeof(response), "$%zu\r\n%s\r\n", strlen(argument), argument);
    }

    return response;
}


/**
 * Handle a SET command and store a key-value pair in the global hashmap.
 *
 * @param key The key to set.
 * @param value The value to associate with the key.
 * @return A Redis protocol formatted response indicating success ("+OK").
 */
const char* handle_set_command(const char *key, const char *value) {
    static char response[256];

    // Store the key-value pair in the global hashmap
    hashmap_set(global_map, key, value);

    // Respond with "+OK" to indicate success
    strcpy(response, "+OK\r\n");
    return response;
}


/**
 * Handle a GET command and retrieve the value associated with a key from the global hashmap.
 *
 * @param key The key to retrieve the value for.
 * @return A Redis protocol formatted response containing the value or an error if the key is not found.
 */
const char* handle_get_command(const char *key) {
    static char response[1024];

    // Retrieve the value associated with the key from the global hashmap
    char *value = hashmap_get(global_map, key);
    
    if (value) {
        // If the key exists, respond with the value in Redis protocol format
        snprintf(response, sizeof(response), "$%zu\r\n%s\r\n", strlen(value), value);
    } else {
        // If the key does not exist, respond with "-1" to indicate an error
        strcpy(response, "$-1\r\n");
    }
    
    return response;
}


/**
 * Handle a DEL command to delete one or more keys from the global hashmap.
 *
 * @param cmd The command structure containing the keys to delete.
 * @return A Redis protocol formatted response indicating the number of keys deleted.
 */
const char* handle_del_command(Command cmd) {
    static char response[256];
    int count = 0;

    // Log the start of key deletion and the total number of keys
    printf("Begin deletion of keys. Total number of keys: %d\n", cmd.num_keys);

    for (int i = 0; i < cmd.num_keys; ++i) {
        // Log the attempt to delete each key
        printf("Attempting to delete key: %s\n", cmd.keys[i]);

        // Check if the key exists in the global hashmap and remove it if found
        if (hashmap_remove(global_map, cmd.keys[i])) {
            count++;
            // Log that the key was successfully deleted
            printf("Key deleted: %s\n", cmd.keys[i]);
        } else {
            // Log that the key was not found
            printf("Key not found: %s\n", cmd.keys[i]);
        }
    }

    // Log the total number of keys deleted and format the response
    printf("Total number of keys deleted: %d\n", count);
    snprintf(response, sizeof(response), ":%d\r\n", count);
    return response;
}


/**
 * Handle an EXISTS command to check the existence of one or more keys in the global hashmap.
 *
 * @param cmd The command structure containing the keys to check.
 * @return A Redis protocol formatted response indicating the number of keys that exist.
 */
const char* handle_exists_command(Command cmd) {
    static char response[256];
    int count = 0;
    
    // Iterate through the keys to check their existence in the global hashmap
    for (int i = 0; i < cmd.num_keys; ++i) {
        // Check if the key exists and increment the count if found
        if (hashmap_get(global_map, cmd.keys[i]) != NULL) {
            count++;
        }
    }
    
    // Format the response with the count of existing keys
    snprintf(response, sizeof(response), ":%d\r\n", count);
    return response;
}


/**
 * Handle an APPEND command to append a value to an existing key's value in the global hashmap.
 *
 * @param key The key to append the value to.
 * @param value The value to append.
 * @return A Redis protocol formatted response indicating the length of the updated value.
 */
const char* handle_append_command(const char *key, const char *value) {
    static char response[256];
    char *existing_value = hashmap_get(global_map, key);
    size_t new_length = strlen(value) + (existing_value ? strlen(existing_value) : 0);

    // Create a new buffer to store the updated value
    char *new_value = malloc(new_length + 1);
    if (existing_value) {
        strcpy(new_value, existing_value);
    }
    strcat(new_value, value);

    // Update the value associated with the key in the global hashmap
    hashmap_set(global_map, key, new_value);
    int appended_length = strlen(new_value);
    free(new_value);

    // Format the response with the length of the updated value
    snprintf(response, sizeof(response), ":%d\r\n", appended_length);
    return response;
}



/**
 * Handle a RANDOMKEY command to retrieve a random key from the hashmap.
 *
 * @param h The hashmap to retrieve a random key from.
 * @return A Redis protocol formatted response containing the random key or a null response if no keys are present.
 */
const char* handle_randomkey_command(hashmap *h) {
    static char response[256];
    char* key = get_random_key(h);

    if (key) {
        // If a random key is found, format the response with the key
        snprintf(response, sizeof(response), "$%zu\r\n%s\r\n", strlen(key), key);
    } else {
        // If no keys are present, return a null response
        strcpy(response, "$-1\r\n");
    }

    return response;
}


/**
 * Handle an EXPIRE command to set an expiration time (in seconds) on a key in the global hashmap.
 *
 * @param key The key to set an expiration time for.
 * @param seconds The number of seconds until expiration.
 * @return A Redis protocol formatted response indicating success (1) or failure (0).
 */
const char* handle_expire_command(const char *key, int seconds) {
    static char response[256];
    long long current_time_ms = time(NULL) * 1000LL;
    long long expiration_time_ms = current_time_ms + ((long long)seconds * 1000LL);

    // Check if the key exists in the global hashmap
    if (hashmap_get(global_map, key) == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    // Set the expiration time for the key in the expiration map
    expiration_map_set(expiration_map, key, expiration_time_ms);

    // Respond with a success indication
    snprintf(response, sizeof(response), ":%d\r\n", 1);
    return response;
}



/**
 * Handle a PEXPIRE command to set an expiration time (in milliseconds) on a key in the global hashmap.
 *
 * @param key The key to set an expiration time for.
 * @param ms The number of milliseconds until expiration.
 * @return A Redis protocol formatted response indicating success (1) or failure (0).
 */
const char* handle_pexpire_command(const char *key, int ms) {
    static char response[256];
    long long current_time_ms = time(NULL) * 1000LL;
    long long expiration_time_ms = current_time_ms + ms;

    // Check if the key exists in the global hashmap
    if (hashmap_get(global_map, key) == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    // Set the expiration time for the key in the expiration map
    expiration_map_set(expiration_map, key, expiration_time_ms);

    // Respond with a success indication
    snprintf(response, sizeof(response), ":%d\r\n", 1);
    return response;
}


/**
 * Handle a PERSIST command to remove the expiration time of a key in the global hashmap.
 *
 * @param key The key to remove the expiration time from.
 * @return A Redis protocol formatted response indicating success (1) or failure (0).
 */
const char* handle_persist_command(const char *key) {
    static char response[256];

    // Check if the key does not exist or does not have an expiration time
    if (hashmap_get(global_map, key) == NULL || !expiration_map_exists(expiration_map, key)) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    // Remove the expiration time for the key in the expiration map
    expiration_map_remove(expiration_map, key);

    // Respond with a success indication
    snprintf(response, sizeof(response), ":%d\r\n", 1);
    return response;
}


/**
 * Handle a TTL command to get the remaining time to live (in seconds) of a key in the global hashmap.
 *
 * @param key The key to get the TTL for.
 * @return A Redis protocol formatted response indicating TTL in seconds (-2 if the key does not exist, -1 if it does not expire, and the TTL value if it expires).
 */
const char* handle_ttl_command(const char *key) {
    static char response[256];
    long long current_time_ms = time(NULL) * 1000LL;

    // Check if the key does not exist
    if (hashmap_get(global_map, key) == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", -2);
        return response;
    }

    long long expiration_time_ms = get_expiration_time_ms(expiration_map, key);

    // Check if the key does not have an expiration time
    if (expiration_time_ms == 0) {
        snprintf(response, sizeof(response), ":%d\r\n", -1);
        return response;
    }

    int ttl = (int)((expiration_time_ms - current_time_ms) / 1000LL);

    // Respond with TTL value or -1 if it's negative (already expired)
    snprintf(response, sizeof(response), ":%d\r\n", ttl > 0 ? ttl : -1);
    return response;
}

/**
 * Handle a PTTL command to get the remaining time to live (in milliseconds) of a key in the global hashmap.
 *
 * @param key The key to get the PTTL for.
 * @return A Redis protocol formatted response indicating PTTL in milliseconds (-2 if the key does not exist, -1 if it does not expire, and the PTTL value if it expires).
 */
const char* handle_pttl_command(const char *key) {
    static char response[256];
    long long current_time_ms = time(NULL) * 1000LL;

    // Check if the key does not exist
    if (hashmap_get(global_map, key) == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", -2);
        return response;
    }

    long long expiration_time_ms = get_expiration_time_ms(expiration_map, key);

    // Check if the key does not have an expiration time
    if (expiration_time_ms == 0) {
        snprintf(response, sizeof(response), ":%d\r\n", -1);
        return response;
    }

    int pttl = (int)(expiration_time_ms - current_time_ms);

    // Respond with PTTL value or -1 if it's negative (already expired)
    snprintf(response, sizeof(response), ":%d\r\n", pttl > 0 ? pttl : -1);
    return response;
}


/**
 * Handle a KEYS command to find all keys matching the given pattern in the global hashmap.
 *
 * @param pattern The pattern to search for.
 * @return A Redis protocol formatted response containing the matching keys.
 */
const char* handle_keys_command(const char *pattern) {
    static char response[10240];  // A static buffer for the response
    char *response_ptr = response;  // Pointer to the response buffer
    int count = 0;  // Counter for matching keys

    // Iterate over the hashmap entries
    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        hashmap_entry *entry = global_map->entries[i];
        while (entry != NULL) {
            // Check if the key matches the pattern
            if (strstr(entry->key, pattern) != NULL) {
                count++;
            }
            entry = entry->next;
        }
    }

    // Format the response with the count of matching keys
    response_ptr += sprintf(response_ptr, "*%d\r\n", count);

    // Iterate over the hashmap entries again to add matching keys to the response
    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        hashmap_entry *entry = global_map->entries[i];
        while (entry != NULL) {
            // Check if the key matches the pattern and add it to the response
            if (strstr(entry->key, pattern) != NULL) {
                response_ptr += sprintf(response_ptr, "$%zu\r\n%s\r\n", strlen(entry->key), entry->key);
            }
            entry = entry->next;
        }
    }

    return response;
}


/**
 * Handle a RENAME command to rename a key in the global hashmap.
 *
 * @param key The current key name.
 * @param newkey The new key name.
 * @return A Redis protocol formatted response indicating success or an error if the key does not exist.
 */
const char* handle_rename_command(const char *key, const char *newkey) {
    static char response[256];

    // Get the value associated with the current key
    char *value = hashmap_get(global_map, key);

    // Check if the key does not exist
    if (value == NULL) {
        strcpy(response, "-ERR no such key\r\n");
        return response;
    }

    // Set the value with the new key name and remove the old key
    hashmap_set(global_map, newkey, value);
    hashmap_remove(global_map, key);

    // Respond with a success indication
    strcpy(response, "+OK\r\n");
    return response;
}


/**
 * Handle a COPY command to copy the value of a key to another key in the global hashmap.
 *
 * @param source The source key.
 * @param destination The destination key.
 * @return A Redis protocol formatted response indicating success (1) or failure (0).
 */
const char* handle_copy_command(const char *source, const char *destination) {
    static char response[256];

    // Get the value associated with the source key
    char *value = hashmap_get(global_map, source);

    // Check if the source key does not exist
    if (value == NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    // Check if the destination key already exists
    if (hashmap_get(global_map, destination) != NULL) {
        snprintf(response, sizeof(response), ":%d\r\n", 0);
        return response;
    }

    // Set the value with the destination key
    hashmap_set(global_map, destination, value);

    // Respond with a success indication
    snprintf(response, sizeof(response), ":%d\r\n", 1);
    return response;
}


/**
 * Handle an INCR command to increment the value of a key by 1.
 *
 * @param key The key whose value should be incremented.
 * @return A Redis protocol formatted response containing the new incremented value.
 */
const char* handle_incr_command(const char *key) {
    static char response[256];

    // Get the current value associated with the key
    char *value = hashmap_get(global_map, key);
    long long number;

    // Initialize the number to 0 if the key doesn't exist
    if (value == NULL) {
        number = 0;
    } else {
        // Attempt to parse the current value as a long long
        char *endptr;
        number = strtoll(value, &endptr, 10);
        
        // Check for parsing errors
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }
    }

    // Increment the number and update the key's value
    number++;
    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", number);
    hashmap_set(global_map, key, new_value);

    // Format the response with the new value
    snprintf(response, sizeof(response), ":%lld\r\n", number);
    return response;
}


/**
 * Handle a DECR command to decrement the value of a key by 1.
 *
 * @param key The key whose value should be decremented.
 * @return A Redis protocol formatted response containing the new decremented value.
 */
const char* handle_decr_command(const char *key) {
    static char response[256];

    // Get the current value associated with the key
    char *value = hashmap_get(global_map, key);
    long long number;

    // Initialize the number to 0 if the key doesn't exist
    if (value == NULL) {
        number = 0;
    } else {
        // Attempt to parse the current value as a long long
        char *endptr;
        number = strtoll(value, &endptr, 10);
        
        // Check for parsing errors
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }
    }

    // Decrement the number and update the key's value
    number--;
    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", number);
    hashmap_set(global_map, key, new_value);

    // Format the response with the new value
    snprintf(response, sizeof(response), ":%lld\r\n", number);
    return response;
}


/**
 * Handle an INCRBY command to increment the value of a key by a specified amount.
 *
 * @param key The key whose value should be incremented.
 * @param increment The amount by which to increment the value.
 * @return A Redis protocol formatted response containing the new incremented value.
 */
const char* handle_incrby_command(const char *key, const char *increment) {
    static char response[256];

    // Get the current value associated with the key
    char *value = hashmap_get(global_map, key);
    long long number;

    // Initialize the number to 0 if the key doesn't exist
    if (value == NULL) {
        number = 0;
    } else {
        // Attempt to parse the current value as a long long
        char *endptr;
        number = strtoll(value, &endptr, 10);
        
        // Check for parsing errors
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }
    }

    // Parse the increment value
    long long increment_value;
    char *endptr;
    increment_value = strtoll(increment, &endptr, 10);

    // Check for parsing errors in the increment value
    if (*endptr != '\0') {
        return "-ERR increment is not an integer or out of range\r\n";
    }

    // Increment the number by the specified increment and update the key's value
    number += increment_value;
    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", number);
    hashmap_set(global_map, key, new_value);

    // Format the response with the new value
    snprintf(response, sizeof(response), ":%lld\r\n", number);
    return response;
}


/**
 * Handle a DECRBY command to decrement the value of a key by a specified amount.
 *
 * @param key The key whose value should be decremented.
 * @param decrement The amount by which to decrement the value.
 * @return A Redis protocol formatted response containing the new decremented value.
 */
const char* handle_decrby_command(const char *key, const char *decrement) {
    static char response[256];

    // Get the current value associated with the key
    char *value = hashmap_get(global_map, key);
    long long number;

    // Initialize the number to 0 if the key doesn't exist
    if (value == NULL) {
        number = 0;
    } else {
        // Attempt to parse the current value as a long long
        char *endptr;
        number = strtoll(value, &endptr, 10);
        
        // Check for parsing errors
        if (*endptr != '\0') {
            return "-ERR value is not an integer or out of range\r\n";
        }
    }

    // Parse the decrement value
    long long decrement_value;
    char *endptr;
    decrement_value = strtoll(decrement, &endptr, 10);

    // Check for parsing errors in the decrement value
    if (*endptr != '\0') {
        return "-ERR decrement is not an integer or out of range\r\n";
    }

    // Decrement the number by the specified decrement and update the key's value
    number -= decrement_value;
    char new_value[256];
    snprintf(new_value, sizeof(new_value), "%lld", number);
    hashmap_set(global_map, key, new_value);

    // Format the response with the new value
    snprintf(response, sizeof(response), ":%lld\r\n", number);
    return response;
}


/**
 * Handle a HELPER command to display a MAN-like message explaining the available commands.
 *
 * @return A Redis protocol formatted response containing the MAN page.
 */
const char* handle_helper_command() {
    static char help_text[4096]; // Static buffer to store the MAN page

    const char* help_content =
        "Commands:\n"
        "PING: Test server connectivity.\n"
        "SET key value: Set the value of a key.\n"
        "GET key: Retrieve the value of a key.\n"
        "DEL key [key ...]: Delete one or more keys.\n"
        "EXISTS key [key ...]: Check if one or more keys exist.\n"
        "APPEND key value: Append a value to the end of a key.\n"
        "RANDOMKEY: Return a random key.\n"
        "EXPIRE key seconds: Set an expiration time for a key in seconds.\n"
        "PEXPIRE key milliseconds: Set an expiration time for a key in milliseconds.\n"
        "PERSIST key: Remove the expiration time for a key.\n"
        "TTL key: Get the remaining time to live (TTL) of a key in seconds.\n"
        "PTTL key: Get the remaining time to live (TTL) of a key in milliseconds.\n"
        "KEYS pattern: Find all keys matching a given pattern.\n"
        "RENAME key newkey: Rename a key.\n"
        "COPY source destination: Copy the value of a key to another key.\n"
        "INCR key: Increment the numeric value of a key.\n"
        "DECR key: Decrement the numeric value of a key.\n"
        "INCRBY key increment: Increment the numeric value of a key by a specified amount.\n"
        "DECRBY key decrement: Decrement the numeric value of a key by a specified amount.\n";

    // Calculate the length of help_content, including the '\0' string terminator
    int help_content_length = strlen(help_content);

    // Format the response in bulk (multi-string) format
    snprintf(help_text, sizeof(help_text), "*2\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", help_content_length, help_content, help_content_length, help_content);

    return help_text;
}


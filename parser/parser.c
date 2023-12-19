#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

Command parse_command(const char *input) {

    /*******************I parse the commands by separating the strings that are spaced 
     *(so command + argument etc.) and I put each of them to a new line to make parsing easier****************************/
    
    // Initialize a Command struct
    Command cmd;
    memset(&cmd, 0, sizeof(Command));

    char *line = strtok(input, "\r\n");
    int line_count = 0;
    int key_index = 0;

    while (line != NULL) {

        // Parse the number of keys in the command
        if (line[0] == '*') {
            sscanf(line, "*%d", &cmd.num_keys);
        } 
        
        // No need to worry about the argument length
        else if (line[0] == '$') {
        } 
        
        else {
            line_count++;
/***********************PARSING COMMAND NAME************************************/
            if (line_count == 1) {
                
                if (strcmp(line, "PING") == 0) {
                    cmd.type = CMD_PING;
                } 
                
                else if (strcmp(line, "SET") == 0) {
                    cmd.type = CMD_SET;
                } 
                
                else if (strcmp(line, "GET") == 0) {
                    cmd.type = CMD_GET;
                } 
                
                else if (strcmp(line, "DEL") == 0) {
                    cmd.type = CMD_DEL;
                } 
                
                else if (strcmp(line, "EXISTS") == 0) {
                    cmd.type = CMD_EXISTS;
                } 
                
                else if (strcmp(line, "APPEND") == 0) {
                    cmd.type = CMD_APPEND;
                } 
                
                else if (strcmp(line, "RANDOMKEY") == 0) {
                    cmd.type = CMD_RANDOMKEY;
                } 
                
                else if (strcmp(line, "EXPIRE") == 0) {
                    cmd.type = CMD_EXPIRE;
                } 
                
                else if (strcmp(line, "PEXPIRE") == 0) {
                    cmd.type = CMD_PEXPIRE;
                } 
                
                else if (strcmp(line, "PERSIST") == 0) {
                    cmd.type = CMD_PERSIST;
                } 
                
                else if (strcmp(line, "TTL") == 0) {
                    cmd.type = CMD_TTL;
                } 
                
                else if (strcmp(line, "KEYS") == 0) {
                    cmd.type = CMD_KEYS;
                } 
                
                else if (strcmp(line, "PTTL") == 0) {
                    cmd.type = CMD_PTTL;
                } 
                
                else if (strcmp(line, "RENAME") == 0) {
                    cmd.type = CMD_RENAME;
                } 
                
                else if (strcmp(line, "COPY") == 0) {
                    cmd.type = CMD_COPY;
                } 
                
                else if (strcmp(line, "INCR") == 0) {
                    cmd.type = CMD_INCR;
                } 
                
                else if (strcmp(line, "DECR") == 0) {
                    cmd.type = CMD_DECR;
                } 
                
                else if (strcmp(line, "INCRBY") == 0) {
                    cmd.type = CMD_INCRBY;
                } 
                
                else if (strcmp(line, "DECRBY") == 0) {
                    cmd.type = CMD_DECRBY;
                } 
                
                else if (strcmp(line, "HELPER") == 0) {
                    cmd.type = CMD_HELPER;
                } 
                
                else { 
                    cmd.type = CMD_UNKNOWN;
                }
            }
/***********************END PARSING NAMES & BEGIN PARSING ARGS************************************/

            else if (cmd.type == CMD_PING) {
                if (line_count == 2) {
                    strncpy(cmd.argument, line, sizeof(cmd.argument) - 1);
                }
            } 
            
            else if (cmd.type == CMD_GET || cmd.type == CMD_PERSIST || cmd.type == CMD_TTL || 
                     cmd.type == CMD_PTTL || cmd.type == CMD_KEYS ||
                     cmd.type == CMD_INCR || cmd.type == CMD_DECR) {
                if (line_count == 2) {
                    strncpy(cmd.key, line, sizeof(cmd.key) - 1);
                }
            } 
            
            else if (cmd.type == CMD_SET) {
                if (line_count == 2) {
                    strncpy(cmd.key, line, sizeof(cmd.key) - 1);
                }
                else if (line_count == 3) {
                    strncpy(cmd.value, line, sizeof(cmd.value) - 1);
                }
            } 
            
            // Handle commands with variable numbers of keys
            else if ((cmd.type == CMD_EXISTS || cmd.type == CMD_DEL) && key_index < MAX_KEYS) {
                strncpy(cmd.keys[key_index], line, sizeof(cmd.keys[0]) - 1);
                key_index++;
            }
            
            else if (cmd.type == CMD_EXPIRE || cmd.type == CMD_PEXPIRE || cmd.type == CMD_APPEND || 
                     cmd.type == CMD_RENAME || cmd.type == CMD_COPY || cmd.type == CMD_INCRBY ||
                     cmd.type == CMD_DECRBY) {
                if (line_count == 2) {
                    strncpy(cmd.key, line, sizeof(cmd.key) - 1);
                } else if (line_count == 3) {
                    strncpy(cmd.value, line, sizeof(cmd.value) - 1);
                }
            }
        }

        line = strtok(NULL, "\r\n");
    }

    if (cmd.type == CMD_DEL || cmd.type == CMD_EXISTS) {
        cmd.num_keys = key_index;
    }

    return cmd;
}

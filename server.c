#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <signal.h>

#include "parser/parser.h"
#include "handler/command.h"
#include "struct/hashmap.h"
#include "utils/utils.h"

#define CHECK_INTERVAL 5

#define RED "\x1b[31m"
#define RS "\x1b[0m"


//faire etape 2 puis 3 puis 1 de la partie avancé = mieux

hashmap *global_map;
expiration *expiration_map;

// Structure for storing thread information
struct th_info {
int fd; // File descriptor for the client socket
int i; // Client identifier (for reference)
};

void *handle_client(void *pctx) {
    // Cast the context pointer to the appropriate struct type
    struct th_info *ctx = (struct th_info *)pctx;
    
    // Get the client file descriptor from the context
    int client_fd = ctx->fd;

    // Create a buffer to read client data
    char buffer[1024] = {0};

    int read_bytes = read(client_fd, buffer, 1024);
    if (read_bytes > 0) {
        buffer[read_bytes] = '\0';
        
        // Log received data for debugging
        printf("Received : %s\n", buffer);

        // Parse the command from the received data
        Command cmd = parse_command(buffer);
        const char* response;

        // Handle different command types
        switch (cmd.type) {
            case CMD_PING:
                response = handle_ping_command(cmd.argument);
                break;
            case CMD_SET:
                response = handle_set_command(cmd.key, cmd.value);
                break;
            case CMD_GET:
                response = handle_get_command(cmd.key);
                break;
            case CMD_DEL:
                response = handle_del_command(cmd);
                break;
            case CMD_EXISTS:
                response = handle_exists_command(cmd);
                break;
            case CMD_APPEND:
                response = handle_append_command(cmd.key, cmd.value);
                break;
            case CMD_RANDOMKEY:
                response = handle_randomkey_command(global_map);
                break;
            case CMD_EXPIRE:
                response = handle_expire_command(cmd.key, atoi(cmd.value));
                break;
            case CMD_PEXPIRE:
                response = handle_pexpire_command(cmd.key, atoi(cmd.value));
                break;
            case CMD_PERSIST:
                response = handle_persist_command(cmd.key);
                break;
            case CMD_TTL:
                response = handle_ttl_command(cmd.key);
                break;
            case CMD_PTTL:
                response = handle_pttl_command(cmd.key);
                break;
            case CMD_KEYS:
                response = handle_keys_command(cmd.key);
                break;
            case CMD_RENAME:
                response = handle_rename_command(cmd.key, cmd.value);
                break;
            case CMD_COPY:
                response = handle_copy_command(cmd.key, cmd.value);
                break;
            case CMD_INCR:
                response = handle_incr_command(cmd.key);
                break;
            case CMD_DECR:
                response = handle_decr_command(cmd.key);
                break;
            case CMD_INCRBY:
                response = handle_incrby_command(cmd.key, cmd.value);
                break;
            case CMD_DECRBY:
                response = handle_decrby_command(cmd.key, cmd.value);
                break;
            case CMD_HELPER:
                response = handle_helper_command();
                break;
            case CMD_UNKNOWN:
            // Handle default behavior
                response = "-Unknown command\r\n";
        }

        printf("Response : %s\n", response);

        // Write the response back to the client
        write(client_fd, response, strlen(response));
    }

    close(client_fd);
    free(ctx);

    return NULL;
}

void signal_handler(int signum) {
    printf("\nServer is closing...\n");
    save_to_file(global_map, "data.json");
    exit(signum);
}

void *expiration_checker(void *arg) {
    while (1) {
        sleep(CHECK_INTERVAL); 
        check_and_remove_expired_keys(global_map, expiration_map);
    }
}


int main(int argc, char const *argv[]) {                                                                             
    printf(RED "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░ \n" RS);
    printf(RED "█████████████████░░████████████████░░█████████████████\n" RS);
    printf(RED "█░ ____        ░█░░ _____          ░░█░ ____        ░█" RS " ___ ____    ____  _____ ______     _______ ____        \n");   
    printf(RED "█░|  _ \\       ░█░░| ____|         ░░█░|  _ \\       ░█" RS "|_ _/ ___|  / ___|| ____|  _ \\ \\   / / ____|  _ \\    \n");   
    printf(RED "█░| |_)|       ░█░░|  _|           ░░█░| | | |      ░█" RS " | |\\___ \\  \\___ \\|  _| | |_) \\ \\ / /|  _| | |_) |\n");  
    printf(RED "█░|  _<        ░█░░| |___          ░░█░| |_| |      ░█"RS " | | ___) |  ___) | |___|  _ < \\ V / | |___|  _ <      \n");   
    printf(RED "█░|_| \\_\\      ░█░░|_____|         ░░█░|____/       ░█" RS "|___|____/  |____/|_____|_| \\_\\ \\_/  |_____|_| \\_\\ \n"); 
    printf(RED "█████████████████░░████████████████░░█████████████████\n" RS);
    printf(RED "░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░\n" RS);

printf("\n\n");

printf(
"   _____             ____  _  ______ \n"
"  / ___ \\           |  _ \\| |/ / ___|\n"
" / / __| \\   _____  | |_) | ' / |  _ \n"
"| | (__   | |_____| |  _ <| . \\ |_| |\n"
" \\ \\___| /          |_| \\_\\_|\\_\\____|\n"
"  \\_____/                            \n");

    printf("\n\n");
    printf("Welcome to the AISE Redis-inspired server!\n");
    printf("This server offers several Redis functionalities for educational purposes.\n");
    printf("How to Use This Server:\n");
    printf("1. Open a new command prompt or terminal window for the client.\n");
    printf("2. Connect to the server using the Redis CLI: redis-cli -p <port> (Note : Replace <port> with the port number your server is listening on.)\n");
    printf("3. Once connected, type" RED " HELPER" RS " to get a list of available commands and their usage.\n");
    printf("4. Use the connected terminal as your client to interact with the server.\n\n");

    printf("To stop the server, press CTRL+C in the server's terminal window.\n");
    printf("To disconnect the client, also press CTRL+C or type <exit> in the client's terminal window.\n\n");


    signal(SIGINT, signal_handler);
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *result = NULL;
    int ret = getaddrinfo(NULL, argv[1], &hints, &result);

    if (ret < 0) {
        herror("getaddrinfo");
        return 1;
    }

    int sock = 0;
    int server_ready = 0;
    struct addrinfo *tmp;
    
    global_map = hashmap_create();
    expiration_map = expiration_map_create();

    load_from_file(global_map, "data.json");

    pthread_t expiration_thread;
    pthread_create(&expiration_thread, NULL, expiration_checker, NULL);

    for (tmp = result; tmp != NULL; tmp = tmp->ai_next) {
        sock = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);

        if (sock < 0) {
            perror("socket");
            continue;
        }

        if (bind(sock, tmp->ai_addr, tmp->ai_addrlen) < 0) {
            perror("bind");
            continue;
        }

        if (listen(sock, 5) < 0) {
            perror("listen");
            continue;
        }

        server_ready = 1;
        break;
    }

    if (server_ready == 0) {
        fprintf(stderr, "The server couldn't start.\n");
        return 1;
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = -1;
    int i = 0;

    while (1) {
        client_fd = accept(sock, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd < 0) {
            perror("accept");
            break;
        }

        struct th_info *ctx = malloc(sizeof(struct th_info));
        if (ctx == NULL) {
            perror("malloc");
            continue;
        }

        ctx->fd = client_fd;
        ctx->i = i++;

        pthread_t th;
        pthread_create(&th, NULL, handle_client, (void *)ctx);
    }

    hashmap_free(global_map);
    expiration_map_free(expiration_map);    
    
    close(sock);
    freeaddrinfo(result);
    return 0;
}







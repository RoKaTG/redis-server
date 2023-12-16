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

#include "parser.h"
#include "command.h"
#include "hashmap.h"
#include "cJSON.h"

#define CHECK_INTERVAL 5

hashmap *global_map;
expiration *expiration_map;

// Structure pour stocker les informations du thread
struct th_info {
    int fd;  // Descripteur de fichier pour le socket client
    int i;   // Identifiant du client (pour référence)
};

void save_to_file(hashmap *h, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return;

    cJSON *root = cJSON_CreateObject();

    for (int i = 0; i < HASHMAP_SIZE; ++i) {
        hashmap_entry *entry = h->entries[i];
        while (entry) {
            cJSON_AddStringToObject(root, entry->key, entry->value);
            entry = entry->next;
        }
    }

    char *json_data = cJSON_Print(root);
    fprintf(file, "%s", json_data);
    free(json_data);
    cJSON_Delete(root);
    fclose(file);
}


void load_from_file(hashmap *h, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *json_data = malloc(file_size + 1);
    if (!json_data) {
        fclose(file);
        return;
    }

    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0'; 

    cJSON *root = cJSON_Parse(json_data);
    cJSON *current_element = NULL;
    cJSON_ArrayForEach(current_element, root) {
        const char *key = current_element->string;
        const char *value = cJSON_GetStringValue(current_element);
        hashmap_set(h, key, value);
    }

    cJSON_Delete(root);
    free(json_data);
    fclose(file);
}


void *handle_client(void *pctx) {
    struct th_info *ctx = (struct th_info *)pctx;
    int client_fd = ctx->fd;
    char buffer[1024] = {0};

    int read_bytes = read(client_fd, buffer, 1024);
    if (read_bytes > 0) {
        buffer[read_bytes] = '\0'; 
        printf("Reçu : %s\n", buffer); // Log pour débogage

        Command cmd = parse_command(buffer);
        const char* response;

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
            case CMD_KEYS:
                response = handle_keys_command(cmd.key);
                break;
            case CMD_UNKNOWN:
            //default:
                response = "-Commande inconnue\r\n";
        }

        printf("Réponse : %s\n", response); // Log pour débogage
        write(client_fd, response, strlen(response));
    }

    close(client_fd);
    free(ctx);

    return NULL;
}

void signal_handler(int signum) {
    printf("Arrêt du serveur...\n");
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
        fprintf(stderr, "Le serveur n'a pas pu démarrer\n");
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

    //save_to_file(global_map, "data.json");

    return 0;
}

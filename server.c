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


// Structure pour stocker les informations du thread
struct th_info {
    int fd;  // Descripteur de fichier pour le socket client
    int i;   // Identifiant du client (pour référence)
};

void *handle_client(void *pctx) {
    struct th_info *ctx = (struct th_info *)pctx;
    int client_fd = ctx->fd;

    char welcome_msg[128];
    snprintf(welcome_msg, 128, "Bienvenue, Client %d\n", ctx->i);
    write(client_fd, welcome_msg, strlen(welcome_msg));

    sleep(5);

    close(client_fd);

    free(ctx);

    return NULL;
}

int main(int argc, char const *argv[]) {
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

    int client_fd = -1;
    struct sockaddr client_addr;
    socklen_t client_addr_len;
    int i = 0;

    while (1) {
        client_fd = accept(sock, &client_addr, &client_addr_len);

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

    close(sock);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "tcp_server.h"

static int client_fds[MAX_CLIENTS];
static pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
static int client_count = 0;


static void *client_handler(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[1024];

    while (1) {
        int n = recv(client_fd, buffer, sizeof(buffer), 0);
        if(n<= 0){
            break;
        }
        
    }

    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++)
    {
        if (client_fds[i] == client_fd) {
            client_fds[i] = client_fds[--client_count];
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);
    // Clean up the client socket
    printf("Client disconnected: %d\n", client_fd);
    close(client_fd);
    return NULL;
}

static void *tcp_server_thread(void *arg){
    int port = *(int *)arg;
    free(arg);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return NULL;
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return NULL;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return NULL;
    }
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        return NULL;
    }
    printf("TCP server listening on port %d\n", port);
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int));
        if (!client_fd) {
            perror("Memory allocation failed");
            continue;
        }
        *client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (*client_fd < 0) {
            perror("Accept failed");
            free(client_fd);
            continue;
        }

        pthread_mutex_lock(&client_mutex);
        if (client_count < MAX_CLIENTS) {
            client_fds[client_count++] = *client_fd;
            pthread_mutex_unlock(&client_mutex);
            printf("Client connected: %d\n", *client_fd);
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, client_handler, client_fd);
            pthread_detach(thread_id);
        } else {
            pthread_mutex_unlock(&client_mutex);
            printf("Max clients reached. Closing connection: %d\n", *client_fd);
            close(*client_fd);
            free(client_fd);
        }
    }
}

void *start_tcp_server(int port) {
    pthread_t server_thread;
    int *port_ptr = malloc(sizeof(int));
    if (!port_ptr) {
        perror("Memory allocation failed");
        return -1;
    }
    *port_ptr = port;

    if (pthread_create(&server_thread, NULL, tcp_server_thread, port_ptr) != 0) {
        perror("Failed to create server thread");
        free(port_ptr);
        return -1;
    }
    pthread_detach(server_thread);
    return 0;
}

int tcp_send(const void *data, size_t size) {
    int sent_count = 0;
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
        if (send(client_fds[i], data, size, 0) < 0) {
            perror("Send failed");
        }
        else {
            sent_count++;
        }
    }
    pthread_mutex_unlock(&client_mutex);
    return 0;
}
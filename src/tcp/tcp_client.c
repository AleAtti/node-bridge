#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "tcp_client.h"

static int client_fd = -1;
static pthread_mutex_t client_lock = PTHREAD_MUTEX_INITIALIZER;

static void *tcp_client_thread(void *arg)
{
    const char *host = (const char *)arg;
    int port = ((int *)((char **)arg)[1]);
    free(arg);

    while (1)
    {
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0)
        {
            perror("Socket creation failed");
            return NULL;
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0)
        {
            perror("[TCP-Client] Invalid address");
            close(client_fd);
            return NULL;
        }

        if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            perror("[TCP-Client] Connection failed");
            close(client_fd);
            return NULL;
        }

        printf("[TCP-Client] Connected to TCP server at %s:%d\n", host, port);

        printf("[TCP-Client] Disconnected\n");
        close(client_fd);
        pthread_mutex_lock(&client_lock);
        client_fd = -1;
        pthread_mutex_unlock(&client_lock);
        sleep(5); // retry
    }
    
    return NULL;
}

void tcp_client_start(const char *host, int port) {
    char **args = malloc(sizeof(char *) * 2);
    args[0] = strdup(host);
    args[1] = malloc(sizeof(int));
    memcpy(args[1], &port, sizeof(int));
    pthread_t tid;
    pthread_create(&tid, NULL, tcp_client_thread, args);
    pthread_detach(tid);
}

int tcp_client_send(const void *data, size_t len) {
    int sent = 0;
    pthread_mutex_lock(&client_lock);
    if (client_fd >= 0) {
        sent = send(client_fd, data, len, 0);
    }
    pthread_mutex_unlock(&client_lock);
    return sent;
}
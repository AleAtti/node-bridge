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
    TcpClientConfig *config = (TcpClientConfig *)arg;
    if (!config)
    {
        fprintf(stderr, "[TCP-Client] Invalid configuration\n");
        return NULL;
    }

    char host[64];
    strncpy(host, config->host, sizeof(host) - 1);
    host[sizeof(host) - 1] = '\0'; // Ensure null termination
    int port = config->port;
    free(config);

    while (1)
    {

        printf("[TCP-Client] Starting TCP client thread for %s:%d\n", host, port);

        pthread_mutex_lock(&client_lock);
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        pthread_mutex_unlock(&client_lock);

        if (client_fd < 0)
        {
            fprintf(stderr, "[TCP-Client] Socket creation failed: %s\n", strerror(errno));
            return NULL;
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0)
        {
            fprintf(stderr, "[TCP-Client] Invalid address: %s\n", strerror(errno));
            close(client_fd);
            return NULL;
        }

        if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            fprintf(stderr, "[TCP-Client] Connection failed: %s\n", strerror(errno));
            close(client_fd);
            return NULL;
        }

        printf("[TCP-Client] Connected to TCP server at %s:%d\n", host, port);

        char buf[256];
        while (1)
        {
            int n = recv(client_fd, buf, sizeof(buf), 0);
            if (n == 0)
            {
                printf("[TCP-Client] Server closed the connection\n");
                break;
            }
            else if (n < 0)
            {
                perror("[TCP-Client] recv failed");
                break;
            }
        }

        pthread_mutex_lock(&client_lock);
        close(client_fd);
        client_fd = -1;
        pthread_mutex_unlock(&client_lock);

        printf("[TCP-Client] Connection closed, retrying in 5 seconds...\n");
        sleep(5); // Wait before retrying connection
    }

    return NULL;
}

void tcp_client_start(const char *host, int port)
{
    TcpClientConfig *args = malloc(sizeof(TcpClientConfig));
    if (!args)
    {
        perror("Memory allocation failed");
        return;
    }
    strncpy(args->host, host, sizeof(args->host));
    args->host[sizeof(args->host) - 1] = '\0'; // Ensure null termination
    args->port = port;
    pthread_t tid;
    pthread_create(&tid, NULL, tcp_client_thread, args);
    pthread_detach(tid);
}

void tcp_client_stop()
{
    pthread_mutex_lock(&client_lock);
    if (client_fd >= 0)
    {
        close(client_fd);
        client_fd = -1;
        printf("[TCP-Client] Connection closed\n");
    }
    pthread_mutex_unlock(&client_lock);
}

int tcp_client_send(const void *data, size_t len)
{
    int sent = 0;
    pthread_mutex_lock(&client_lock);
    if (client_fd >= 0)
    {
        sent = send(client_fd, data, len, 0);
    }
    pthread_mutex_unlock(&client_lock);
    return sent;
}
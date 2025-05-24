#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static int client_fds[MAX_CLIENTS];
static pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
static int client_count = 0;


static void *client_handler(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        // Echo back the received data
        send(client_fd, buffer, bytes_read, 0);
    }

    if (bytes_read < 0) {
        perror("recv");
    }

    close(client_fd);
    
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_fds[i] == client_fd) {
            client_fds[i] = client_fds[--client_count];
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);

    return NULL;
}
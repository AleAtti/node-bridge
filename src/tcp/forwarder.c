#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "forwarder.h"
#include "tcp_server.h"
#include "tcp_client.h"

// Buffer siize for TCP data.
#define TCP_BUFFER_SIZE 2048
static char tcp_buffer[TCP_BUFFER_SIZE];
static size_t tcp_buffer_len = 0;
static pthread_mutex_t tcp_buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

// At the moment set default interval to 2 seconds 
static int interval_ms = 2000;
static int use_interval = 1; // enable interval by default (1) or disable it (0)

static enum {
    FORWARD_NONE,
    FORWARD_SERVER,
    FORWARD_CLIENT
} forward_mode = FORWARD_NONE;

static void *flush_thread(void * arg){
    while (1)
    {
        usleep(interval_ms * 1000); // Sleep for the specified interval
        pthread_mutex_lock(&tcp_buffer_mutex);

        if (use_interval && tcp_buffer_len > 0) {
            // Send the buffered data
            if (forward_mode == FORWARD_SERVER) {
                tcp_server_send(tcp_buffer, tcp_buffer_len);
            } else if (forward_mode == FORWARD_CLIENT) {
                tcp_client_send(tcp_buffer, tcp_buffer_len);
            }
            // Reset the buffer
            tcp_buffer_len = 0;
        }
        pthread_mutex_unlock(&tcp_buffer_mutex);
    }
    return NULL;
}

void forwarder_init(const Config *cfg) {
    if (strcmp(cfg->Tcp.mode, "server") == 0) {
        forward_mode = FORWARD_SERVER;
        tcp_server_start(cfg->Tcp.port);
    } else if (strcmp(cfg->Tcp.mode, "client") == 0) {
        forward_mode = FORWARD_CLIENT;
        tcp_client_start(cfg->Tcp.host, cfg->Tcp.port);
    } else {
        forward_mode = FORWARD_NONE;
        fprintf(stderr, "[Forwarder] Unknown TCP mode: %s\n", cfg->Tcp.mode);
    }

    use_interval = cfg->Forwarder.use_interval;
    interval_ms = cfg->Forwarder.interval_ms;

    pthread_t flush_tid;
    if (pthread_create(&flush_tid, NULL, flush_thread, NULL) != 0) {
        fprintf(stderr, "[Forwarder] Failed to create flush thread\n");
        return;
    }
    pthread_detach(flush_tid);
}

void forwarder_send(const void *data, size_t len) {

    if (use_interval == 0)
    {
        // If interval is disabled, send data immediately
        if (forward_mode == FORWARD_SERVER) {
            tcp_server_send(data, len);
        } else if (forward_mode == FORWARD_CLIENT) {
            tcp_client_send(data, len);
        } else {
            fprintf(stderr, "[Forwarder] No active TCP connection to send data\n");
        }
        return;
    }
    
    // Otherwise if interval is enabled, buffer the data
    pthread_mutex_lock(&tcp_buffer_mutex);
    if (tcp_buffer_len + len > TCP_BUFFER_SIZE) {
        fprintf(stderr, "[Forwarder] Buffer overflow, dropping data\n");
        pthread_mutex_unlock(&tcp_buffer_mutex);
        return;
    }
    memcpy(tcp_buffer + tcp_buffer_len, data, len);
    tcp_buffer_len += len;
    pthread_mutex_unlock(&tcp_buffer_mutex);
}
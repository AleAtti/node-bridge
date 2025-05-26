#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

typedef struct{
    char host[64];
    int port;
} TcpClientConfig;

void tcp_client_start(const char *host, int port);
int tcp_client_send(const void *data, size_t size);
#endif
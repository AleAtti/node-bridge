#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <stddef.h>
#define MAX_CLIENTS 10

void *tcp_server_start(int port);
int tcp_server_send(const void *data, size_t size);

#endif
#ifndef FORWARDER_H
#define FORWARDER_H
#include <stddef.h>
#include "config.h"
void forwarder_init(const Config *cfg);
void forwarder_send(const void *data, size_t len);
#endif
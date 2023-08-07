#ifndef ASCOM_ALPACA_DISCOVERY_H
#define ASCOM_ALPACA_DISCOVERY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "alpaca.h"

#define NUM_THREAD_MAX 10

#define ASCOM_DISCOVERY_DEFAULT_PORT 32227
#define ASCOM_DEFAULT_PORT 11111

#define DISCOVERY_MESSAGE "alpacadiscovery1"

typedef struct _alpacaDiscoveryConfig_t {
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    uint32_t portDiscovery;
    uint32_t portAscom;
    int socket;
    int addrlen;
} alpacaDiscoveryConfig_t;

typedef struct _alpacaDiscoveryResponse_t {
    alpacaDiscoveryConfig_t *config;
    int new_socket;
} alpacaDiscoveryResponse_t;

int initAlpacaDiscovery(alpaca_t *alpaca);

#endif

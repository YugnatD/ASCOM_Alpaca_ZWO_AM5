/*
Fichier : lib.c
Auteur : Tanguy Dietrich
Description :
*/
#include "alpacaDiscovery.h"
#include <stdio.h>
#include <stdlib.h>

static void *alpaca_discovery_thread(void *arg);
static void *alpaca_discovery_process_communication(void *arg);


int initAlpacaDiscovery(alpaca_t *alpaca)
{
    int opt = 1;
    char buffer[1024] = {0};
    alpacaDiscoveryConfig_t *config = malloc(sizeof(alpacaDiscoveryConfig_t));
    config->portDiscovery = alpaca->portDiscovery;
    config->portAscom = alpaca->portAscom;
    config->addrlen = sizeof(config->server_addr);

    int err;
    if ((config->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    config->server_addr.sin_family = AF_INET;
    config->server_addr.sin_port = htons(ASCOM_DISCOVERY_DEFAULT_PORT);
    config->server_addr.sin_addr.s_addr = INADDR_ANY; //inet_addr("");

    // Bind to the set port and IP:
    if(bind(config->socket, (struct sockaddr*)&config->server_addr, sizeof(config->server_addr)) < 0){
        printf("Couldn't bind to the port\n");
        return -1;
    }

    err = pthread_create(alpaca->discoveryThread, NULL, alpaca_discovery_thread, config);
    if (err != 0)
    {
        fprintf(stderr, "can't create thread :[%s]", strerror(err));
        return 1;
    }
    return 0;
}

// this thread wait for a connection, then run a new thread to process the communication
static void *alpaca_discovery_thread(void *arg)
{
    alpacaDiscoveryConfig_t *config = (alpacaDiscoveryConfig_t *)arg;
    fprintf(stdout, "Listening on port %d\n", config->portDiscovery);
    // int valread;
    char buffer[1024] = {0};
    while (1)
    {
        fprintf(stdout, "Waiting for message... on socket %d\n", config->socket);
        if (recvfrom(config->socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&config->client_addr, &config->addrlen) < 0){
            printf("Couldn't receive\n");
            return -1;
        }
        fprintf(stdout, "Message received: %s\n", buffer);

        // compare the message received to the discovery message
        if (strcmp(buffer, DISCOVERY_MESSAGE) == 0)
        {
            fprintf(stdout, "Discovery message received\n");
            // send the answer as a form of json string
            sprintf(buffer, "{\"AlpacaPort\": %d}", config->portAscom);
            fprintf(stdout, "Sending answer: %s\n", buffer);
            sendto(config->socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&config->client_addr, config->addrlen);
        }
    }
    return NULL;
}
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

// static pthread_t alpaca_discovery_thread[NUM_THREAD_MAX];


int alpaca_discovery(uint32_t portDiscovery, uint32_t portAscom, pthread_t *thread)
{
    int opt = 1;
    alpacaDiscoveryConfig_t *config = malloc(sizeof(alpacaDiscoveryConfig_t));
    config->portDiscovery = portDiscovery;
    config->portAscom = portAscom;
    config->addrlen = sizeof(config->address);

    int err;
    // init a socket to listen on portDiscovery
    if ((config->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 
    if (setsockopt(config->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    config->address.sin_family = AF_INET;
    config->address.sin_addr.s_addr = INADDR_ANY;
    config->address.sin_port = htons(ASCOM_DISCOVERY_DEFAULT_PORT);
  
    // Forcefully attaching socket to the port 8080
    if (bind(config->socket, (struct sockaddr*)&config->address, sizeof(config->address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(config->socket, NUM_THREAD_MAX) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    err = pthread_create(thread, NULL, alpaca_discovery_thread, config);
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
    int new_socket, valread;
    char buffer[1024] = {0};
    while (1)
    {
        if ((new_socket = accept(config->socket, (struct sockaddr*)&config->address, (socklen_t*)&config->addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read(new_socket, buffer, 1024);
        if (valread == 0)
        {
            // end of connection
            break;
        }
        else if (valread < 0)
        {
            return NULL;
        }
        else
        {
            // compare the message received to the discovery message
            if (strcmp(buffer, DISCOVERY_MESSAGE) == 0)
            {
                // send the answer as a form of json string
                sprintf(buffer, "{\"AlpacaPort\": %d}", config->portAscom);
                
            }
        }
    }
    close(new_socket);
    return NULL;
}

// this thread process the message received, answer to the request and end the connection if needed
// static void *alpaca_discovery_process_communication(void *arg)
// {
//     int socket = (int *)arg;
//     char buffer[1024] = {0};
//     int valread;
//     while (1)
//     {
//         valread = read(socket, buffer, 1024);
//         if (valread == 0)
//         {
//             // end of connection
//             break;
//         }
//         else if (valread < 0)
//         {
//             perror("read");
//             exit(EXIT_FAILURE);
//         }
//         else
//         {
//             // compare the message received to the discovery message
//             if (strcmp(buffer, DISCOVERY_MESSAGE) == 0)
//             {
//                 // send the answer as a form of json string
//                 // fprintf(buffer, "{\"AlpacaPort\": %d}", config->portAscom);
                
//             }
//         }
//     }
// }

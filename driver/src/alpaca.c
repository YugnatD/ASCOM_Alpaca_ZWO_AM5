/*
Fichier : alpaca.c
Auteur : Tanguy Dietrich
Description :
*/
#include "alpaca.h"
#include <stdio.h>
#include <stdlib.h>
#include "alpacaDiscovery.h"

void initAlpacaSocket(uint32_t portAscom, pthread_t ascomThread)
{

}

void initAlpacaDriver(alpacaConfig_t *drv)
{
    fprintf(stdout, "Device name: %s\n", drv->name);
    fprintf(stdout, "Device description: %s\n", drv->description);
    fprintf(stdout, "Device manufacturer: %s\n", drv->manufacturer);
    fprintf(stdout, "Device manufacturer version: %s\n", drv->manufacturerVersion);
    fprintf(stdout, "Device location: %s\n", drv->location);
    fprintf(stdout, "Device UUID: %s\n", drv->UUID);
    fprintf(stdout, "Device driver info: %s\n", drv->driverInfo);
    drv->init(drv->serialBus);
}

static void *threadAscom(void *arg)
{
    fprintf(stdout, "Thread ASCOM\n");
    return NULL;
}


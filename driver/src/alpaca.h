#ifndef ASCOM_ALPACA_H
#define ASCOM_ALPACA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define API_VERSION 1
#define API_NAME "api"
#define API_VERSION_STRING "v1"
#define DEVICE_NUMBER 0
#define DEFAULT_PORT 11111

typedef enum _partType {
    partTypeUnknown = 0,
    partTypeCamera = 1,
    partTypeDome = 2,
    partTypeFilterWheel = 3,
    partTypeFocuser = 4,
    partTypeMount = 5,
} partType;

typedef struct _alpacaConfig_t {
  char *name;
    char *description;
    char *manufacturer;
    char *manufacturerVersion;
    char *location;
    char *UUID;
    char *driverInfo;
    char *driverVersion;
    char *interfaceVersion;
    char **supportedActions;
    partType deviceType;
    void *deviceStruct;
    char serialBus[64];
    // function pointers (init, deinit, etc.)
    void (*init)(char *);
    void (*deinit)(void);
} alpacaConfig_t;

void initAlpacaSocket(uint32_t portAscom, pthread_t ascomThread);
void initAlpacaDriver(alpacaConfig_t *drv);

#endif

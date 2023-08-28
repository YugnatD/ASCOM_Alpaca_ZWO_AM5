#ifndef ASCOM_ALPACA_H
#define ASCOM_ALPACA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <microhttpd.h>


#define API_VERSION 1
#define API_NAME "api"
#define API_VERSION_STRING "v1"
#define DEVICE_NUMBER 0
#define DEFAULT_PORT 11111

#define MAX_DEVICE 10

typedef enum _partType {
    partTypeUnknown = 0,
    partTypeCamera = 1,
    partTypeDome = 2,
    partTypeFilterWheel = 3,
    partTypeFocuser = 4,
    partTypeMount = 5,
} partType;

typedef enum _responseTypeAlpaca {
    integerType = 0, // %d
    floatType = 1, // %f
    integerList = 2, // [%d]
    floatList = 3, // [%f]
    stringType = 4, // \"%s\"
} responseTypeAlpaca;

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

typedef struct _alpaca_t {
  alpacaConfig_t *config; // list of devices
  uint32_t numDevice;
  // alpacaDiscoveryConfig_t *discoveryConfig;
  uint32_t portAscom;
  uint32_t portDiscovery;
  pthread_t *ascomThread;
  pthread_t *discoveryThread;
} alpaca_t;



void initAlpacaSocket(alpaca_t *alpaca);
void initAlpacaDriver(alpaca_t *alpaca, alpacaConfig_t *drv);
void removeAlpacaDriver(alpacaConfig_t *drv);
int requestResponse(char *buffer, char *value, int32_t errorNumber, char *errorMessage, struct MHD_Connection * connection, uint32_t clientTransactionID);
void castUploadData(char* uploadData, uint32_t sizeUpload, uint32_t *clientTransID, uint32_t *clientID, char *ptrExtra);
void generateValueReponse(void *data, responseTypeAlpaca t, uint32_t num, char *str);

#endif

/*
Fichier : alpaca.c
Auteur : Tanguy Dietrich
Description :
*/
#include "alpaca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <microhttpd.h>
// #include <restinio/all.hpp>
// Oat++
// https://www.gnu.org/software/libmicrohttpd/ // sudo apt-get install -y libmicrohttpd-dev
#include "alpacaDiscovery.h"

static void *threadAscom(void *arg);

#define DEFAULT_RESPONSE "UNKNOWN REQUEST"
#define RESPONSE_TEST_1 "RESPONSE1"
#define RESPONSE_TEST_2 "RESPONSE2"

            

/**
 * A client has requested the given url using the given method
 * (#MHD_HTTP_METHOD_GET, #MHD_HTTP_METHOD_PUT,
 * #MHD_HTTP_METHOD_DELETE, #MHD_HTTP_METHOD_POST, etc).  The callback
 * must call MHD callbacks to provide content to give back to the
 * client and return an HTTP status code (i.e. #MHD_HTTP_OK,
 * #MHD_HTTP_NOT_FOUND, etc.).
 *
 * @param cls argument given together with the function
 *        pointer when the handler was registered with MHD
 * @param url the requested url
 * @param method the HTTP method used (#MHD_HTTP_METHOD_GET,
 *        #MHD_HTTP_METHOD_PUT, etc.)
 * @param version the HTTP version string (i.e.
 *        #MHD_HTTP_VERSION_1_1)
 * @param upload_data the data being uploaded (excluding HEADERS,
 *        for a POST that fits into memory and that is encoded
 *        with a supported encoding, the POST data will NOT be
 *        given in upload_data and is instead available as
 *        part of #MHD_get_connection_values; very large POST
 *        data *will* be made available incrementally in
 *        @a upload_data)
 * @param[in,out] upload_data_size set initially to the size of the
 *        @a upload_data provided; the method must update this
 *        value to the number of bytes NOT processed;
 * @param[in,out] con_cls pointer that the callback can set to some
 *        address and that will be preserved by MHD for future
 *        calls for this request; since the access handler may
 *        be called many times (i.e., for a PUT/POST operation
 *        with plenty of upload data) this allows the application
 *        to easily associate some request-specific state.
 *        If necessary, this state can be cleaned up in the
 *        global #MHD_RequestCompletedCallback (which
 *        can be set with the #MHD_OPTION_NOTIFY_COMPLETED).
 *        Initially, `*con_cls` will be NULL.
 * @return #MHD_YES if the connection was handled successfully,
 *         #MHD_NO if the socket must be closed due to a serios
 *         error while handling the request
 */
static int process_request(void *cls, struct MHD_Connection * connection, const char * url, const char * method, const char * version, const char * upload_data, size_t * upload_data_size, void ** ptr) {
    static int dummy;
    // const char * page = cls;
    alpaca_t *alpaca = (alpaca_t *)cls;
    struct MHD_Response * response;
    int ret, intValue;
    float floatValue;
    char strResponse[8192];
    char Value[4096];
    uint32_t clientTransID, clientID=0;
    char *ptrExtra;
    printf("URL: %s\n", url); // "/"
    printf("Method: %s\n", method); // "GET"
    printf("Version: %s\n", version); // "HTTP/1.1"
    printf("Upload data: %s\n", upload_data); // null / Upload data: ClientTransactionID=24&ClientID=31927&Connected=True
    printf("Upload data size: %d\n", *upload_data_size); // 0 / 52
    // printf("Response: %d\n", ret); // "1"
    printf("\r\n");
    // Accept GET AND PUT request
    if (0 != strcmp(method, "GET") && 0 != strcmp(method, "PUT")){return MHD_NO;}
    /* The first time only the headers are valid, do not respond in the first round... */ 
    if (&dummy != *ptr){*ptr = &dummy;return MHD_YES;}
    // if (0 != *upload_data_size){ return MHD_NO; /* upload data in a GET!? */}
    *ptr = NULL; /* clear context pointer */
    // printf("URL: %s\n", url); // "/"
    // printf("Method: %s\n", method); // "GET"
    // printf("Version: %s\n", version); // "HTTP/1.1"
    // printf("Upload data: %s\n", upload_data); // null
    // printf("Upload data size: %d\n", *upload_data_size); // 0
    // printf("Response: %d\n", ret); // "1"
    castUploadData((char *)upload_data, *upload_data_size, &clientTransID, &clientID, ptrExtra);

    // TODO : move to a function to process the request
    if (0 == strcmp(url, "/")) {
        response = MHD_create_response_from_buffer (strlen(DEFAULT_RESPONSE),(void*) DEFAULT_RESPONSE, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    } else if (0 == strcmp(url, "/api/v1/")) {
        // make a list of all device available on alpaca_t
        response = MHD_create_response_from_buffer (strlen(RESPONSE_TEST_1),(void*) RESPONSE_TEST_1, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    } else if (0 == strcmp(url, "/api/v1/telescope")) {
        response = MHD_create_response_from_buffer (strlen(RESPONSE_TEST_2),(void*) RESPONSE_TEST_2, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    } else if (0 == strcmp(url, "/api/v1/telescope/0/connected")) {
        intValue = 1;
        generateValueReponse((void*)&intValue, integerType, 0, Value);
        ret = requestResponse(strResponse, Value, 0, "", connection, clientTransID);
    } else if (0 == strcmp(url, "/management/apiversions")) {
        // sprintf(strResponse, "{\"Value\": [%d], \"ClientTransactionID\": %d, \"ServerTransactionID\": %d, \"ErrorNumber\": %d, \"ErrorMessage\": \"%s\"}", 1, 0, 0, 0, "");
        // response = MHD_create_response_from_buffer (strlen(strResponse),(void*) strResponse, MHD_RESPMEM_PERSISTENT);
        // ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        intValue = 1;
        generateValueReponse((void*)&intValue, integerList, 0, Value);
        ret = requestResponse(strResponse, Value, 0, "", connection, clientTransID);
    } else if (0 == strcmp(url, "/management/v1/description")) {
        // sprintf(Value, "{\"ServerName\": \"%s\", \"Manufacturer\": \"%s\", \"ManufacturerVersion\": \"%s\", \"Location\": \"%s\"}", alpaca->config[0].name, alpaca->config[0].manufacturer, alpaca->config[0].manufacturerVersion, alpaca->config[0].location);
        // sprintf(strResponse, "{\"Value\": %s, \"ClientTransactionID\": %d, \"ServerTransactionID\": %d, \"ErrorNumber\": %d, \"ErrorMessage\": \"%s\"}", Value, 0, 0, 0, "");
        // response = MHD_create_response_from_buffer (strlen(strResponse),(void*) strResponse, MHD_RESPMEM_PERSISTENT);
        // ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        printf("---------------------------------------------Description\r\n");
        generateValueReponse((void*)alpaca->config[0].name, stringType, 0, Value);
        ret = requestResponse(strResponse, Value, 0, "", connection, clientTransID);
    } else if (0 == strcmp(url, "/management/v1/configureddevices")) {
        // Value = [{"DeviceName": self.name, "DeviceType": DEVICE_TYPE, "DeviceNumber": int(DEVICE_NUMBER), "UniqueID": self.UUID}]
        sprintf(Value, "[{\"DeviceName\": \"%s\", \"DeviceType\": \"telescope\", \"DeviceNumber\": %d, \"UniqueID\": \"%s\"}]", alpaca->config[0].name, DEVICE_NUMBER, alpaca->config[0].UUID);
        sprintf(strResponse, "{\"Value\": %s, \"ClientTransactionID\": %d, \"ServerTransactionID\": %d, \"ErrorNumber\": %d, \"ErrorMessage\": \"%s\"}", Value, 0, 0, 0, "");
        response = MHD_create_response_from_buffer (strlen(strResponse),(void*) strResponse, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    } else if (0 == strcmp(url, "/api/v1/telescope/0/interfaceversion")) {
        // interface verions == "1"
        sprintf(strResponse, "{\"Value\": \"1\", \"ClientTransactionID\": %d, \"ServerTransactionID\": %d, \"ErrorNumber\": %d, \"ErrorMessage\": \"%s\"}", 0, 0, 0, "");
        response = MHD_create_response_from_buffer (strlen(strResponse),(void*) strResponse, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    } else {
        response = MHD_create_response_from_buffer (strlen(DEFAULT_RESPONSE),(void*) DEFAULT_RESPONSE, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    }
    MHD_destroy_response(response);
    return ret;
}

// Example of request :
// uploadData="ClientTransactionID=24&ClientID=31927&Connected=True", sizeUpload = 52
// uploadData=NULL, sizeUpload = 0
// function return clientTransID = 24, clientID = 31927, ptrExtra = 'Connected=True'
void castUploadData(char* uploadData, uint32_t sizeUpload, uint32_t *clientTransID, uint32_t *clientID, char *ptrExtra) {
    *clientTransID = 0;
    *clientID = 0;
    // *ptrExtra = '\0'; // Zero-terminated string
    
    // Start by checking if uploadData is NULL
    if (uploadData == NULL || sizeUpload == 0) {
        return;
    }

    // Find the position of '=' in the uploadData
    char *equalSign = strchr(uploadData, '=');
    if (equalSign == NULL) {
        return;
    }

    // Parse ClientTransactionID
    *clientTransID = atoi(equalSign + 1);

    // Find the position of '&' in the uploadData
    char *ampersand = strchr(uploadData, '&');
    if (ampersand == NULL) {
        return;
    }

    // Find the position of '=' after the first '&'
    equalSign = strchr(ampersand, '=');
    if (equalSign == NULL) {
        return;
    }

    // Parse ClientID
    *clientID = atoi(equalSign + 1);

    // Find the position of the second '=' in uploadData
    equalSign = strchr(equalSign + 1, '=');
    if (equalSign == NULL) {
        return;
    }

    // Copy the remaining part after the second '=' into ptrExtra
    // strcpy(ptrExtra, equalSign + 1);
    // for now we will only make a ptr to the string
    ptrExtra = equalSign + 1;
}

// this create a response in format :
// {"Value": Value, "ClientTransactionID": ClientTransactionID, "ServerTransactionID":  ServerTransID, "ErrorNumber": ErrorNumber, "ErrorMessage": ErrorMessage}
// Value is the value of the request
// ServerTransactionID is the transaction ID of the request (random number)
// ClientTransactionID is the transaction ID of the request 
// ErrorNumber is the error number of the request (0 if no error)
// ErrorMessage is the error message of the request (empty if no error) ""
int requestResponse(char *buffer, char *value, int32_t errorNumber, char *errorMessage, struct MHD_Connection * connection, uint32_t clientTransactionID)
{
    int ret;
    struct MHD_Response * response;
    int32_t ServerTransactionID = 0;
    ServerTransactionID = rand();
    sprintf(buffer, "{\"Value\": %s, \"ClientTransactionID\": %d, \"ServerTransactionID\": %d, \"ErrorNumber\": %d, \"ErrorMessage\": \"%s\"}", value, clientTransactionID, ServerTransactionID, errorNumber, errorMessage);
    response = MHD_create_response_from_buffer (strlen(buffer),(void*) buffer, MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    printf("Response: %s\n", buffer);
    // MHD_destroy_response(response); // done in the process_request, don't do it here 
    return ret;
}

// this create a response depending of the format
// void *data is the data to send
// responseTypeAlpaca t is the type of data to send
// uint32_t num is the number of data to send, if 0 send only one data
// char *str is the out string generated
void generateValueReponse(void *data, responseTypeAlpaca t, uint32_t num, char *str)
{
    int *a;
    float *b;
    char *c;
    switch (t)
    {
    case integerType:
        a = (int*)data;
        sprintf(str, "%d", *a);
        break;
    case floatType:
        b = (float*)data;
        sprintf(str, "%f", *b);
        break;
    case integerList:
        a = (int*)data;
        sprintf(str, "[%d]", *a);
        break;
    case floatList:
        b = (float*) data;
        sprintf(str, "[%f]", *b);
        break;
    case stringType:
        c = (char *)data;
        sprintf(str, "\"%s\"", c);
        break;
    default:
        break;
    }
}


void initAlpacaSocket(alpaca_t *alpaca)
{
    fprintf(stdout, "Port ASCOM: %d\n", alpaca->portAscom);
    pthread_create(alpaca->ascomThread, NULL, threadAscom, alpaca);
}

void initAlpacaDriver(alpaca_t *alpaca, alpacaConfig_t *drv)
{
    // add driver to list
    // allocate memory for new driver
    // ACTUALLY SUPPORT ONLY TEN DRIVER BE CAREFUL, YOU CAN CHANGE THIS IN alpaca.h
    // TODO ADD A RESIZEABLE ARRAY OF DRIVER USING A STRUCTURE
    alpaca->config = (alpacaConfig_t *)malloc(MAX_DEVICE * sizeof(alpacaConfig_t));
    alpaca->config[alpaca->numDevice] = *drv;
    alpaca->numDevice++;
    fprintf(stdout, "Device name: %s\n", drv->name);
    fprintf(stdout, "Device description: %s\n", drv->description);
    fprintf(stdout, "Device manufacturer: %s\n", drv->manufacturer);
    fprintf(stdout, "Device manufacturer version: %s\n", drv->manufacturerVersion);
    fprintf(stdout, "Device location: %s\n", drv->location);
    fprintf(stdout, "Device UUID: %s\n", drv->UUID);
    fprintf(stdout, "Device driver info: %s\n", drv->driverInfo);
    drv->init(drv->serialBus);
}

void removeAlpacaDriver(alpacaConfig_t *drv)
{
    drv->deinit();
}

static void *threadAscom(void *arg)
{
    alpaca_t *alpaca = (alpaca_t *)arg;
    fprintf(stdout, "Port ASCOM: %d\n", alpaca->portAscom);
    struct MHD_Daemon * d;
    // d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, portAscom, NULL, NULL, &process_request, NULL, MHD_OPTION_END);
    d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, alpaca->portAscom, NULL, NULL, &process_request, alpaca, MHD_OPTION_END);
    if (d == NULL){fprintf(stderr, "Error starting server\n"); exit(EXIT_FAILURE);}
    (void) getc (stdin);
    MHD_stop_daemon(d);
    return NULL;
}


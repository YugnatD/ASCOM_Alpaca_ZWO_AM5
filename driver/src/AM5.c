/*
Fichier : AM5.c
Auteur : Tanguy Dietrich
Description :
*/
#include "AM5.h"
#include <stdio.h>
#include <stdlib.h>

// This driver is greatly inspired by the indigo driver for the same device
// https://github.com/indigo-astronomy/indigo/tree/master/indigo_drivers/mount_asi
// thanks to the indigo team for their work

telescope_t zwo_am5_telescope_config = {
    .alignementMode = 1,
    .altitude = 46.09098,
    .apertureArea = 3.14,
    .apertureDiameter = 3.14,
    .atHome = false,
    .atPark = false,
    .azimuth = 0.0,
    .canFindHome = false,
    .canPark = false,
    .canPulseGuide = false,
    .canSetDeclinationRate = false,
    .canSetGuideRates = false,
    .canSetPark = false,
    .canSetPierSide = false,
    .canSetRightAscensionRate = false,
    .canSetTracking = false,
    .canSlew = false,
    .canSlewAltAz = false,
    .canSlewAltAzAsync = false,
    .canSlewAsync = false,
    .canSync = false,
    .canSyncAltAz = false,
    .canUnpark = false,
    .declination = 0.0,
    .declinationRate = 0.0,
    .doesRefraction = false,
    .equatorialSystem = 0,
    .focalLength = 0.0,
    .guideRateDeclination = 0.0,
    .guideRateRightAscension = 0.0,
    .isPulseGuiding = false,
    .rightAscension = 0.0,
    .rightAscensionRate = 0.0,
    .sideOfPier = 0,
    .siderealTime = 0.0,
    .siteElevation = 0.0,
    .siteLatitude = 0.0,
    .siteLongitude = 0.0,
    .slewing = false,
    .slewSettleTime = 0,
    .targetDeclination = 0.0,
    .targetRightAscension = 0.0,
    .tracking = false,
    .trackingRate = 0,
    .trackingRates = NULL,
    .trackingRatesCount = 0,
    .utcDate = NULL,
    .axisRates = NULL,
    .axisRatesCount = 0,
    .canMoveAxis = false,
    .destinationSideOfPier = 0
};


alpacaConfig_t zwo_am5_alpaca_config = {
    .name = ZWO_AM5_NAME,
    .description = ZWO_AM5_DESCRIPTION,
    .manufacturer = ZWO_AM5_MANUFACTURER,
    .manufacturerVersion = ZWO_AM5_MANUFACTURER_VERSION,
    .location = ZWO_AM5_LOCATION,
    .UUID = ZWO_AM5_UUID,
    .driverInfo = ZWO_AM5_DRIVER_INFO,
    .driverVersion = ZWO_AM5_DRIVER_VERSION,
    .interfaceVersion = ZWO_AM5_INTERFACE_VERSION,
    .supportedActions = ZWO_AM5_SUPPORTED_ACTIONS,
    .deviceType = ZWO_AM5_DEVICE_TYPE,
    .deviceStruct = &zwo_am5_telescope_config,
    .serialBus = "",
    .init = asi_init_AM5,
    .deinit = asi_deinit_AM5
};

// Private internal variables
static int fd_AM5 = -1;



void asi_init_AM5(char *device)
{
    // print to stdout
    fprintf(stdout, "initAM5: %s\n", device);
    // open the device
    fd_AM5 = open(device, O_RDWR | O_NOCTTY);
    if (fd_AM5 == -1)
    {
        fprintf(stderr, "Error opening device %s\n", device);
        exit(1);
    }
    // set parameter of serial port :
    struct termios tty;
    struct termios tty_old;
    memset (&tty, 0, sizeof tty);

    /* Error Handling */
    if ( tcgetattr ( fd_AM5, &tty ) != 0 ) {
        printf("Error %d from tcgetattr: %s\n", errno, strerror(errno));
    }

    /* Save old tty parameters */
    tty_old = tty;

    /* Set Baud Rate */
    cfsetospeed (&tty, (speed_t)B9600);
    cfsetispeed (&tty, (speed_t)B9600);

    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    /* Make raw */
    cfmakeraw(&tty);

    /* Flush Port, then applies attributes */
    tcflush( fd_AM5, TCIFLUSH );
    if ( tcsetattr ( fd_AM5, TCSANOW, &tty ) != 0) {
    printf("Error %d from tcsetattr: %s\n", errno, strerror(errno));
    }

    //check if the mount is an AM5
    telescope_response_t ret;
    ret = asi_get_mount_model();
    //TODO : compare ret with AM5
}

void asi_deinit_AM5()
{
    // print to stdout
    fprintf(stdout, "deinitAM5\n");
    // close the device
    close(fd_AM5);
}


//
// Commands usually start with “:” and end with “#”, such as “:AA#”. Fields in the
// middle represent the command itself.
void asi_send_command_AM5(char *cmd)
{
    write(fd_AM5, cmd, strlen(cmd));
}

// response of the serial port
// 1) No response. Some commands do not require response values, such as moving
// commands.
// 2) Response 0 or 1. It stands for success or failure of the command. Success for GOTO
// commands is 0, for other commands is 1.
// 3) Response 0 or 1 or e+ error code+#. For example, e1#, the number 1 stands for
// error code. You can check the explanation below to see the reason.
// 4) Response character string ends with +#. For example, HH:MM:SS#, it was used to
// response data.
// 5) Response N/A# or e+ error code# (Sync commands).
// Note: The response values of Get Commands starting with G all comes with #.
telescope_response_t asi_receive_command_AM5()
{
    telescope_response_t ret;
    char response[RESPONSE_SIZE];
    int n = read(fd_AM5, response, RESPONSE_SIZE);
    // set default value of the response
    ret.errorNumber = 0;
    ret.success = true;
    ret.additionalInfoLength = n;
    ret.additionalInfo = malloc(n);
    memcpy(ret.additionalInfo, response, n);
    //then try to cast the answer to see if it contains an error code
    // TODO 
    return ret;
}

telescope_response_t asi_send_receive_command_AM5(char *cmd)
{
    telescope_response_t ret;
    // send the command
    asi_send_command_AM5(cmd);
    // read the response
    ret = asi_receive_command_AM5();
    return ret;
}

telescope_response_t asi_get_version()
{
    return asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_VERSION);
}

telescope_response_t asi_get_mount_model()
{
    return asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_MOUNT_MODEL);
}


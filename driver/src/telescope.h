#ifndef ASCOM_ALPACA_TELESCOPE_H
#define ASCOM_ALPACA_TELESCOPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


typedef struct _telescope_t { 
    uint8_t alignementMode;
    double altitude;
    double apertureArea;
    double apertureDiameter;
    bool atHome;
    bool atPark;
    double azimuth;
    bool canFindHome;
    bool canPark;
    bool canPulseGuide;
    bool canSetDeclinationRate;
    bool canSetGuideRates;
    bool canSetPark;
    bool canSetPierSide;
    bool canSetRightAscensionRate;
    bool canSetTracking;
    bool canSlew;
    bool canSlewAltAz;
    bool canSlewAltAzAsync;
    bool canSlewAsync;
    bool canSync;
    bool canSyncAltAz;
    bool canUnpark;
    double declination;
    double declinationRate;
    bool doesRefraction;
    uint32_t equatorialSystem;
    double focalLength;
    double guideRateDeclination;
    double guideRateRightAscension;
    bool isPulseGuiding;
    double rightAscension;
    double rightAscensionRate;
    int32_t sideOfPier;
    double siderealTime;
    double siteElevation;
    double siteLatitude;
    double siteLongitude;
    bool slewing;
    uint32_t slewSettleTime;
    double targetDeclination;
    double targetRightAscension;
    bool tracking;
    uint32_t trackingRate;
    uint32_t *trackingRates;
    uint32_t trackingRatesCount;
    char *utcDate; // "2016-11-14T07:03:08.1234567Z"
    uint32_t *axisRates;
    uint32_t axisRatesCount;
    bool canMoveAxis;
    int32_t destinationSideOfPier;
} telescope_t;

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
typedef struct _telescope_response_t {
    bool success;
    uint8_t errorNumber;
    char *additionalInfo;
    uint32_t additionalInfoLength;
} telescope_response_t;


void telescope_free_response(telescope_response_t *response);
#endif

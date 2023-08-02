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
        // exit(1);
        return ;
    }
    // set parameter of serial port :
    struct termios tty;
    struct termios tty_old;
    memset (&tty, 0, sizeof tty);

    /* Error Handling */
    if ( tcgetattr ( fd_AM5, &tty ) != 0 ) {
        fprintf(stderr, "Error %d from tcgetattr: %s\n", errno, strerror(errno));
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
        fprintf(stderr, "Error %d from tcsetattr\n", errno);
    }

    //check if the mount is an AM5
    telescope_response_t ret;
    ret = asi_get_mount_model(); // should answer AM5#
    fprintf(stdout, "Mount model : %s\n", ret.additionalInfo);
    if(strcmp(ret.additionalInfo, "AM5#") != 0)
    {
        fprintf(stderr, "Error, the mount is not an AM5\r\n");
        return ;
    }
    telescope_free_response(&ret);
    // check the version : 
    ret = asi_get_version(); // should answer
    fprintf(stdout, "Version : %s\n", ret.additionalInfo);
    // actually, do not check the version, because, it is not implemented in the driver
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
    int n = read(fd_AM5, response, RESPONSE_SIZE); // TODO : implement a timeout
    // set default value of the response
    ret.errorNumber = 0;
    ret.success = true;
    ret.additionalInfoLength = n;
    ret.additionalInfo = malloc(n+1); // +1 for the \0
    memcpy(ret.additionalInfo, response, n);
    ret.additionalInfo[n] = '\0';
    //then try to cast the answer to see if it contains an error code, or a success code
    if(ret.additionalInfo[0] == 'e' && n==3)// example : "e7#", error number is 7
    {
        ret.success = false;
        ret.errorNumber = atoi(ret.additionalInfo + 1); 
    } else if(ret.additionalInfo[0] == '0' && n==1) // failure "0", check the size too, because it could fail with this kind of answer "01/01/00#" from get_utc
    {
        ret.success = false;
        ret.errorNumber = 0;
    } else if(ret.additionalInfo[0] == '1' && n==1) // success "1"
    {
        ret.success = true;
        ret.errorNumber = -1;
    } else // something else, maybe a coordinate, a version number, etc... ("AM5#", "1.3.0#", "01/01/00#, )
    {
        // I consider it as a success
        // fprintf(stdout, "Unknown answer : %s\n", ret.additionalInfo);
        ret.success = true;
        ret.errorNumber = -1;
    }
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

// GETTER FUNCTIONS
telescope_response_t asi_get_version()
{
    return asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_VERSION);
}

telescope_response_t asi_get_mount_model()
{
    return asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_MOUNT_MODEL);
}

telescope_response_t asi_get_utc(time_t *secs, int *utc_offset)
{
    telescope_response_t ret;
    char command[COMMAND_SIZE];
    NOT_IMPLEMENTED
    // get ZWO_AM5_CMD_GET_DATE_TIME_MDY
    ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_DATE_TIME_MDY);
    printf("ret.additionalInfo : %s\n", ret.additionalInfo); // 01/01/00#
    if (ret.success == false){return ret;}
    // get ZWO_AM5_CMD_GET_DATE_TIME_HMS
    ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_DATE_TIME_HMS);
    printf("ret.additionalInfo : %s\n", ret.additionalInfo); // 00:50:24#
    if (ret.success == false){return ret;}
    // get ZWO_AM5_CMD_GET_TIME_ZONE
    ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_TIME_ZONE);
    printf("ret.additionalInfo : %s\n", ret.additionalInfo); // +00:00#
    return ret;
}

telescope_response_t asi_get_sidereal_time(double *sideral_time)
{
    telescope_response_t ret;
    char command[COMMAND_SIZE];
    NOT_IMPLEMENTED
    // get ZWO_AM5_CMD_GET_SIDERAL_TIME
    ret.success = false;
    return ret;
}

telescope_response_t asi_get_site(double *latitude, double *longitude)
{
    telescope_response_t ret;
    char command[COMMAND_SIZE];
    NOT_IMPLEMENTED // TODO : convert the coordinates
    // get ZWO_AM5_CMD_GET_LATITUDE
    ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_LATITUDE);
    printf("ret.additionalInfo : %s\n", ret.additionalInfo); // +21*17:00#

    // get ZWO_AM5_CMD_GET_LONGITUDE
    ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_GET_LONGITUDE);
    printf("ret.additionalInfo : %s\n", ret.additionalInfo); // -006*06:03#
    // ret.success = false;
    return ret;
}

telescope_response_t asi_get_meridian_settings(bool *flip_enabled, bool *track_passed, int *track_passed_limit)
{
    telescope_response_t ret;
    char command[COMMAND_SIZE];
    NOT_IMPLEMENTED
    // get ZWO_AM5_CMD_GET_MERIDIAN_SETTINGS

    ret.success = false;
    return ret;
}

telescope_response_t asi_get_coordinates(double *ra, double *dec)
{
    telescope_response_t ret;
    char command[COMMAND_SIZE];
    NOT_IMPLEMENTED
    // get ZWO_AM5_CMD_GET_COORDINATES_RA

    // get ZWO_AM5_CMD_GET_COORDINATES_DEC
    ret.success = false;
    return ret;
}

telescope_response_t asi_get_guide_rate(double *ra, double *dec)
{
    telescope_response_t ret;
    char command[COMMAND_SIZE];
    NOT_IMPLEMENTED
    // get ZWO_AM5_CMD_GET_SLEW_RATE
    ret.success = false;
    return ret;
}

telescope_response_t asi_get_tracking_status(bool *tracking)
{
    telescope_response_t ret;
    char command[COMMAND_SIZE];
    NOT_IMPLEMENTED
    // get ZWO_AM5_CMD_GET_TRACKING_STATUS
    ret.success = false;
    return ret;
}


// SETTER FUNCTIONS 
telescope_response_t asi_set_utc_date(time_t *secs, int utc_offset)
{
    char command[COMMAND_SIZE];
    time_t seconds = *secs + utc_offset * 3600;
	struct tm tm;
    telescope_response_t ret;
	gmtime_r(&seconds, &tm);
	// SC ZWO_AM5_CMD_SET_DATE_TIME_MDY
    sprintf(command, ZWO_AM5_CMD_SET_DATE_TIME_MDY, tm.tm_mon + 1, tm.tm_mday, tm.tm_year % 100);
    ret = asi_send_receive_command_AM5(command);
    if (ret.success == false){return ret;}
    // SG ZWO_AM5_CMD_SET_TIME_ZONE
    sprintf(command, ZWO_AM5_CMD_SET_TIME_ZONE, -utc_offset);
    ret = asi_send_receive_command_AM5(command);
    if (ret.success == false){return ret;}
    // SL ZWO_AM5_CMD_SET_DATE_TIME_HMS
    sprintf(command, ZWO_AM5_CMD_SET_DATE_TIME_HMS, tm.tm_hour, tm.tm_min, tm.tm_sec);
    ret = asi_send_receive_command_AM5(command);
    // if (ret.success == false){return ret;}
    return ret;
}

telescope_response_t asi_set_site(double latitude, double longitude)
{
    char command[COMMAND_SIZE];
    telescope_response_t ret;
    ret.success = false;
    NOT_IMPLEMENTED
    // set latitude : ZWO_AM5_CMD_SET_LATITUDE
    sprintf(command, ZWO_AM5_CMD_SET_LATITUDE, latitude, latitude); // TODO: convert to real coordinate
    ret = asi_send_receive_command_AM5(command);
    if (ret.success == false){return ret;}
    // set longitude : ZWO_AM5_CMD_SET_LONGITUDE
    sprintf(command, ZWO_AM5_CMD_SET_LONGITUDE, longitude, longitude); // TODO: convert to real coordinate
    ret = asi_send_receive_command_AM5(command);
    // if (ret.success == false){return ret;}
    return ret;
}

telescope_response_t asi_set_meridian_action(bool flip, bool track)
{
    char command[COMMAND_SIZE];
    telescope_response_t ret;
    ret.success = false;
    NOT_IMPLEMENTED
    // set meridian action : ZWO_AM5_CMD_SET_MERIDIAN_SETTING
    return ret;
}

telescope_response_t asi_set_meridian_limit(int16_t limit)
{
    char command[COMMAND_SIZE];
    telescope_response_t ret;
    ret.success = false;
    NOT_IMPLEMENTED
    // set meridian limit : ZWO_AM5_CMD_SET_MERIDIAN_SETTING
    return ret;
}

telescope_response_t asi_slew(double ra, double dec)
{
    double d, m, s;
    char command[COMMAND_SIZE];
    telescope_response_t ret;
    // set RA : ZWO_AM5_CMD_SET_SLEW_RA
    telescope_double_to_dms(ra, &d, &m, &s);
    sprintf(command, ZWO_AM5_CMD_SET_SLEW_RA, (int32_t)d, (int32_t)m, s);
    ret = asi_send_receive_command_AM5(command);
    if (ret.success == false){return ret;}
    // set DEC : ZWO_AM5_CMD_SET_SLEW_DEC
    telescope_double_to_dms(dec, &d, &m, &s);
    sprintf(command, ZWO_AM5_CMD_SET_SLEW_DEC, (int32_t)d, (int32_t)m, s);
    ret = asi_send_receive_command_AM5(command);
    if (ret.success == false){return ret;}
    // then goto : ZWO_AM5_CMD_SET_GOTO
    ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_GOTO);
    return ret;
}

telescope_response_t asi_sync(double ra, double dec)
{
    double d, m, s;
    char command[COMMAND_SIZE];
    telescope_response_t ret;
    // set RA : ZWO_AM5_CMD_SET_SYNC_RA
    telescope_double_to_dms(ra, &d, &m, &s);
    sprintf(command, ZWO_AM5_CMD_SET_SYNC_RA, (int32_t)d, (int32_t)m, s);
    ret = asi_send_receive_command_AM5(command);
    if (ret.success == false){return ret;}
    // set DEC : ZWO_AM5_CMD_SET_SYNC_DEC
    telescope_double_to_dms(dec, &d, &m, &s);
    sprintf(command, ZWO_AM5_CMD_SET_SYNC_DEC, (int32_t)d, (int32_t)m, s);
    ret = asi_send_receive_command_AM5(command);
    if (ret.success == false){return ret;}
    // set sync : ZWO_AM5_CMD_SET_SYNC
    ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_SYNC);
    return ret;
}


telescope_response_t asi_set_guide_rate(double ra, double dec)
{
    char command[COMMAND_SIZE];
    telescope_response_t ret;
    // DEC not used, because, the two rates are linked
    if(ra != dec){fprintf(stderr, "WARNING : RA and DEC rates are linked, DEC rate will be ignored\r\n");}
    // set guide rate : ZWO_AM5_CMD_SET_GUIDE_RATE
    sprintf(command, ZWO_AM5_CMD_SET_GUIDERATE, ra);
    ret = asi_send_receive_command_AM5(command); // TODO : CHECK PROTOCOL, THIS SETTER DO NOT WORK, BLOCKED HERE
    return ret;
}

telescope_response_t asi_set_tracking(bool tracking)
{
    char command[COMMAND_SIZE];
    telescope_response_t ret;
    if(tracking)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_TRACKING_ON);
    }
    else
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_TRACKING_OFF);
    }
    return ret;
}

telescope_response_t asi_set_tracking_rate(track_mode_t mode)
{
    telescope_response_t ret;
    if(mode == TRACK_LUNAR)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_TRACKING_LUNAR);
    }
    else if(mode == TRACK_SOLAR)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_TRACKING_SOLAR);
    }
    else if(mode == TRACK_SIDEREAL)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_TRACKING_SIDEREAL);
    }
    else
    {
        ret.success = false;
        fprintf(stderr, "Unknown tracking mode\r\n");
    }
    return ret;
}

telescope_response_t asi_set_slew_rate(uint32_t rate)
{
    char command[COMMAND_SIZE];
    sprintf(command, ZWO_AM5_CMD_SET_SLEW_RATE, rate);
    return asi_send_receive_command_AM5(command);
}

telescope_response_t asi_motion_dec(direction_t dir)
{
    telescope_response_t ret;
    if(dir == DIR_POSITIVE)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_MOVING_START_NORTH);
    }
    else if(dir == DIR_NEGATIVE)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_MOVING_START_SOUTH);
    }
    else if(dir == DIR_STOP)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_MOVING_STOP_NORTH);
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_MOVING_STOP_SOUTH);
    }
    else
    {
        ret.success = false;
        fprintf(stderr, "Unknown direction\r\n");
    }
    return ret;
}

telescope_response_t asi_motion_ra(direction_t dir)
{
    telescope_response_t ret;
    if(dir == DIR_POSITIVE)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_MOVING_START_EAST);
    }
    else if(dir == DIR_NEGATIVE)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_MOVING_START_WEST);
    }
    else if(dir == DIR_STOP)
    {
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_MOVING_STOP_EAST);
        ret = asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_MOVING_STOP_WEST);
    }
    else
    {
        ret.success = false;
        fprintf(stderr, "Unknown direction\r\n");
    }
    return ret;
}

telescope_response_t asi_home()
{
    return asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_HOME_POSITION);
}

telescope_response_t asi_park()
{
    return asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_GOTO_PARK_POSITION);
}

telescope_response_t asi_stop()
{
    return asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_STOP_ALL);
}

telescope_response_t asi_clear_alignment_data()
{
    return asi_send_receive_command_AM5(ZWO_AM5_CMD_SET_CLEAR_ALIGNMENT);
}

telescope_response_t asi_guide_north(uint32_t duration)
{
    char command[COMMAND_SIZE];
    sprintf(command, ZWO_AM5_CMD_SET_GUIDE_PULSE_NORTH, duration);
    return asi_send_receive_command_AM5(command);
}

telescope_response_t asi_guide_south(uint32_t duration)
{
    char command[COMMAND_SIZE];
    sprintf(command, ZWO_AM5_CMD_SET_GUIDE_PULSE_SOUTH, duration);
    return asi_send_receive_command_AM5(command);
}

telescope_response_t asi_guide_east(uint32_t duration)
{
    char command[COMMAND_SIZE];
    sprintf(command, ZWO_AM5_CMD_SET_GUIDE_PULSE_EAST, duration);
    return asi_send_receive_command_AM5(command);
}

telescope_response_t asi_guide_west(uint32_t duration)
{
    char command[COMMAND_SIZE];
    sprintf(command, ZWO_AM5_CMD_SET_GUIDE_PULSE_WEST, duration);
    return asi_send_receive_command_AM5(command);
}

telescope_response_t asi_set_buzzer_volume(uint32_t volume)
{
    char command[COMMAND_SIZE];
    sprintf(command, ZWO_AM5_CMD_SET_BUZZER_VOLUME, volume);
    return asi_send_receive_command_AM5(command);
}










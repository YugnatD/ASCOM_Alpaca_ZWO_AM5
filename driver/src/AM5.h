#ifndef ZWO_AM5_H
#define ZWO_AM5_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <pthread.h>
#include "telescope.h"
#include "alpaca.h"

#define BAUDRATE 9600
#define COMMAND_SIZE 128
#define RESPONSE_SIZE 128

#define NOT_IMPLEMENTED printf("NOT IMPLEMENTED YET\r\n");

#define ZWO_AM5_NAME "ZWO AM5"
#define ZWO_AM5_DESCRIPTION "ZWO AM5"
#define ZWO_AM5_MANUFACTURER "ZWO"
#define ZWO_AM5_MANUFACTURER_VERSION "0.1"
#define ZWO_AM5_LOCATION "Geneve"
#define ZWO_AM5_UUID "424242424242"
#define ZWO_AM5_DRIVER_INFO "ZWO AM5"
#define ZWO_AM5_DRIVER_VERSION "0.1"
#define ZWO_AM5_INTERFACE_VERSION "0.1"
#define ZWO_AM5_SUPPORTED_ACTIONS "action1,action2"
#define ZWO_AM5_DEVICE_TYPE partTypeMount

// Setter command :
#define ZWO_AM5_CMD_SET_DATE_TIME_MDY ":SC%02d/%02d/%02d#"
#define ZWO_AM5_CMD_SET_DATE_TIME_HMS ":SL%02d:%02d:%02d#"
#define ZWO_AM5_CMD_SET_TIME_ZONE ":SG%+03d#"
#define ZWO_AM5_CMD_SET_LATITUDE ":St%+03d*%02d#"
#define ZWO_AM5_CMD_SET_LONGITUDE ":Sg%03d*%02d#"
#define ZWO_AM5_CMD_SET_MERIDIAN_SETTING ":STa%d#"
#define ZWO_AM5_CMD_SET_SLEW_RA ":Sr%02d:%02d:%02.0f#"
#define ZWO_AM5_CMD_SET_SLEW_DEC ":Sd%+03d*%02d:%02.0f#"
#define ZWO_AM5_CMD_SET_GOTO ":MS#"
#define ZWO_AM5_CMD_SET_SYNC_RA ":Sr%02d:%02d:%02.0f#"
#define ZWO_AM5_CMD_SET_SYNC_DEC ":Sd%+03d*%02d:%02.0f#"
#define ZWO_AM5_CMD_SET_SYNC ":CM#"
#define ZWO_AM5_CMD_SET_GUIDERATE ":Rg%.1f#"
#define ZWO_AM5_CMD_SET_TRACKING_ON ":Te#"
#define ZWO_AM5_CMD_SET_TRACKING_OFF ":Td#"
#define ZWO_AM5_CMD_SET_TRACKING_SIDEREAL ":TQ#"
#define ZWO_AM5_CMD_SET_TRACKING_LUNAR ":TL#"
#define ZWO_AM5_CMD_SET_TRACKING_SOLAR ":TS#"
#define ZWO_AM5_CMD_SET_SLEW_RATE ":R%01d#" // 0 - 9
#define ZWO_AM5_CMD_SET_MOVING_STOP_NORTH ":Qn#"
#define ZWO_AM5_CMD_SET_MOVING_STOP_SOUTH ":Qs#"
#define ZWO_AM5_CMD_SET_MOVING_START_NORTH ":Mn#"
#define ZWO_AM5_CMD_SET_MOVING_START_SOUTH ":Ms#"
#define ZWO_AM5_CMD_SET_MOVING_STOP_WEST ":Qw#"
#define ZWO_AM5_CMD_SET_MOVING_STOP_EAST ":Qe#"
#define ZWO_AM5_CMD_SET_MOVING_START_WEST ":Mw#"
#define ZWO_AM5_CMD_SET_MOVING_START_EAST ":Me#"
#define ZWO_AM5_CMD_SET_HOME_POSITION ":hC#"
#define ZWO_AM5_CMD_SET_GOTO_PARK_POSITION ":hP#"
#define ZWO_AM5_CMD_SET_STOP_ALL ":Q#"
#define ZWO_AM5_CMD_SET_CLEAR_ALIGNMENT ":NSC#"
#define ZWO_AM5_CMD_SET_GUIDE_PULSE_NORTH ":Mgn%04d#"
#define ZWO_AM5_CMD_SET_GUIDE_PULSE_SOUTH ":Mgs%04d#"
#define ZWO_AM5_CMD_SET_GUIDE_PULSE_WEST ":Mgw%04d#"
#define ZWO_AM5_CMD_SET_GUIDE_PULSE_EAST ":Mge%04d#"
#define ZWO_AM5_CMD_SET_BUZZER_VOLUME ":SBu%01d#" // 0 - 2



// Getter command :
#define ZWO_AM5_CMD_GET_DATE_TIME_MDY ":GC#"
#define ZWO_AM5_CMD_GET_DATE_TIME_HMS  ":GL#"
#define ZWO_AM5_CMD_GET_TIME_ZONE ":GG#"
#define ZWO_AM5_CMD_GET_SIDERAL_TIME ":GS#"
#define ZWO_AM5_CMD_GET_LATITUDE ":Gt#"
#define ZWO_AM5_CMD_GET_LONGITUDE ":Gg#"
#define ZWO_AM5_CMD_GET_MERIDIAN_SETTINGS ":GTa#"
#define ZWO_AM5_CMD_GET_COORDINATES_RA ":GR#"
#define ZWO_AM5_CMD_GET_COORDINATES_DEC ":GD#"
#define ZWO_AM5_CMD_GET_SLEW_RATE ":Ggr#"
#define ZWO_AM5_CMD_GET_TRACKING_STATUS ":GAT#"
#define ZWO_AM5_CMD_GET_MOUNT_MODEL ":GVP#"
#define ZWO_AM5_CMD_GET_STATUS_MODE ":GU#"
#define ZWO_AM5_CMD_GET_VERSION ":GV#"
#define ZWO_AM5_CMD_GET_CARDINAL_DIRECTION ":Gm#"
#define ZWO_AM5_CMD_GET_BUZZER_VOLUME ":GBu#"


// Utility command :
void asi_init_AM5(char *device);
void asi_deinit_AM5();
void asi_send_command_AM5(char *cmd);
telescope_response_t asi_receive_command_AM5();
telescope_response_t asi_send_receive_command_AM5(char *cmd);


// GETTER COMMANDS
telescope_response_t asi_get_version();
telescope_response_t asi_get_mount_model();
telescope_response_t asi_get_utc(time_t *secs, int *utc_offset);
telescope_response_t asi_get_sidereal_time(double *sideral_time);
telescope_response_t asi_get_site(double *latitude, double *longitude);
telescope_response_t asi_get_meridian_settings(bool *flip_enabled, bool *track_passed, int *track_passed_limit);
telescope_response_t asi_get_coordinates(double *ra, double *dec);
telescope_response_t asi_get_guide_rate(double *ra, double *dec);
telescope_response_t asi_get_tracking_status(bool *tracking);

// SETTER COMMANDS
telescope_response_t asi_set_utc_date(time_t *secs, int utc_offset);
telescope_response_t asi_set_site(double latitude, double longitude);
telescope_response_t asi_set_meridian_action(bool flip, bool track);
telescope_response_t asi_set_meridian_limit(int16_t limit);
telescope_response_t asi_slew(double ra, double dec);
telescope_response_t asi_sync(double ra, double dec);
telescope_response_t asi_set_guide_rate(double ra, double dec);
telescope_response_t asi_set_tracking(bool tracking);
telescope_response_t asi_set_tracking_rate(track_mode_t mode);
telescope_response_t asi_set_slew_rate(uint32_t rate);
telescope_response_t asi_motion_dec(direction_t dir);
telescope_response_t asi_motion_ra(direction_t dir);
telescope_response_t asi_home();
telescope_response_t asi_park();
telescope_response_t asi_stop();
telescope_response_t asi_clear_alignment_data();
telescope_response_t asi_guide_north(uint32_t duration);
telescope_response_t asi_guide_south(uint32_t duration);
telescope_response_t asi_guide_east(uint32_t duration);
telescope_response_t asi_guide_west(uint32_t duration);
telescope_response_t asi_set_buzzer_volume(uint32_t volume);
#endif

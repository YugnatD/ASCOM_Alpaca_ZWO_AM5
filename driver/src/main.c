#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "alpaca.h"
#include "AM5.h"
#include "telescope.h"
#include "alpacaDiscovery.h"

// get the zwo am5 alpaca config
extern alpacaConfig_t zwo_am5_alpaca_config;

int main(int argc, char *argv[])
{
  double ra, dec;
  time_t secs;
  int utcOffset;
  telescope_response_t response;
  strcpy(zwo_am5_alpaca_config.serialBus, "/dev/ttyACM0");
  pthread_t threadDiscovery;
  pthread_t threadAscom;
  alpaca_discovery(ASCOM_DISCOVERY_DEFAULT_PORT, ASCOM_DEFAULT_PORT, &threadDiscovery);
  initAlpacaSocket(ASCOM_DEFAULT_PORT, threadAscom);
  initAlpacaDriver(&zwo_am5_alpaca_config);

  // set actual utc date, seems to work
  // secs = time(NULL);
  // utcOffset = 2;
  // response = asi_set_utc_date(&secs, utcOffset);
  // if (response.success) {
  //   printf("Set UTC date: OK\r\n");
  // } else {
  //   printf("Set UTC date: %s, error = %d\n", response.additionalInfo, response.errorNumber);
  // }
  // response = asi_get_utc(&secs, &utcOffset);
  // printf("UTC: %s\n", ctime(&secs));

  // set coordinates NOK
  // ra = 0.0;
  // dec = 0.0;
  // response = asi_set_site(0.0, 0.0); // 
  // if (response.success) {
  //   printf("Set site: OK\r\n");
  // } else {
  //   printf("Set site: %s, error = %d\n", response.additionalInfo, response.errorNumber); // 0, error = 0
  // }
  // response = asi_get_site(&ra, &dec);
  // printf("Site: %f, %f\n", ra, dec);
  // response = asi_slew(0.0, 0.0);
  // printf("Slew: %d, error = %d\n", response.success, response.errorNumber); // error = 7

  // response = asi_set_tracking(1);
  // printf("Set tracking: %d, error = %d\n", response.success, response.errorNumber);

  removeAlpacaDriver(&zwo_am5_alpaca_config);
  pthread_join(threadDiscovery, NULL);
}

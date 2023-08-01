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
  strcpy(zwo_am5_alpaca_config.serialBus, "/dev/ttyUSB0");
  pthread_t threadDiscovery;
  pthread_t threadAscom;
  alpaca_discovery(ASCOM_DISCOVERY_DEFAULT_PORT, ASCOM_DEFAULT_PORT, &threadDiscovery);
  initAlpacaSocket(ASCOM_DEFAULT_PORT, threadAscom);
  initAlpacaDriver(&zwo_am5_alpaca_config);
  removeAlpacaDriver(&zwo_am5_alpaca_config);
  pthread_join(threadDiscovery, NULL);
}

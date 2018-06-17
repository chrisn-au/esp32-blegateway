#ifndef CN_TEST
#define CN_TEST

#include "ESPHelper32.h"
#include "Timer.h"
#include <ArduinoJson.h>

class CNHelper : public ESPHelper32 , public Timer
{
     public:
      CNHelper(netInfo *startNet);
//     CNHelper();
      bool begin(MQTT_CALLBACK_SIGNATURE);
      void status();
      void loop();
//     int loop();
};



#endif

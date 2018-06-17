#include "CNTest.h"

CNHelper::CNHelper(netInfo *startNet): ESPHelper32(startNet), Timer(){

}

bool CNHelper::begin(MQTT_CALLBACK_SIGNATURE){

  ESPHelper32::OTA_enable();
  ESPHelper32::addSubscription("/test");
  ESPHelper32::setMQTTCallback(callback);

  return(ESPHelper32::begin());
}

void CNHelper::loop(){
  ESPHelper32::loop();
  Timer::update();
}

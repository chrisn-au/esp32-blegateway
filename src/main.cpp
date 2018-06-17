#include "CNTest.h"
#include "CN-USER-CONFIG.H"
CNHelper myESP(&homeNet);
#include <string>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 300*6; //In seconds
int restart = 60 * 60 * 6;

std::string baseMqtt = "BLE/";

static void scanCompleteCB(BLEScanResults scanResults) {

}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      StaticJsonBuffer<1000> jsonBuffer;
      char buffer[1000];
      char *pHex;
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      if (advertisedDevice.haveManufacturerData()){
           pHex = BLEUtils::buildHexData(nullptr, (uint8_t*)advertisedDevice.getManufacturerData().data(), advertisedDevice.getManufacturerData().length());
           Serial.printf("Manufacturer Data: %s \n", pHex);
      }
      JsonObject& root = jsonBuffer.createObject();
      root["address"] = advertisedDevice.getAddress().toString().c_str();
      root["rssi"] = advertisedDevice.getRSSI();
      root["manufacturerdata"] = pHex;
      root.printTo(buffer, sizeof(buffer));
      std::string topic;
      std::string address;
      address = advertisedDevice.getAddress().toString();
      topic  = baseMqtt + "/" + address;
      myESP.publish(topic.c_str(),buffer);
      free(pHex);

    }
};

BLEScanResults bleScanner(){
  BLEScanResults foundDevices;
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->stop();
//  pBLEScan->setInterval(1000);
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(),true);
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  foundDevices = pBLEScan->start(scanTime, scanCompleteCB);
  return (foundDevices);
}

std::string getMacAddress(){
  uint8_t baseMac[6];
  WiFi.macAddress(baseMac);
	char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	return std::string(baseMacChr);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {

}

void restartESP(){
   ESP.restart();
}
void publish(){
  // publish health message
  StaticJsonBuffer<256> jsonBuffer;
  char buffer[256];

  JsonObject& root = jsonBuffer.createObject();
  root["status"] = "ok";
  root["RSSI"] = WiFi.RSSI();
  root.printTo(Serial);
  root.printTo(buffer, sizeof(buffer));
  myESP.publish(baseMqtt.c_str(),buffer);
  // restart the scan
  bleScanner();
}

void setup(){

  	Serial.begin(115200);	//start the serial line
  	delay(500);
  	Serial.println("Starting Up, Please Wait...");
    myESP.begin(mqttCallback);
    delay(500);
    // setup MQTT  base topic
    baseMqtt = baseMqtt + getMacAddress();

    //Setup repeating BLE scans and reboot every hour to workaround the grave of unkown problem
    myESP.every(scanTime*1000, publish);
    myESP.after(restart*1000, restartESP);
    bleScanner();
  	Serial.println("Initialization Finished.");
}

void loop(){
  myESP.loop();
}

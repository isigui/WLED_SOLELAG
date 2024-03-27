#include "Audioreactive_Presence.h"
#include "logger.h"

TemperatureManager::TemperatureManager(int dataPin, Logger& logger) : 
dataPin(dataPin), 
_logger(logger),
initialized(false),
startTime(millis()),
delayTime(2000),
oneWire(dataPin),
sensors(&oneWire)
{
    //pinMode(dataPin, INPUT_PULLUP);
    // Serial.println("initializing temperature sensor");
    // dht_sensor.begin();

}
void TemperatureManager::init(){
    Serial.println("initializing temperature sensor");
    //dht_sensor.begin();
    sensors.begin();
}
void TemperatureManager::update(){
  if(initialized == false)
  {
    initialized = millis()-startTime>delayTime;
    if(initialized == false)
    {
    Serial.println("sensor not initiliazed yet");
    }
    return;
  }
  Serial.println("reading temperature");
  sensors.requestTemperatures();
  data.temperature = sensors.getTempCByIndex(0);
  //float h = dht_sensor.readHumidity(true);
  // if (dht_sensor.measure(&data.temperature, &data.humidity)){
  //data.temperature = t;
  //data.humidity = h;
  Serial.println("celsius: " + String(data.temperature));
  // }
  // else
  // {
  //   Serial.println("unable to read dht_sensor");
  // }
}


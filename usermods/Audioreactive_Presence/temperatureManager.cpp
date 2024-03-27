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
}
void TemperatureManager::init(){
    Serial.println("initializing temperature sensor");
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
  sensors.requestTemperatures();
  data.temperature = sensors.getTempCByIndex(0);

}


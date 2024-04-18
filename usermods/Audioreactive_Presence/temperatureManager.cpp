#include "Audioreactive_Presence.h"
#include "logger.h"

TemperatureManager::TemperatureManager(int dataPin, Logger& logger) : 
dataPin(dataPin), 
_logger(logger),
initialized(false),
startTime(millis()),
delayTime(2000),
lastReadTime(0),
updateInterval(20000),
oneWire(dataPin),
sensors(&oneWire)
{
}
void TemperatureManager::init(){
  //_taskCoreId = taskCoreId;
    //Serial.println("initializing temperature sensor on core: " +String(taskCoreId));
    if(initialized == false)
      return;
    sensors.begin();
}
bool TemperatureManager::update(){
  if(initialized == false)
  {
    initialized = millis()-startTime>delayTime;
    if(initialized == false)
    {
    Serial.println("sensor not initiliazed yet");
    }
    return false;
  }
  //Serial.println("reading temperature sensor on core : "+String(_taskCoreId));
  if(millis()-lastReadTime>updateInterval)
  {
    Serial.println("temperaturemanager: read");
    sensors.requestTemperatures();
    data.temperature = sensors.getTempCByIndex(0);
    lastReadTime = millis();
    return true;

  }
  
  return false;
}


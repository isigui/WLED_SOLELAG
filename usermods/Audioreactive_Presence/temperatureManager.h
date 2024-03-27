#ifndef TEMPERATUREMANAGER_H
#define TEMPERATUREMANAGER_H

#include <Arduino.h>
#include "logger.h"
#include "OneWire.h"
#include "DallasTemperature.h"

struct TemperatureData {
    float temperature;
    TemperatureData() : temperature(0) {}
};

class TemperatureManager {
public:
    TemperatureManager(int dataPin, Logger& logger);
    void init();
    void update();
    TemperatureData data;

private:
    OneWire oneWire;
    DallasTemperature sensors;
    Logger& _logger;
    int dataPin;
    unsigned long delayTime;
    unsigned long startTime;
    bool initialized;
};

#endif // TEMPERATUREMANAGER_H

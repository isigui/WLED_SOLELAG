#include "logger.h"
#include <Arduino.h>

Logger::Logger(bool isEnable) : isDebug(isEnable){}


void Logger::Log(const char *message) {
    if(isDebug){
        Serial.println(message);
    }
}


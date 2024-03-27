#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include "logger.h"

class ButtonManager {
public:
    ButtonManager(int scrollPin, int validationPin, Logger& logger);
    bool IsActive();
    bool hasScroll();
    bool hasValidate();
    bool buttonPressed();
    bool hasNotConfirmed();
    void resetActionTimer();

private:
    Logger& logger;
    int scrollPin;
    int validationPin;
    unsigned long delayTime;
    unsigned long confirmActionDelayTime;
    unsigned long currentTime;
    unsigned long confirmTimer;
};

#endif // BUTTONMANAGER_H

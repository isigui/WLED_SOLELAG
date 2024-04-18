#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <Arduino.h>
#include "logger.h"
#include "DFRobotDFPlayerMini.h"

class SoundManager {
public:
    SoundManager(Logger& logger);
    void init();
    void test();
    void play(int index);
    void printDetail(uint8_t type, int value);
    long timer;

private:
    Logger& _logger;
    DFRobotDFPlayerMini player;
    bool initialized;
};

#endif // SOUNDMANAGER_H

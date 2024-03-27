#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H
#define I2C_SDA 9
#define I2C_SCL 7

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "logger.h"
#include "optionMenu.h"
#include "menuManager.h"

class LcdDisplay {
public:
    LcdDisplay(ulong offDelayTime, MenuManager& menuManager, Logger& logger);
    void init();
    void print(const char* text,int col, int row);
    void centerPrint(const char* text, int row);
    void handleBacklightOff();
    void Display(OptionMenu* menu);
    bool isBacklightOff();
    void ShowActiveOption();
    void backlightOn();
    void clear();
    bool autoBackLightOff = true;

private:
    Logger& _logger;
    MenuManager& menuManager;
    LiquidCrystal_I2C lcd;
    bool lcd_on;
    unsigned long currentTime;
    unsigned long backlightOffDelayTime; // delay before backlight off in ms
    void setCursor(int col, int row);
    void backlightOff();
    
    
};

#endif // LCDDISPLAY_H

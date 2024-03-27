#include "lcdDisplay.h"
#include "logger.h"
#include "optionMenu.h"
#include "menuManager.h"

LcdDisplay::LcdDisplay(ulong backlightOff, MenuManager &menuManager, Logger &logger) : lcd(0x27, 16, 2),
                                                                                       _logger(logger),
                                                                                       backlightOffDelayTime(backlightOff),
                                                                                       menuManager(menuManager)
{
    // Constructor
    currentTime = 0;
    lcd_on = true;
}

void LcdDisplay::init()
{
    lcd.init(I2C_SDA, I2C_SCL);
    
    backlightOn();
}

void LcdDisplay::backlightOn()
{
    lcd_on = true;
    lcd.backlight();
}

void LcdDisplay::backlightOff()
{
    lcd_on = false;
    lcd.noBacklight();
}

void LcdDisplay::clear()
{
    lcd.clear();
}

void LcdDisplay::print(const char *text, int col, int row)
{
    lcd.setCursor(col, row);
    lcd.print(text);
    backlightOn();
}
void LcdDisplay::centerPrint(const char* text, int row){
    size_t textLength = strlen(text);
    size_t coloffset = (16 - textLength) / 2;
    
    print(text, coloffset, row);
}

void LcdDisplay::setCursor(int col, int row)
{
    lcd.setCursor(col, row);
}
void LcdDisplay::Display(OptionMenu *menuOption)
{
    lcd.clear();
    if (menuOption != nullptr)
    {
        print(menuOption->texte, 0, 0);
        if (menuOption->selected)
        {
            print("Active", 5, 1);
        }
        currentTime = millis();
    }
    else
    {
        print("........", 4, 0);
    }
}
bool LcdDisplay::isBacklightOff()
{
    return !lcd_on;
}
void LcdDisplay::handleBacklightOff()
{
    if (lcd_on == true && autoBackLightOff == true)
    {
        if (millis() - currentTime > backlightOffDelayTime)
        {
            ShowActiveOption();
            _logger.Log("turning off lcd");
            backlightOff();
        }
    }
    else
    {
        currentTime = millis();
    }
}
void LcdDisplay::ShowActiveOption()
{
    OptionMenu *activeOption = menuManager.getSelectedOption();
    if (activeOption != nullptr)
    {
        _logger.Log(("active option is : " + String(activeOption->texte)).c_str());
    }
    Display(activeOption);
}

#include "MenuManager.h"
#include <Arduino.h>
#include "logger.h"

MenuManager::MenuManager(OptionMenu options[], int optionsize, Logger &logger)
    : _logger(logger), optionsMenu(options), selectedOption(0), currentOption(-1), nombreOptions(optionsize)
{
    // Constructeur
    // nombreOptions = sizeof(optionsMenu) / sizeof(optionsMenu[0]);
}

void MenuManager::validateSelection()
{
    // Code pour valider la sélection et exécuter l'action associée
    // Utilisez la classe LcdDisplay ou autre méthode que vous préférez
    if (currentOption != -1)
    {
        //_logger.Log(("nombreOptions " + String(nombreOptions)).c_str());
        //_logger.Log(("selection option " + String(currentOption)).c_str());
        for (int i = 0; i < nombreOptions; i++)
        {
            optionsMenu[i].selected = false;
        }
        optionsMenu[currentOption].selected = true;
    }
}
void MenuManager::gotoNextOption()
{
    _logger.Log(("gotoNextOption: nb options " + String(nombreOptions)).c_str());
    int opt = (currentOption + 1) % nombreOptions;
    setCurrentOption(opt);
}

OptionMenu *MenuManager::gotoOption(String modeCode){
    int optionRequested = -1;
    for (int i = 0; i < nombreOptions; i++)
    {
        if(modeCode.equals(optionsMenu[i].modeCode))
        {
            optionRequested = i;
        }
    }
    if(optionRequested>-1){
        setCurrentOption(optionRequested);
        validateSelection();
        return &optionsMenu[optionRequested]; 
    }
    else
    {
        _logger.Log(("no option matching code "+ modeCode).c_str());
        return nullptr;
    }
}
void MenuManager::setCurrentOption(int optionIndex)
{
    _logger.Log(("gotoNextOption: currentoption: " + String(optionIndex)).c_str());
    currentOption = optionIndex;
}

OptionMenu *MenuManager::getCurrentOption()
{
    if (currentOption > -1)
        return &optionsMenu[currentOption];
    return nullptr;
}
OptionMenu *MenuManager::getSelectedOption()
{
    int activeOptionIndex = -1;
    for (int i = 0; i < nombreOptions; i++)
    {
        if (optionsMenu[i].selected == true)
        {
            activeOptionIndex = i;
        }
    }
    if (activeOptionIndex > -1)
        return &optionsMenu[activeOptionIndex];
    else
    {
        Serial.println("no active option");
        return nullptr;
    }
}

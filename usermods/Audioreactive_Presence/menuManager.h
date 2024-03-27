#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "OptionMenu.h"
#include "logger.h"

class MenuManager {
public:
    MenuManager(OptionMenu options[], int optionsize, Logger& logger);
    void validateSelection();
    void setCurrentOption(int optionIndex);
    OptionMenu* getCurrentOption();
    OptionMenu* getSelectedOption();
    OptionMenu* gotoOption(String modeCode);
    void gotoNextOption();

private:
    Logger& _logger;
    OptionMenu* optionsMenu;
    int nombreOptions;
    int selectedOption;
    int currentOption;
};

#endif // MENUMANAGER_H

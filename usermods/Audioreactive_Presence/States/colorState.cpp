// colorState.cpp
#include "colorState.h"
#include "../Audioreactive_Presence.h"

colorState::colorState(AudioreactivePresenceUsermod* usermod) : presenceStateBase(usermod) {}

void colorState::enterState() {
    usermodPtr->_logger.Log("enter color state");
    usermodPtr->_lcdDisplay.print(usermodPtr->activeMenu->texte,0,0);
    strip.setBrightness(128);

    // strip.trigger();  // force strip refresh
    // stateChanged = true;  // inform external devices/UI of change
    
    //strip.show();
}

void colorState::exitState() {
    
    if(usermodPtr != nullptr)
        usermodPtr->_logger.Log("enter color state");
    else
    {
        Serial.print("usermodPtr is null");
    }
}

void colorState::update() {
    // Logique de mise à jour pour l'état Night

}

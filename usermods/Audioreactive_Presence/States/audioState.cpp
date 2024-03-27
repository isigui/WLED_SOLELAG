// audioState.cpp
#include "audioState.h"
#include "../Audioreactive_Presence.h"

audioState::audioState(AudioreactivePresenceUsermod* usermod) : presenceStateBase(usermod) {}
void audioState::enterState() {
    usermodPtr->_logger.Log("enter audio state");
    usermodPtr->_lcdDisplay.print(usermodPtr->activeMenu->texte,0,0);
    applyPreset(3,CALL_MODE_DIRECT_CHANGE);
    
    // Logique d'entrée pour l'état detection
}

void audioState::exitState() {
    usermodPtr->_logger.Log("exit audio state");
}

void audioState::update() {
    // Logique de mise à jour pour l'état detection
}

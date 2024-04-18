// OffState.cpp
#include "offState.h"
#include "../Audioreactive_Presence.h"

offState::offState(AudioreactivePresenceUsermod* usermod): presenceStateBase(usermod){}

void offState::enterState() {
    usermodPtr->_logger.Log("enter off state");
    usermodPtr->_lcdDisplay.print(usermodPtr->activeMenu->texte,0,0);





    applyPreset(1,CALL_MODE_DIRECT_CHANGE);

    // Logique d'entrée pour l'état Off
}

void offState::exitState() {
    usermodPtr->_logger.Log("exit off state");
    // Logique de sortie pour l'état Off
}

void offState::update() {
    // Logique de mise à jour pour l'état Off

}

// temperatureState.cpp
#include "temperatureState.h"
#include "../Audioreactive_Presence.h"


temperatureState::temperatureState(AudioreactivePresenceUsermod* usermod): presenceStateBase(usermod), 
update_interval(10000),
current_time(0)
{}
 
void temperatureState::enterState() {
    usermodPtr->_logger.Log("enter temp state");
    usermodPtr->_lcdDisplay.print(usermodPtr->activeMenu->texte,0,0);
    current_time = millis();
    usermodPtr->_temperatureManager.init();

}

void temperatureState::exitState() {
    usermodPtr->_logger.Log("exit temp state");
    // Logique de sortie pour l'état timer
}

void temperatureState::update() {
    if(millis()-current_time<update_interval)
        return;
    current_time = millis();
    TemperatureData data = usermodPtr->readDhtSensor();
    usermodPtr->_lcdDisplay.clear();
    usermodPtr->_lcdDisplay.centerPrint((String(data.temperature) + " degre").c_str(),0);
    // Logique de mise à jour pour l'état timerState
}
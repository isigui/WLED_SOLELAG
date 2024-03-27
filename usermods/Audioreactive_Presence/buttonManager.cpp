#include "buttonManager.h"
#include "logger.h"
bool canValidate = false;
ButtonManager::ButtonManager(int scrollPin, int validationPin, Logger& logger) : 
scrollPin(scrollPin), 
validationPin(validationPin), 
logger(logger), 
delayTime(150), 
confirmActionDelayTime(5000) {
    currentTime = 0;
    confirmTimer = 0;
    pinMode(scrollPin, INPUT_PULLUP);
    pinMode(validationPin, INPUT_PULLUP);
}
bool ButtonManager::IsActive(){
    int timer = millis() - currentTime;
    if(timer> delayTime)
    {
        resetActionTimer();
        return true;
    }
    return false;
}
bool ButtonManager::buttonPressed(){
    return digitalRead(scrollPin) == LOW || digitalRead(validationPin) == LOW;
}
bool ButtonManager::hasScroll() {
    int pinVal = digitalRead(scrollPin);
    //logger.Log(("scroll val: " + String(pinVal)).c_str());
    if (pinVal == LOW) {
        logger.Log(("scroll val: " + String(pinVal)).c_str());
        resetActionTimer();
        confirmTimer = millis();
        canValidate = true;
        return true;
        // Ajoutez votre code de gestion du scroll ici
    }
    return false;
}

bool ButtonManager::hasValidate() {
    int pinVal = digitalRead(validationPin);
    //logger.Log(("confirm val: " + String(pinVal)).c_str());
    if (pinVal == LOW && canValidate) {
        logger.Log(("confirm val: " + String(pinVal)).c_str());
        canValidate = false;
        resetActionTimer();
        confirmTimer = 0;
        return true;
        // Ajoutez votre code de gestion de la sélection ici
    }
    return false;
}

bool ButtonManager::hasNotConfirmed() {
    if (millis() - confirmTimer > confirmActionDelayTime && confirmTimer>0) {
        confirmTimer = 0;
        canValidate = false;
        logger.Log("check confirm");
        // Ajoutez votre code de gestion de l'absence de confirmation ici
        return true;
    }
    return false;
}

void ButtonManager::resetActionTimer() {
    currentTime = millis();
}

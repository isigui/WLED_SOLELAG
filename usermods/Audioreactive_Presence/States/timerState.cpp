// OffState.cpp
#include "timerState.h"
#include "../Audioreactive_Presence.h"
#include "fx.h"


int lastTime;
uint16_t mode_chronometre(void)
{
  uint32_t tempsEcoule = strip.now; // Temps écoulé en millisecondes
  Serial.println("minuteur: " + String(SEGLEN));
  if (SEGMENT.speed == 0)
    SEGMENT.speed = 60;
  if (tempsEcoule < SEGMENT.speed * 1000)
  {
    //  Calculer la position de la prochaine LED dans la partie non éclairée du bandeau
    float period = ((SEGMENT.speed*1000.0) / (SEGLEN * 1.0));
    uint8_t nombreLEDAllumees = floor(tempsEcoule / period);
    int reste = tempsEcoule - (nombreLEDAllumees*period);
    int secondEcoulee = floor(tempsEcoule / 1000)*1000;
    

    uint8_t nombreLEDEteintes = SEGLEN - nombreLEDAllumees;
    // calcul de la position de la led suivante en partant de la fin du bandeau en fonction du reste (qui va de 0 à 1000ms donc)
    int positionLEDSuivante  = (SEGLEN - floor(((reste/period)*nombreLEDEteintes)));
    //Serial.println("minuteur: " + String(SEGMENT.speed)+" current: "+ String(secondEcoulee) + " last: "+ String(lastTime) +" Allumee " + String(nombreLEDAllumees) + " Eteintes: " + String(nombreLEDEteintes) + " strip.now:  " + String(tempsEcoule) + "partie entiere: " + String(secondEcoulee) + "  reste: " + String(reste) + "percent: " + reste / 1000.0 + " position led: " + String(positionLEDSuivante));
    if(secondEcoulee - lastTime != 0)
    {
      lastTime = secondEcoulee;
      timerState::triggerTimerEvent();
    }
    // Allumer les LED en rouge
    for (int i = 0; i < SEGLEN; i++)
    {
      if (i <= nombreLEDAllumees)
        SEGMENT.setPixelColor(i, 255, 0, 0);
      else
      {
        if (i == positionLEDSuivante)
        {
          SEGMENT.setPixelColor(i, 0, 0, 255);
        }
        else
        {
          SEGMENT.setPixelColor(i, 0, 0, 0);
        }
      }
    }
  }
  else
  {
    lastTime = SEGMENT.speed * 1000;
    timerState::triggerTimerEvent();
  }

  return FRAMETIME;
}
static const char _data_FX_MODE_CHRONOMETRE[] PROGMEM = "chrono@!;!,!;!;01";

timerState::timerState(AudioreactivePresenceUsermod* usermod): presenceStateBase(usermod){}
 
void timerState::enterState() {
    usermodPtr->_logger.Log("enter timer state");
    lastTime = 0;
    usermodPtr->_lcdDisplay.autoBackLightOff = false;
    usermodPtr->_lcdDisplay.print(usermodPtr->activeMenu->texte,0,0);
    timerState::subscribeToTimerEvent([this]() {
        // Réagissez à l'événement ici
        // Vous pouvez appeler des méthodes spécifiques de timerState ou effectuer d'autres actions nécessaires
        this->onTimer();
    });
    strip.addEffect(FX_MODE_CHRONOMETRE, &mode_chronometre, _data_FX_MODE_CHRONOMETRE);
    strip.getMainSegment().speed = usermodPtr->MinuteurDurationInSeconds;
    strip.getMainSegment().on = true;
    strip.setBrightness(255);
    resetTimebase();
    strip.getMainSegment().setMode(FX_MODE_CHRONOMETRE);
}

void timerState::exitState() {
    usermodPtr->_lcdDisplay.autoBackLightOff = true;
    usermodPtr->_logger.Log("exit timer state");
}

void timerState::update() {}

std::function<void()> timerState::timerEvent; 
void timerState::triggerTimerEvent()
{
    // Déclenchez l'événement en appelant la fonction de rappel
    if (timerEvent) {
        timerEvent();
    }
    else
    {
      Serial.println("no subscription to timer");
    }
}

void timerState::subscribeToTimerEvent(std::function<void()> callback)
{
    // Abonnez-vous à l'événement en fournissant une fonction de rappel
    timerEvent = callback;
}

void timerState::onTimer(){
    //int elapsed = strip.now/1000;
    String elapsedStr = String(lastTime);
    int remaining = strip.getMainSegment().speed*1000-lastTime;
    String remainingStr = String(remaining/1000);
    usermodPtr->_logger.Log(elapsedStr.c_str());
    usermodPtr->_lcdDisplay.clear();
    usermodPtr->_lcdDisplay.centerPrint("chrono",0);
    usermodPtr->_lcdDisplay.centerPrint(remainingStr.c_str(),1);
    if(remaining <=1000)
    {
      Serial.println("Done !");
         usermodPtr->setMode("OFF");   
    }
}

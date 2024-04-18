// OffState.cpp
#include "timerState.h"
#include "../Audioreactive_Presence.h"
#include "fx.h"

uint32_t chronoTimeInMs;
uint32_t chronoMaxTimeInSecond;
uint32_t getLedColorByLot(int i, int lotSize)
{
  int lotIndex = i / lotSize;
  uint32_t ledColor;
  switch (lotIndex)
  {
  case 0: // Lot 1 en rouge
    ledColor = RGBW32(255, 0, 0, 0);
    break;
  case 1: // Lot 2 en vert
    ledColor = RGBW32(0, 255, 0, 0);
    break;
  case 2: // Lot 3 en bleu
    ledColor = RGBW32(0, 0, 255, 0);
    break;
  case 3: // Lot 4 en jaune
    ledColor = RGBW32(255, 255, 0, 0);
    break;
  default:
    ledColor = RGBW32(138, 43, 226, 255);
    break;
  }
  return ledColor;
}
uint16_t mode_chronometre(void)
{
  uint32_t tempsEcoule = strip.now; // Temps écoulé en millisecondes
  // Serial.println("minuteur: " + String(SEGLEN));
  int lotSize = SEGLEN / 4;
  if (chronoMaxTimeInSecond == 0)
    chronoMaxTimeInSecond = 60;

  if (tempsEcoule < 10000)
  { // countdown before chrono starts
    int countdown_reste = tempsEcoule - (floor(tempsEcoule / 1000.0) * 1000);
    uint32_t color = countdown_reste < 500 ? RGBW32(255, 0, 0, 255) : RGBW32(0, 0, 0, 0);
    for (int i = 0; i < SEGLEN; i++)
    {
      SEGMENT.setPixelColor(i, color);
    }
  }
  else
  {
    int countertime = tempsEcoule - 10000;
    if (countertime < chronoMaxTimeInSecond * 1000)
    {
      //  Calculer la position de la prochaine LED dans la partie non éclairée du bandeau
      float period = ((chronoMaxTimeInSecond * 1000.0) / (SEGLEN * 1.0));
      uint8_t nombreLEDAllumees = floor(countertime / period);
      uint32_t reste = countertime - (nombreLEDAllumees * period);
      uint32_t milliSecondEcoulee = floor(countertime / 1000) * 1000;

      uint8_t nombreLEDEteintes = SEGLEN - nombreLEDAllumees;
      // calcul de la position de la led suivante en partant de la fin du bandeau en fonction du reste (qui va de 0 à 1000ms donc)
      int positionLEDSuivante = (SEGLEN - floor(((reste / period) * nombreLEDEteintes)));
      // Serial.println("minuteur: " + String(SEGMENT.speed)+" current: "+ String(secondEcoulee) + " last: "+ String(chronoTime) +" Allumee " + String(nombreLEDAllumees) + " Eteintes: " + String(nombreLEDEteintes) + " strip.now:  " + String(tempsEcoule) + "partie entiere: " + String(secondEcoulee) + "  reste: " + String(reste) + "percent: " + reste / 1000.0 + " position led: " + String(positionLEDSuivante));
      if (milliSecondEcoulee - chronoTimeInMs != 0)
      {
        chronoTimeInMs = milliSecondEcoulee;
        Serial.println("trigger from chrono en cours");
        timerState::triggerTimerEvent();
      }
      // Allumer les LED en rouge
      for (int i = 0; i < SEGLEN; i++)
      {
        if (i <= nombreLEDAllumees)
        {
          SEGMENT.setPixelColor(i, getLedColorByLot(i, lotSize));
        }
        else
        {
          if (i == positionLEDSuivante)
          {
            SEGMENT.setPixelColor(i, RGBW32(138, 43, 226, 255));
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
      chronoTimeInMs = chronoMaxTimeInSecond * 1000;
      Serial.println("trigger from chrono ended");
      timerState::triggerTimerEvent();
    }
  }
  return FRAMETIME;
}
static const char _data_FX_MODE_CHRONOMETRE[] PROGMEM = "chrono@!;!,!;!;01";

timerState::timerState(AudioreactivePresenceUsermod *usermod) : presenceStateBase(usermod) {}

void timerState::enterState()
{
  usermodPtr->_logger.Log("enter timer state");
  chronoTimeInMs = 0;
  chronoMaxTimeInSecond = usermodPtr->MinuteurDurationInSeconds;
  usermodPtr->_logger.Log(("minuteur de : " + String(chronoMaxTimeInSecond)).c_str());
  usermodPtr->_lcdDisplay.autoBackLightOff = false;
  usermodPtr->_lcdDisplay.print(usermodPtr->activeMenu->texte, 0, 0);
  // usermodPtr->_soundManager.init();
  // usermodPtr->_soundManager.play(1);
  timerState::subscribeToTimerEvent([this]()
                                    {
        // Réagissez à l'événement ici
        // Vous pouvez appeler des méthodes spécifiques de timerState ou effectuer d'autres actions nécessaires
        this->onTimer(); });


  strip.addEffect(FX_MODE_CHRONOMETRE, &mode_chronometre, _data_FX_MODE_CHRONOMETRE);
  strip.getMainSegment().speed = chronoMaxTimeInSecond;
  strip.getMainSegment().on = true;
  strip.setBrightness(255);
  resetTimebase();
  strip.getMainSegment().setMode(FX_MODE_CHRONOMETRE);
}

void timerState::exitState()
{
  chronoTimeInMs = 0;
  usermodPtr->publishHAMinuteur(0,chronoMaxTimeInSecond);
  usermodPtr->_lcdDisplay.autoBackLightOff = true;
  usermodPtr->_logger.Log("exit timer state");
}

void timerState::update()
{

  
}

std::function<void()> timerState::timerEvent;
void timerState::triggerTimerEvent()
{
  // Déclenchez l'événement en appelant la fonction de rappel
  if (timerEvent)
  {
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

void timerState::onTimer()
{
  // int elapsed = strip.now/1000;
  String elapsedStr = String(chronoTimeInMs);
  Serial.println(elapsedStr +" / " + String(chronoMaxTimeInSecond*1000));
  uint32_t remaining = chronoMaxTimeInSecond * 1000 - chronoTimeInMs;
  String remainingStr = String(remaining / 1000);
Serial.println(("remaining: " +String(remaining)).c_str());
  usermodPtr->_logger.Log(String(chronoTimeInMs).c_str());
  usermodPtr->_lcdDisplay.clear();
  usermodPtr->_lcdDisplay.centerPrint("chrono", 0);
  usermodPtr->_lcdDisplay.centerPrint(remainingStr.c_str(), 1);
  usermodPtr->publishHAMinuteur(chronoTimeInMs/1000,chronoMaxTimeInSecond);
  // switch(chronoTimeInMs){
  //     // case 1000: usermodPtr->_soundManager.play(2) ; break;
  //     // case 15000: usermodPtr->_soundManager.play(3) ; break;
  //     // case 30000: usermodPtr->_soundManager.play(4) ; break;
  //     // case 45000: usermodPtr->_soundManager.play(5) ; break;
  //     // case 59000: usermodPtr->_soundManager.play(6) ; break;
  // }

  if (remaining <= 1000)
  {
    Serial.println("Done !");
    usermodPtr->setMode("OFF");
  }
}

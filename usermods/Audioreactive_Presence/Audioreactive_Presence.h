#pragma once

#include "wled.h"
#include <Arduino.h>
#include "lcdDisplay.h"
#include "buttonManager.h"
#include "logger.h"
#include "optionMenu.h"
#include "menuManager.h"
#include "temperatureManager.h"
#include "States/offState.h"
#include "States/detectionState.h"
#include "States/colorState.h"
#include "States/audioState.h"
#include "States/timerState.h"
#include "States/temperatureState.h"
//#include <functional>

class AudioreactivePresenceUsermod : public Usermod
{
private:
  bool initDone = false;
  bool enabled = false;
  bool mqttConnected = false;
  static const char _name[];
  static const char _enabled[];
  char errorMessage[100] = "";
  unsigned long _lastMqttReconnectTime;
  uint16_t MinuteurInSeconds;
  ButtonManager _buttonManager;
  MenuManager _menuManager;

  OptionMenu optionsMenu[6];
  presenceStateBase* currentState;
  //static std::function<void()> timerEvent;
public:
  LcdDisplay _lcdDisplay;
  Logger _logger;
  OptionMenu *activeMenu;
  TemperatureManager _temperatureManager;
  // constructor
  AudioreactivePresenceUsermod();
  // destructor
  ~AudioreactivePresenceUsermod() {}
void setup();
void loop();
void onMqttConnect(bool sessionPresent);
bool onMqttMessage(char* topic, char* payload);
void transitionToState(presenceStateBase* newState);
void setMode(const String& modeName);
TemperatureData readDhtSensor();


//static void triggerTimerEvent();
//static void subscribeToTimerEvent(std::function<void()> callback); // Méthode pour s'abonner à l'événement
uint16_t getId()
{
  return USERMOD_ID_SOLEGAG;
}
    void appendConfigData()
    {
      Serial.println("Enter user mode settings");
      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":MinuteurInSeconds")); oappend(SET_F("',1,'<i>s &#x23F1</i>');"));
      //oappend(SET_F("addInfo('AudioreactivePresenceUsermod:MinuteurInSeconds',30,'s <i> &#x23F1; </i>');"));
      
    }
void addToConfig(JsonObject& root)
    {
      // Serial.println("add to config");
      // JsonObject top = root.createNestedObject(FPSTR(_name));
      // //save these vars persistently whenever settings are saved
      // top["minuteur"] = userVar0;
    }
bool readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)
      Serial.println("read from config");
      JsonObject top = root[FPSTR(_name)];

      bool configComplete = !top.isNull();


      return configComplete;
    }

 };

// add more strings here to reduce flash memory usage
const char AudioreactivePresenceUsermod::_name[] PROGMEM = "SOLEGAG";
//const char AudioreactivePresenceUsermod::_enabled[] PROGMEM = "enabled";

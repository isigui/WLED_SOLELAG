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

  ButtonManager _buttonManager;
  MenuManager _menuManager;

  OptionMenu optionsMenu[6];
  presenceStateBase* currentState;
  
  static const char _defaultMode[];
  static const char _minuteurDurationInSeconds[];
  static const char _minuteurEnableSound[];
  static const char _temperatureMin[];
  static const char _temperatureMax[];
  static const char _temperatureEffectBrightness[];
      
  
  //static std::function<void()> timerEvent;
public:

  uint8_t MinuteurDurationInSeconds = 25;
  bool MinuteurEnableSound = true;
  uint8_t DefaultMode = 0;
  float TemperatureMin = 17.0;
  float TemperatureMax = 30.0;
  uint16_t TemperatureEffectBrightness = 255;

  LcdDisplay _lcdDisplay;
  Logger _logger;
  OptionMenu *activeMenu;
  TemperatureManager _temperatureManager;
  AudioreactivePresenceUsermod();
  ~AudioreactivePresenceUsermod() {}
  void setup();
  void loop();
  void onMqttConnect(bool sessionPresent);
  bool onMqttMessage(char* topic, char* payload);
  void transitionToState(presenceStateBase* newState);
  void setMode(const String& modeName);
  TemperatureData readDhtSensor();
  void appendConfigData();
  void addToConfig(JsonObject& root);
  bool readFromConfig(JsonObject& root);
 

//static void triggerTimerEvent();
//static void subscribeToTimerEvent(std::function<void()> callback); // Méthode pour s'abonner à l'événement
uint16_t getId()
{
  return USERMOD_ID_SOLEGAG;
}


 };

// add more strings here to reduce flash memory usage
const char AudioreactivePresenceUsermod::_name[] PROGMEM = "SOLEGAG";

//const char AudioreactivePresenceUsermod::_defaultMode[] PROGMEM = "mode";

const char AudioreactivePresenceUsermod::_minuteurDurationInSeconds[] PROGMEM = "duration";
const char AudioreactivePresenceUsermod::_minuteurEnableSound[] PROGMEM = "enable";

const char AudioreactivePresenceUsermod::_temperatureMin[] PROGMEM = "min";
const char AudioreactivePresenceUsermod::_temperatureMax[] PROGMEM = "max";

const char AudioreactivePresenceUsermod::_temperatureEffectBrightness[] PROGMEM = "luminosity";

//const char AudioreactivePresenceUsermod::_enabled[] PROGMEM = "enabled";

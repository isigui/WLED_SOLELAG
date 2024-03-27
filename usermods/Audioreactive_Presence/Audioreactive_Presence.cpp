#include "Audioreactive_Presence.h"
#include "fcn_declare.h"

AudioreactivePresenceUsermod::AudioreactivePresenceUsermod() : _logger(true),
                                                               _buttonManager(37, 35, _logger),
                                                               _menuManager(optionsMenu, 6, _logger),
                                                               _lcdDisplay(10000, _menuManager, _logger),
                                                               _temperatureManager(33,_logger),
                                                               _lastMqttReconnectTime(0),
                                                               optionsMenu{{"Off", "OFF", new offState(this), false},
                                                                           {"On", "COLOR", new colorState(this), false},
                                                                           {"Mode audio", "AUDIO", new audioState(this), false},
                                                                           {"Mode detection", "DETECT", new detectionState(this), false},
                                                                           {"Timer", "TIMER", new timerState(this), false},
                                                                           {"Mode temperature", "TEMPERATURE", new temperatureState(this), false}
                                                                           }
{
}

void AudioreactivePresenceUsermod::setup()
{
  _lcdDisplay.init();
  //_temperatureManager.init();
  // currentState = nullptr;
  DEBUG_PRINTLN("default mode is "+String(DefaultMode));
  currentState = optionsMenu[DefaultMode].state;
  //currentState->enterState();
  //transitionToState(currentState);
}

void AudioreactivePresenceUsermod::loop()
{
  if (currentState != nullptr)
  {
    currentState->update();
  }

  else
  {
    _logger.Log("current state is null");
  }

  if (_buttonManager.IsActive())
  {
    if (_lcdDisplay.isBacklightOff())
    {
      if (_buttonManager.buttonPressed())
      {
        _lcdDisplay.ShowActiveOption();
        return;
      }
    }
    if (_buttonManager.hasScroll())
    {
      _logger.Log("scroll");
      _menuManager.gotoNextOption();
      _lcdDisplay.Display(_menuManager.getCurrentOption());
    }
    if (_buttonManager.hasValidate())
    {
      _logger.Log("validate");
      _menuManager.validateSelection();
      activeMenu = _menuManager.getSelectedOption();
      //_lcdDisplay.Display(activeMenu);
      if (activeMenu != nullptr)
      {
        transitionToState(activeMenu->state);
      }
    }
  }
  if (_buttonManager.hasNotConfirmed())
  {
    _lcdDisplay.print("Confirmer ?", 2, 1);
  }
  _lcdDisplay.handleBacklightOff();
  // if (!mqtt->connected()) {
  unsigned long currentTime = millis();

  // Attendre au moins 5 secondes entre les tentatives de reconnexion
  if (currentTime - _lastMqttReconnectTime > 5000)
  {
    // mqtt->connect();
    if (mqtt != nullptr)
    {
      if (mqtt->connected())
      {
        //_logger.Log("MQTT connected");
      }
      else
      {
        _logger.Log("MQTT disconnected");
        mqtt->connect();
      }
    }
    _lastMqttReconnectTime = currentTime; // Mettez à jour le temps de la dernière tentative de reconnexion
  }
  //}
}

void onAlexaMessage(EspalexaDevice *dev)
{
  Serial.println("receiving some alexa messages !!");
  AudioreactivePresenceUsermod *mod = (AudioreactivePresenceUsermod *)usermods.lookup(USERMOD_ID_SOLEGAG);
  if (mod != nullptr)
  {
    Serial.println("name: " + dev->getName());
    Serial.println("Hue: " + String(dev->getHue()));
    Serial.println("Sat: " + String(dev->getSat()));
    uint16_t hue = dev->getHue();
    uint16_t sat = dev->getSat();

    if (hue == 46421 && sat == 127) // LAVANDE
    {
      Serial.println("alexa ask audio mode");
      mod->setMode("AUDIO");//
      return;
    }
    if (hue == 63351 && sat == 64) // ROSE
    {
      Serial.println("alexa ask detection mode");
      mod->setMode("DETECT");
      return;
    }
    if (hue == 54613 && sat == 254) // MAGENTA
    {
      Serial.println("alexa ask color mode");
      mod->setMode("COLOR");
      return;
    }
    if (dev->getState() == false)
    {
      Serial.println("alexa ask to turn off");
      mod->setMode("OFF");
      return;
    }
    if (dev->getState() == true)
    {
      Serial.println("alexa ask to turn on");
       mod->setMode("COLOR");
       return;
    }
  }
  else
  {
    Serial.println("unable to find usermod SOLEGAG");
  }
}

TemperatureData AudioreactivePresenceUsermod::readDhtSensor(){
  _temperatureManager.update();
  return _temperatureManager.data;
}


void AudioreactivePresenceUsermod::setMode(const String& modeName){
   
  OptionMenu* requestOption = _menuManager.gotoOption(modeName);
  if(requestOption != nullptr)
  {
    activeMenu = requestOption;
    transitionToState(activeMenu->state);
  }
  else
  {
    Serial.println("impossible de trouver le mode à activer");
  }
  
}
void AudioreactivePresenceUsermod::transitionToState(presenceStateBase *newState)
{
  _logger.Log("transition");
  if (currentState != nullptr)
  {
    _logger.Log("current state is not null");

    currentState->exitState(); // Appel à exitState sur l'état actuel avant de passer au nouvel état
  }
  else
  {
    _logger.Log("current state is null");
  }

  currentState = newState;

  if (currentState != nullptr)
  {
    _lcdDisplay.clear();
    currentState->enterState(); // Appel à enterState sur le nouvel état
  }
}

void AudioreactivePresenceUsermod::onMqttConnect(bool sessionPresent)
{
  _logger.Log("mqtt connected from audioreactive presence");
  if (currentState != nullptr)
  {
    currentState->onMqttConnect(sessionPresent);
  }
}
bool AudioreactivePresenceUsermod::onMqttMessage(char *topic, char *payload)
{
  _logger.Log("message received");
  if (currentState != nullptr)
  {
    currentState->onMqttMessage(topic, payload);
    _logger.Log("message traité");
  }
  return true;
}


    void AudioreactivePresenceUsermod::appendConfigData()
    {
      Serial.println("Enter user mode settings");
      
      
      oappend(SET_F("dd=addDropdown('SOLEGAG','config:defaultmode');"));
      oappend(SET_F("addOption(dd,'Off',0);"));
      oappend(SET_F("addOption(dd,'Color',1);"));
      oappend(SET_F("addOption(dd,'Audio',2);"));
      oappend(SET_F("addOption(dd,'Detection',3);"));
      oappend(SET_F("addOption(dd,'Minuteur',4);"));
      oappend(SET_F("addOption(dd,'Temperature',5);"));

      oappend(SET_F("addInfo('SOLEGAG:minuteur:duration',1,'<i>s &#x23F1;</i>');"));     // 0 is field type, 1 is actual field
      oappend(SET_F("addInfo('SOLEGAG:minuteur:enable',1,'activer le son <i>&#x266A;</i>');"));     // 0 is field type, 1 is actual field

      oappend(SET_F("addInfo('SOLEGAG:temperature:min',1,'<i> &#127777;</i>');"));     // 0 is field type, 1 is actual field
      oappend(SET_F("addInfo('SOLEGAG:temperature:max',1,'<i> &#127777;</i>');"));     // 0 is field type, 1 is actual field
      oappend(SET_F("addInfo('SOLEGAG:temperature:brightness',1,'luminosité');"));     // 0 is field type, 1 is actual field

    }
void AudioreactivePresenceUsermod::addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));

      JsonObject configNode = top.createNestedObject("config");
      configNode["defaultmode"] = DefaultMode;

      JsonObject minuteurNode = top.createNestedObject("minuteur");
      minuteurNode[FPSTR(_minuteurDurationInSeconds)] = MinuteurDurationInSeconds;
      minuteurNode[FPSTR(_minuteurEnableSound)] = MinuteurEnableSound;
      
      JsonObject temperatureNode = top.createNestedObject("temperature");
      temperatureNode[FPSTR(_temperatureMin)] = TemperatureMin;
      temperatureNode[FPSTR(_temperatureMax)] = TemperatureMax;
      temperatureNode[FPSTR(_temperatureEffectBrightness)] = TemperatureEffectBrightness;

       DEBUG_PRINTLN(F("Solegag config saved."));
    }
bool AudioreactivePresenceUsermod::readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)
JsonObject top = root[FPSTR(_name)];
  if (top.isNull()) {
    DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
    return false;
  }
      Serial.println("read from config");


      MinuteurDurationInSeconds = top["minuteur"][FPSTR(_minuteurDurationInSeconds)] | MinuteurDurationInSeconds;
      MinuteurEnableSound = top["minuteur"][FPSTR(_minuteurEnableSound)] | MinuteurEnableSound;
      TemperatureMin = top["temperature"][FPSTR(_temperatureMin)] | TemperatureMin;
      TemperatureMax = top["temperature"][FPSTR(_temperatureMax)] | TemperatureMax;
      TemperatureEffectBrightness = top["temperature"][FPSTR(_temperatureEffectBrightness)] | TemperatureEffectBrightness;
      DefaultMode = top["config"]["defaultmode"] | DefaultMode;
      return true;
    }


//const char AudioreactivePresenceUsermod::_data_FX_MODE_CHRONOMETRE[] PROGMEM = "chrono@!;!,!;!;01";
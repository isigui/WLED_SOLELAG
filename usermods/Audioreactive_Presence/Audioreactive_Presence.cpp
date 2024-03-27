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
  currentState = optionsMenu[4].state;
  //currentState->enterState();
  //transitionToState(optionsMenu[1].state);
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
TemperatureData AudioreactivePresenceUsermod::readDhtSensor(){
  _temperatureManager.update();
  return _temperatureManager.data;
}



//const char AudioreactivePresenceUsermod::_data_FX_MODE_CHRONOMETRE[] PROGMEM = "chrono@!;!,!;!;01";
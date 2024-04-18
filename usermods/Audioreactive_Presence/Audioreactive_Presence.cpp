#include "Audioreactive_Presence.h"
#include "fcn_declare.h"

AudioreactivePresenceUsermod::AudioreactivePresenceUsermod() : _logger(true),
                                                               _buttonManager(37, 35, _logger),
                                                               _menuManager(optionsMenu, 6, _logger),
                                                               _lcdDisplay(10000, _menuManager, _logger),
                                                               _temperatureManager(33, _logger),
                                                               _soundManager(_logger),
                                                               _lastMqttReconnectTime(0),
                                                               optionsMenu{{"Off", "OFF", new offState(this), false},
                                                                           {"On", "COLOR", new colorState(this), false},
                                                                           {"Mode audio", "AUDIO", new audioState(this), false},
                                                                           {"Mode detection", "DETECT", new detectionState(this), false},
                                                                           {"Timer", "TIMER", new timerState(this), false},
                                                                           {"Mode temperature", "TEMPERATURE", new temperatureState(this), false}}
{
}

void AudioreactivePresenceUsermod::setup()
{
  Serial.println("setting up AudioreactivePresenceUsermod");

  _lcdDisplay.init();

  // currentState = nullptr;
  DEBUG_PRINTLN("default mode is " + String(DefaultMode));
  currentState = optionsMenu[DefaultMode].state;
  // currentState->enterState();
  // transitionToState(currentState);
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
  if (currentTime - _lastMqttReconnectTime > 1000)
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

  if (_temperatureManager.update())
  {
    publishHATemperature();
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
      mod->setMode("AUDIO"); //
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

TemperatureData AudioreactivePresenceUsermod::readDhtSensor()
{
  return _temperatureManager.data;
}

void AudioreactivePresenceUsermod::setMode(const String &modeName)
{

  OptionMenu *requestOption = _menuManager.gotoOption(modeName);
  if (requestOption != nullptr)
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
  publishHACurrentMode();
}

void AudioreactivePresenceUsermod::onMqttConnect(bool sessionPresent)
{
  _logger.Log("mqtt connected from audioreactive presence");
  initializeHAMqtt();
  if (currentState != nullptr)
  {
    currentState->onMqttConnect(sessionPresent);
  }
  _temperatureManager.init();
}
bool AudioreactivePresenceUsermod::onMqttMessage(char *topic, char *payload)
{
  _logger.Log(("message received on topic: "+String(topic)).c_str());
  if (strcmp(topic, String("/currentmode/set").c_str()) == 0)
  {
    _logger.Log("yesssss this is the input select topic !");
    _logger.Log(("payload is: "+String(payload)).c_str());
    setMode(String(payload));
  }
  else
  {
    if (currentState != nullptr)
    {
      currentState->onMqttMessage(topic, payload);
      _logger.Log("message traité");
    }
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

  oappend(SET_F("addInfo('SOLEGAG:minuteur:duration',1,'<i>s &#x23F1;</i>');"));            // 0 is field type, 1 is actual field
  oappend(SET_F("addInfo('SOLEGAG:minuteur:enable',1,'activer le son <i>&#x266A;</i>');")); // 0 is field type, 1 is actual field

  oappend(SET_F("addInfo('SOLEGAG:temperature:min',1,'<i> &#127777;</i>');")); // 0 is field type, 1 is actual field
  oappend(SET_F("addInfo('SOLEGAG:temperature:max',1,'<i> &#127777;</i>');")); // 0 is field type, 1 is actual field
  oappend(SET_F("addInfo('SOLEGAG:temperature:brightness',1,'luminosité');")); // 0 is field type, 1 is actual field
}
void AudioreactivePresenceUsermod::addToConfig(JsonObject &root)
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
bool AudioreactivePresenceUsermod::readFromConfig(JsonObject &root)
{
  // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
  // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)
  JsonObject top = root[FPSTR(_name)];
  if (top.isNull())
  {
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

void AudioreactivePresenceUsermod::initializeHAMqtt()
{

  // mqttCurrentModeTopic = String(mqttDeviceTopic) + "/current_mode";
  mqttTemperatureTopic = String(mqttDeviceTopic) + "/temperature";
  mqttMinuteurTopic = String(mqttDeviceTopic) + "/minuteur";
  mqttModeSelectTopic = String(mqttDeviceTopic) + "/currentmode"; // Sujet pour l'input_select
  // Serial.println("mqttCurrentModeTopic: " + mqttCurrentModeTopic);
  Serial.println("mqttTemperatureTopic: " + mqttTemperatureTopic);
  Serial.println("mqttMinuteurTopic: " + mqttMinuteurTopic);
  Serial.println("mqttSelectModeTopic: " + mqttModeSelectTopic);
  // publishHAMqttSensor("mode", "Mode", mqttCurrentModeTopic, "","");
  publishHAMqttSensor("temperature", "Temperature", mqttTemperatureTopic, "temperature", "°C");
  publishHAMqttSensor("chrono", "Chronomètre", mqttMinuteurTopic, "", "");
  publishHAInputSelect("currentmode", "CurrentMode", mqttModeSelectTopic);
  mqttInitialized = true;
}

void AudioreactivePresenceUsermod::publishHACurrentMode()
{
  if (!WLED_MQTT_CONNECTED)
    return;

  // Construire l'objet JSON pour le mode courant
  //StaticJsonDocument<100> doc;
  Serial.print("publish state to Home Assistant: ");

  mqtt->publish(mqttModeSelectTopic.c_str(), 0, false, activeMenu->modeCode);
}
void AudioreactivePresenceUsermod::publishHATemperature()
{
  if (!WLED_MQTT_CONNECTED)
    return;
  Serial.println("publish temperature to Home Assistant");
  mqtt->publish(mqttTemperatureTopic.c_str(), 0, false, String(_temperatureManager.data.temperature).c_str());
}
void AudioreactivePresenceUsermod::publishHAMinuteur(uint32_t currentTimer, uint32_t maxTimer)
{
  if (!WLED_MQTT_CONNECTED)
    return;
  // Construire l'objet JSON pour le minuteur
  StaticJsonDocument<200> doc;
  doc["current"] = currentTimer;
  doc["target"] = maxTimer;
  char buffer[200];
  serializeJson(doc, buffer);
  Serial.print("publish chrono to Home Assistant: ");
  Serial.println(buffer);
  mqtt->publish(mqttMinuteurTopic.c_str(), 0, false, buffer);
}
void AudioreactivePresenceUsermod::publishHAMqttSensor(
    const String &name,
    const String &friendly_name,
    const String &state_topic,
    const String &deviceClass,
    const String &unitOfMeasurement)
{
  if (WLED_MQTT_CONNECTED)
  {
    String discoverytopic = String("homeassistant/sensor/") + mqttClientID + "/" + name + "/config";

    StaticJsonDocument<300> doc;

    doc["name"] = String(serverDescription) + " " + friendly_name;
    doc["state_topic"] = state_topic;
    doc["unique_id"] = String(mqttClientID) + name;
    doc["expire_after"] = 1800;

    JsonObject device = doc.createNestedObject("device"); // attach the sensor to the same device
    device["name"] = String(serverDescription);
    device["model"] = "WLED";
    device["identifiers"] = String("wled-") + String(serverDescription);
    device["sw_version"] = VERSION;

    if (unitOfMeasurement != "")
    {
      doc["unit_of_measurement"] = unitOfMeasurement;
    }

    if (deviceClass != "")
    {
      doc["device_class"] = deviceClass;
    }

    // Add value_template logic
    if (name == "mode")
    {
      // Value template logic for mode
      doc["value_template"] = "{{ value_json.mode }}";
    }
    if (name == "chrono")
    {
      doc["value_template"] = "{{ value_json.current }}";
    }

    String payload;
    serializeJson(doc, payload);
    Serial.println(payload);
    mqtt->publish(discoverytopic.c_str(), 0, true, payload.c_str());
  }
}

void AudioreactivePresenceUsermod::publishHAInputSelect(
    const String &name,
    const String &friendly_name,
    const String &state_topic)
{
  if (!WLED_MQTT_CONNECTED)
    return;
  String discoverytopic = String("homeassistant/select/") + mqttClientID + "/" + name + "/config";
  // Construire l'objet JSON pour l'input_select
  StaticJsonDocument<1024> doc;
  doc["name"] = String(serverDescription) + " " + friendly_name;
  doc["unique_id"] = String(mqttClientID) + "_" + name + "_select";
  // Créer un tableau JSON pour stocker les options de mode
  JsonArray options = doc.createNestedArray("options");
  options.add("OFF");
  options.add("COLOR");
  options.add("DETECT");
  options.add("AUDIO");
  options.add("TIMER");
  options.add("TEMPERATURE");

  doc["device_class"] = "select";
  // Ajouter le sujet MQTT où envoyer les commandes pour changer l'option sélectionnée
  doc["state_topic"] = mqttModeSelectTopic;
  doc["command_topic"] = mqttModeSelectTopic+"/set";
  // Ajouter des informations sur le device ESP32
  JsonObject device = doc.createNestedObject("device");
  device["identifiers"] = String("wled-") + String(serverDescription); // Identifiant unique de votre ESP32
  device["name"] = String(serverDescription);                          // Nom de l'esp32
  device["model"] = "WLED";                                            // Modele de l'esp32
  // Vous pouvez ajouter d'autres informations comme le fabricant, la version du firmware, etc.

  String payload;
  serializeJson(doc, payload);
  Serial.print("publish input_select to Home Assistant: ");
  Serial.println(payload);

  mqtt->publish(discoverytopic.c_str(), 0, true, payload.c_str());
  mqtt->subscribe((mqttModeSelectTopic+"/set").c_str(), 0);
}

// const char AudioreactivePresenceUsermod::_data_FX_MODE_CHRONOMETRE[] PROGMEM = "chrono@!;!,!;!;01";
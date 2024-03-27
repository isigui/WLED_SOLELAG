// detectionState.cpp
#include "detectionState.h"
#include "../Audioreactive_Presence.h"
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include "HTTPClient.h"

detectionState::detectionState(AudioreactivePresenceUsermod *usermod) : presenceStateBase(usermod) {}
void detectionState::enterState()
{
  usermodPtr->_logger.Log("enter detection state");
  usermodPtr->_lcdDisplay.print(usermodPtr->activeMenu->texte, 0, 0);
  lastPreset = 4;
  applyPreset(4, CALL_MODE_DIRECT_CHANGE);
  lastActionTime = 0;
  presetDuration = 10000;
  // mqtt->connect();
  if (mqtt != nullptr)
  {
    String topic = String(mqttDeviceTopic) + "/detection";
    mqtt->subscribe(topic.c_str(), 0);
    usermodPtr->_logger.Log(("subscribe to mqtt topic" + topic).c_str());
  }

  // Logique d'entrée pour l'état detection
}

void detectionState::exitState()
{
  usermodPtr->_logger.Log("exit detection state");
  String topic = String(mqttDeviceTopic) + "/detection";
  mqtt->unsubscribe(topic.c_str());
}

void detectionState::onMqttConnect(bool sessionPresent)
{
  usermodPtr->_logger.Log("detectionState mqtt connected");
  String topic = String(mqttDeviceTopic) + "/detection";
  mqtt->subscribe(topic.c_str(), 0);
  usermodPtr->_logger.Log(("subscribe to mqtt topic" + topic).c_str());
}
bool detectionState::onMqttMessage(char *topic, char *payload)
{
  unsigned long currentTime = millis();
  if (currentTime - lastActionTime < presetDuration)
  {
    Serial.println("current time: " + String(currentTime) + " lastActionTime: "+ String(lastActionTime) + " presetDuration: "+ String(presetDuration)); 
    usermodPtr->_logger.Log("Attente de 10 secondes avant la prochaine action.");
    return false; // Ne traitez pas le message MQTT ou ne déclenchez pas le preset pendant la période d'attente.
  }
  // Gérez le message MQTT reçu ici
  char errorMessage[100] = "";
  Serial.println("Message reçu : " + String(payload));
  Serial.println("Message MQTT reçu sur le topic : " + String(topic));
  PSRAMDynamicJsonDocument payloadJson(4096); // in practice about 2673
  DeserializationError error = deserializeJson(payloadJson, payload);
  if (error)
  {
    strcat(errorMessage, PSTR("deserializeJson() failed: "));
    strcat(errorMessage, error.c_str());
    usermodPtr->_logger.Log(errorMessage);
  }
  else
  {

    const char *clientName = payloadJson["client_name"];
    const int preset = payloadJson["preset"];
    presetDuration = payloadJson["preset_duration"].as<unsigned int>() * 1000;
    //  Affichez le clientName
    Serial.println("Client Name: " + String(clientName) + " declenche preset "+ String(preset) + "pendant "+String(presetDuration) + "ms");
    usermodPtr->_lcdDisplay.clear();
    usermodPtr->_lcdDisplay.centerPrint(clientName, 0);
    lastPreset = 0;
    lastActionTime = currentTime;
    applyPreset(preset,CALL_MODE_DIRECT_CHANGE);
  }
  // Affichez le message reçu

  return true;
}

void detectionState::update()
{
  // usermodPtr->_logger.Log("update detection state");
  unsigned long currentTime = millis();
  if (lastPreset != 4 && (currentTime - lastActionTime > presetDuration))
  {
    lastPreset = 4;
    applyPreset(4, CALL_MODE_DIRECT_CHANGE);
    usermodPtr->_logger.Log("détection en cours");
    presetDuration = 10000;
  }
}

  // int effect = jsonBody["seg"][0]["fx"].as<int>();
  // Serial.println("set effect: "+String(effect));
  // strip.setMode(0,effect);
  // strip.trigger();


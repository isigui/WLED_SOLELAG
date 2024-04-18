// detectonState.h
#ifndef DETECTIONSTATE_H
#define DETECTIONSTATE_H

#include "presenceStateBase.h"

class detectionState : public presenceStateBase {
public:
    detectionState(AudioreactivePresenceUsermod* usermod);  // Ajoute le constructeur
    void enterState() override;
    void exitState() override;
    void update() override;
    void onMqttConnect(bool sessionPresent) override;
    bool onMqttMessage(char* topic, char* payload) override;
    void sendApiRequest(const char *path, const char *method);
private:
    unsigned long lastActionTime;
    int lastPreset;
    unsigned long presetDuration;
    String _topic;
    void apiCall(DynamicJsonDocument jsonBody);
};

#endif // DETECTIONSTATE_H

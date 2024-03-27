// audioState.h
#ifndef AUDIOSTATE_H
#define AUDIOSTATE_H

#include "presenceStateBase.h"

class audioState : public presenceStateBase {
public:
    audioState(AudioreactivePresenceUsermod* usermod);  // Ajoute le constructeur
    void enterState() override;
    void exitState() override;
    void update() override;
};

#endif // AUDIOSTATE_H

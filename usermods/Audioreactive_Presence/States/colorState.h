// colorState.h
#ifndef COLORSTATE_H
#define COLORSTATE_H

#include "presenceStateBase.h"

class colorState : public presenceStateBase {
public:
    colorState(AudioreactivePresenceUsermod* usermod);  // Ajoute le constructeur
    void enterState() override;
    void exitState() override;
    void update() override;
};

#endif // COLORSTATE_H

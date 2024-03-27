// OffState.h
#ifndef OFFSTATE_H
#define OFFSTATE_H

#include "presenceStateBase.h"

class offState : public presenceStateBase {
public:
    offState(AudioreactivePresenceUsermod* usermod);  // Ajoute le constructeur
    void enterState() override;
    void exitState() override;
    void update() override;
};

#endif // OFFSTATE_H

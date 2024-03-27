// temperatureState.h
#ifndef TEMPERATURESTATE_H
#define TEMPERATURESTATE_H
#include "presenceStateBase.h"

class temperatureState : public presenceStateBase
{
public:
    temperatureState(AudioreactivePresenceUsermod *usermod);
    void enterState() override;
    void exitState() override;
    void update() override;
private:
    unsigned current_time;
    unsigned update_interval=10000;//10 secondes
};
#endif //TEMPERATURESTATE_H
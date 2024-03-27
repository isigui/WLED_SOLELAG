// timerState.h
#ifndef TIMERSTATE_H
#define TIMERSTATE_H
#include "presenceStateBase.h"
#include "functional"

class timerState : public presenceStateBase
{
public:
    timerState(AudioreactivePresenceUsermod *usermod);
    ulong timer_ms;
    void enterState() override;
    void exitState() override;
    void update() override;
    void onTimer();
    static void triggerTimerEvent();
    static void subscribeToTimerEvent(std::function<void()> callback); 
    static std::function<void()> timerEvent; 
private:
    //int striplength;
    //void updateLED();
};
#endif
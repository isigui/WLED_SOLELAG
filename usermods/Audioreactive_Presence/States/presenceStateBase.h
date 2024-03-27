// PresenceStateBase.h
#ifndef PRESENCESTATEBASE_H
#define PRESENCESTATEBASE_H

class AudioreactivePresenceUsermod;  // Déclaration anticipée de la classe principale

class presenceStateBase {
public:
    presenceStateBase(AudioreactivePresenceUsermod* usermod);  // Ajoute le constructeur
    virtual void enterState() = 0;
    virtual void exitState() = 0;
    virtual void update() = 0;
    virtual void onMqttConnect(bool sessionPresent){
       
    }
    virtual bool onMqttMessage(char* topic, char* payload) {
        return true;
    }

    protected:
    AudioreactivePresenceUsermod* usermodPtr;  // Membre pour stocker le pointeur vers l'objet parent
};

#endif // PRESENCESTATEBASE_H

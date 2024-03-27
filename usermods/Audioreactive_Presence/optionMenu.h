#ifndef OPTIONMENU_H
#define OPTIONMENU_H

#include "./States/presenceStateBase.h"

struct OptionMenu {
    const char *texte;
    const char *modeCode;
    presenceStateBase* state;
    bool selected;

    // Ajoute un constructeur pour initialiser la référence state
    OptionMenu(const char* text, const char* code, presenceStateBase* stateRef, bool isSelected)
        : texte(text), modeCode(code), state(stateRef), selected(isSelected) {}
};

#endif // OPTIONMENU_H

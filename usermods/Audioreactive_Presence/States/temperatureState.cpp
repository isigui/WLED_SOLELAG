// temperatureState.cpp
#include "temperatureState.h"
#include "../Audioreactive_Presence.h"

//static float debug_temp;
static CRGBPalette16 palette;
static CRGB color;
static double maxEffectBrightness;
CRGB temperatureToColor(float temperature)
{
    // Normaliser la température entre 0 et 1
    if (temperature < 17.0)
        temperature = 17.0;
    if (temperature > 30.0)
        temperature = 30.0;
    float normalizedTemperature = (temperature - 17.0) / (30.0 - 17.0);
    Serial.println("temperature "+ String(temperature) + " normalizedTemp " + String(normalizedTemperature) + " (R: " + String(color.r) + " G: " + String(color.g) + " B: " + String(color.b) + ")");
    color = ColorFromPalette(palette, (int)(255 * normalizedTemperature));
    return color;
}

uint16_t mode_temperature(void)
{
    //     int temperature = SEGMENT.speed;
    //  CRGB color = temperatureToColor(temperature);
    double breath = abs(sin((strip.now * 0.001 * 0.5)));
    //Serial.println(time_factor);
    double seglen = SEGLEN;
    for (int i = 0; i < SEGLEN; i++)
    {
        int offset = (i + (int)(breath * SEGLEN)) % SEGLEN;
        //float wave = max(0.5, 1.0 - sin((PI/2.0)*(offset/seglen)));
        double wave = min(maxEffectBrightness, max(0.1,(breath+ sin(PI*(i/seglen)))/2.0));
        strip.setPixelColor(i, color.r*wave, color.g*wave, color.b*wave); // Définir la couleur du pixel
    }
    return FRAMETIME; // Assuming FRAMETIME is a defined constant
}
static const char _data_FX_MODE_TEMPERATURE[] PROGMEM = "temperature@!;!,!;!;01";

temperatureState::temperatureState(AudioreactivePresenceUsermod *usermod) : presenceStateBase(usermod),
                                                                            update_interval(10000),
                                                                            current_time(0)
{
}

void temperatureState::enterState()
{
    usermodPtr->_logger.Log("enter temp state");
    usermodPtr->_lcdDisplay.print(usermodPtr->activeMenu->texte, 0, 0);
    current_time = millis();
    usermodPtr->_temperatureManager.init();
    maxEffectBrightness = usermodPtr->TemperatureEffectBrightness/255.0;
    strip.addEffect(FX_MODE_TEMPERATURE, &mode_temperature, _data_FX_MODE_TEMPERATURE);
    strip.setBrightness(255);
    SEGMENT.speed = 1;
    //debug_temp = 17.0;
    //color = temperatureToColor(debug_temp);
    strip.getMainSegment().setMode(FX_MODE_TEMPERATURE);
    palette = strip.getMainSegment().loadPalette(palette, 54); //54 palette temperature
}
void temperatureState::exitState()
{
    usermodPtr->_logger.Log("exit temp state");
    // Logique de sortie pour l'état timer
}

void temperatureState::update()
{
    if (millis() - current_time < update_interval)
        return;
    current_time = millis();
    TemperatureData data = usermodPtr->readDhtSensor();
     usermodPtr->_lcdDisplay.clear();
     usermodPtr->_lcdDisplay.centerPrint((String(data.temperature) + " degre").c_str(), 0);
    color = temperatureToColor(data.temperature);
    //color = temperatureToColor(28.0);
    //Serial.println(String(SEGMENT.speed));
    
    
    // debug_temp += 0.5;
    // if (debug_temp > 30.0)
    //     debug_temp = 17.0;
    // usermodPtr->_lcdDisplay.clear();
    // usermodPtr->_lcdDisplay.centerPrint((String(debug_temp) + " degre").c_str(), 0);
    

    // Serial.println(debug_temp);
    // Serial.println("R: " +String(color.r) +" G: " + String(color.g) +" B: " + String(color.b));
    //strip.getMainSegment().speed = debug_temp;
    // Logique de mise à jour pour l'état timerState
}
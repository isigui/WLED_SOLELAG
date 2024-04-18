// temperatureState.cpp
#include "temperatureState.h"
#include "../Audioreactive_Presence.h"

// static float debug_temp;
static CRGBPalette16 palette;
static CRGB color;
static double maxEffectBrightness;
static float minTemperature;
static float maxTemperature;
//TaskHandle_t TemperatureReaderTask;
//volatile int taskCoreID = -1;
// void Task1Code(void *parameter)
// {
//     TemperatureManager *tempManager = (TemperatureManager *)parameter;
//     taskCoreID = xPortGetCoreID();
//     tempManager->init(taskCoreID);
//     for (;;)
//     {
//         tempManager->update();
//         delay(10000);
//     }
// }
CRGB temperatureToColor(float temperature)
{
    // Normaliser la température entre 0 et 1
    if (temperature < minTemperature)
        temperature = minTemperature;
    if (temperature > maxTemperature)
        temperature = maxTemperature;
    float normalizedTemperature = (temperature - minTemperature) / (30.0 - minTemperature);
    Serial.println("temperature " + String(temperature) + " normalizedTemp " + String(normalizedTemperature) + " (R: " + String(color.r) + " G: " + String(color.g) + " B: " + String(color.b) + ")");
    color = ColorFromPalette(palette, (int)(255 * normalizedTemperature));
    return color;
}

uint16_t mode_temperature(void)
{
    double breath = abs(sin((strip.now * 0.001 * 0.5)));
    double seglen = SEGLEN;
    for (int i = 0; i < SEGLEN; i++)
    {
        int offset = (i + (int)(breath * SEGLEN)) % SEGLEN;
        // float wave = max(0.5, 1.0 - sin((PI/2.0)*(offset/seglen)));
        double wave = min(maxEffectBrightness, max(0.1, (breath + sin(PI * (i / seglen))) / 2.0));
        strip.setPixelColor(i, color.r * wave, color.g * wave, color.b * wave); // Définir la couleur du pixel
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
    
    maxEffectBrightness = usermodPtr->TemperatureEffectBrightness / 255.0;
    minTemperature = usermodPtr->TemperatureMin;
    maxTemperature = usermodPtr->TemperatureMax;
    strip.addEffect(FX_MODE_TEMPERATURE, &mode_temperature, _data_FX_MODE_TEMPERATURE);
    strip.setBrightness(255);
    SEGMENT.speed = 1;
    strip.getMainSegment().setMode(FX_MODE_TEMPERATURE);
    palette = strip.getMainSegment().loadPalette(palette, 54); // 54 palette temperature
    // TemperatureManager *tempManager = usermodPtr->getTemperatureManager();
    // if (tempManager != nullptr)
    // {
    //     xTaskCreatePinnedToCore(
    //         Task1Code,
    //         "TemperatureReaderTask",
    //         10000,
    //         tempManager,
    //         0,
    //         &TemperatureReaderTask,
    //         1);
    // }
    // else{
    //     Serial.println("Warning: temperature manager is unavailable");
    // }
}
void temperatureState::exitState()
{
    //vTaskDelete(TemperatureReaderTask);
    usermodPtr->_logger.Log("exit temp state");
    // Logique de sortie pour l'état timer
}

void temperatureState::update()
{
    if (millis() - current_time < update_interval)
        return;
    current_time = millis();
    //Serial.println("Temperature state executing on core: " + String(xPortGetCoreID()));
    
    TemperatureData data = usermodPtr->readDhtSensor();
    usermodPtr->_lcdDisplay.clear();
    usermodPtr->_lcdDisplay.centerPrint((String(data.temperature) + " degre").c_str(), 0);
    color = temperatureToColor(data.temperature);
    //color = temperatureToColor(25.0);
}
#include "Audioreactive_Presence.h"
#include "logger.h"

SoundManager::SoundManager(Logger &logger) : _logger(logger), initialized(false), timer(0)
{
}
void SoundManager::init()
{
    if(initialized == true)
        return;
    timer = 0;
    Serial.println("initializing sound");
    Serial1.begin(9600);
    delay(100);
    if (!player.begin(Serial1, true, true))
    {

        Serial.println("problem initialisation sound manager");
    }
    else
    {
        initialized = true;
        Serial.println("sound initialized !");
        player.volume(20);
    }
}
void SoundManager::play(int index){
    if(initialized == false)
        return;
    Serial.println("playing "+String(index));
    player.stop();
    player.play(index);
}
void SoundManager::test()
{
    if (initialized == false)
        return;
    // if (millis() - timer > 3000)
    // {
    //     Serial.println("next sound");
    //     timer = millis();
    //     player.next(); // Play next mp3 every 3 second.
    // }

    if (player.available())
    {
        Serial.print("detail: ");
        printDetail(player.readType(), player.read()); // Print the detail message from DFPlayer to handle different errors and states.
    }
}

void SoundManager::printDetail(uint8_t type, int value)
{
    switch (type)
    {
    case TimeOut:
        Serial.println(F("Time Out!"));
        break;
    case WrongStack:
        Serial.println(F("Stack Wrong!"));
        break;
    case DFPlayerCardInserted:
        Serial.println(F("Card Inserted!"));
        break;
    case DFPlayerCardRemoved:
        Serial.println(F("Card Removed!"));
        break;
    case DFPlayerCardOnline:
        Serial.println(F("Card Online!"));
        break;
    case DFPlayerUSBInserted:
        Serial.println("USB Inserted!");
        break;
    case DFPlayerUSBRemoved:
        Serial.println("USB Removed!");
        break;
    case DFPlayerPlayFinished:
        Serial.print(F("Number:"));
        Serial.print(value);
        Serial.println(F(" Play Finished!"));
        break;
    case DFPlayerError:
        Serial.print(F("DFPlayerError:"));
        switch (value)
        {
        case Busy:
            Serial.println(F("Card not found"));
            break;
        case Sleeping:
            Serial.println(F("Sleeping"));
            break;
        case SerialWrongStack:
            Serial.println(F("Get Wrong Stack"));
            break;
        case CheckSumNotMatch:
            Serial.println(F("Check Sum Not Match"));
            break;
        case FileIndexOut:
            Serial.println(F("File Index Out of Bound"));
            break;
        case FileMismatch:
            Serial.println(F("Cannot Find File"));
            break;
        case Advertise:
            Serial.println(F("In Advertise"));
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

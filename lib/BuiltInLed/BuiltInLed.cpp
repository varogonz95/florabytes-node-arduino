#include <Arduino.h>

#include "BuiltInLed.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

void BuiltInLed::setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

void BuiltInLed::on()
{
    digitalWrite(LED_BUILTIN, HIGH);
}

void BuiltInLed::off()
{
    digitalWrite(LED_BUILTIN, LOW);
}

void BuiltInLed::toggle()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void BuiltInLed::blink(uint32_t duration, uint32_t times, uint8_t initState)
{
    digitalWrite(LED_BUILTIN, initState);
    for (size_t i = 0; i < times * 2; i++)
    {
        toggle();
        delay(duration);
    }
}
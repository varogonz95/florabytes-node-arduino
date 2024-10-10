#include <Arduino.h>

#include "BuiltInLed.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

void BuilInLed::setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

void BuilInLed::on()
{
    digitalWrite(LED_BUILTIN, HIGH);
}

void BuilInLed::off()
{
    digitalWrite(LED_BUILTIN, LOW);
}

void BuilInLed::toggle()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void BuilInLed::blink(uint32_t duration, uint32_t times)
{
    for (size_t i = 0; i < times * 2; i++)
    {
        toggle();
        delay(duration);
    }
}
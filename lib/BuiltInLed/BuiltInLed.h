#ifndef _BUILT_IN_LED_
#define _BUILT_IN_LED_

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

class BuiltInLed
{
private:
    /* data */
public:
    static void setup();
    static void on();
    static void off();
    static void toggle();
    static void blink(uint32_t duration, uint32_t times = 1, uint8_t initState = HIGH);
};

#endif

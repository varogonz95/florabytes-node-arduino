#include <stdlib.h>

class BuilInLed
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

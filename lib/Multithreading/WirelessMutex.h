#ifndef _WIRELESS_MUTEX_
#define _WIRELESS_MUTEX_

#include <Arduino.h>
static auto xWirelessMutex = xSemaphoreCreateMutex();

#endif
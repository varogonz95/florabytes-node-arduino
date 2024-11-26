#include <Arduino.h>
#include <BLEWorkflow.h>

void bleAdvertisementTask(void *params)
{
    xSemaphoreTake(xWirelessMutex, portMAX_DELAY);

    auto device_id = (String *)params;
    BLEWorkflow::startAdvertising(*device_id);
}
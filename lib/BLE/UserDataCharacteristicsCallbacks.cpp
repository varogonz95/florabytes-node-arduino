#include <Arduino.h>
#include <GSON.h>
#include <Workflow.h>

#include "UserDataCharacteristicsCallbacks.h"

UserDataCharacteristicsCallbacks::UserDataCharacteristicsCallbacks()
{
}

void UserDataCharacteristicsCallbacks::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
    try
    {
        Serial.println("Read requested.");
    }
    catch (const std::exception &e)
    {
        Serial.println(e.what());
    }
}

void UserDataCharacteristicsCallbacks::onRead(BLECharacteristic *pCharacteristic)
{
    try
    {
        Serial.println("Read (no param) requested.");
    }
    catch (const std::exception &e)
    {
        Serial.println(e.what());
    }
}

void UserDataCharacteristicsCallbacks::onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
    try
    {
        Serial.println("Write requested.");

        Workflow::Data = param->write.value;
        Workflow::DataLength = param->write.len;
        Workflow::State = WIFI_CREDENTIALS_RECEIVED;

        // Serial.printf("Workflow::Data: %s\n", Workflow::Data == nullptr? "empty" : "has data");
        // Serial.printf("Workflow::DataLength: %d\n", Workflow::DataLength);
    }
    catch (const std::exception &e)
    {
        Serial.println(e.what());
    }
}

void UserDataCharacteristicsCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    try
    {
        Serial.println("Write (no param) requested.");
    }
    catch (const std::exception &e)
    {
        Serial.println(e.what());
    }
}

void UserDataCharacteristicsCallbacks::onNotify(BLECharacteristic *pCharacteristic)
{
}

void UserDataCharacteristicsCallbacks::onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code)
{
}

UserDataCharacteristicsCallbacks::~UserDataCharacteristicsCallbacks()
{
}

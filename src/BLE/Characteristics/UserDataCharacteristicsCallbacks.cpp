#include <Arduino.h>
#include <GSON.h>

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
        // uint8_t *dataBytes = pCharacteristic->getData();
        uint8_t *dataBytes = param->write.value;
        // size_t len = sizeof(dataBytes);
        size_t len = param->write.len;
        std::string dataStr(dataBytes, dataBytes + len);

        Serial.println("Write requested:");
        Serial.printf("Data length: %d\n", len);
        Serial.println(dataStr.c_str());

        
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

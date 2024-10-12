#include <Arduino.h>
// #include <GSON.h>
#include <Workflow.h>

#include "UserDataCharacteristicsCallbacks.h"

UserDataCharacteristicsCallbacks::UserDataCharacteristicsCallbacks() {}

void UserDataCharacteristicsCallbacks::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) {}

void UserDataCharacteristicsCallbacks::onRead(BLECharacteristic *pCharacteristic) {}

void UserDataCharacteristicsCallbacks::onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
    Serial.println("Write requested.");

    auto rawData = param->write.value;
    auto len = param->write.len;
    std::string wifiJson(rawData, rawData + len);
    auto rawJson = wifiJson.c_str();
    Workflow::setData((uint8_t*)rawJson, len);
    Workflow::setState(WIFI_CREDENTIALS_RECEIVED);
}

void UserDataCharacteristicsCallbacks::onWrite(BLECharacteristic *pCharacteristic) {}

void UserDataCharacteristicsCallbacks::onNotify(BLECharacteristic *pCharacteristic) {}

void UserDataCharacteristicsCallbacks::onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code) {}

UserDataCharacteristicsCallbacks::~UserDataCharacteristicsCallbacks() {}

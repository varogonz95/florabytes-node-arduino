#include <Arduino.h>
#include <SerialLogger.h>
#include <WirelessMutex.h>
#include <Workflow.h>

#include "UserDataCharacteristicsCallbacks.h"

UserDataCharacteristicsCallbacks::UserDataCharacteristicsCallbacks() {}

void UserDataCharacteristicsCallbacks::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) {}

void UserDataCharacteristicsCallbacks::onRead(BLECharacteristic *pCharacteristic) {}

void UserDataCharacteristicsCallbacks::onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
    Logger.Info(" [BLE Charactersitic] Write requested.");
    auto rawData = param->write.value;
    auto len = param->write.len;
    Workflow::setData(rawData, len);
    xSemaphoreGive(xWirelessMutex);
    // Workflow::setState(WIFI_CREDENTIALS_RECEIVED);
}

void UserDataCharacteristicsCallbacks::onWrite(BLECharacteristic *pCharacteristic) {}

void UserDataCharacteristicsCallbacks::onNotify(BLECharacteristic *pCharacteristic) {}

void UserDataCharacteristicsCallbacks::onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code) {}

UserDataCharacteristicsCallbacks::~UserDataCharacteristicsCallbacks() {}

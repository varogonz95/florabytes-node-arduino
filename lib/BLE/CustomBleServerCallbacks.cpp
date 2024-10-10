#include <Arduino.h>
#include <Workflow.h>

#include "CustomBleServerCallbacks.h"

CustomBleServerCallbacks::CustomBleServerCallbacks()
{
}

void CustomBleServerCallbacks::onConnect(BLEServer *pServer)
{
    Workflow::State = BLE_PAIRED;
}

void CustomBleServerCallbacks::onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    onConnect(pServer);
}

void CustomBleServerCallbacks::onDisconnect(BLEServer *pServer)
{
    Serial.println("BLE: Client disconnected.");
    Workflow::State = BLE_WAITING_TO_PAIR;
}

void CustomBleServerCallbacks::onDisconnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    onDisconnect(pServer);
}

void CustomBleServerCallbacks::onMtuChanged(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    Serial.println("BLE: MTU Changed");
}

CustomBleServerCallbacks::~CustomBleServerCallbacks()
{
}

#include <Arduino.h>
#include <Workflow.h>

#include "CustomBleServerCallbacks.h"

CustomBleServerCallbacks::CustomBleServerCallbacks()
{
}

void CustomBleServerCallbacks::onConnect(BLEServer *pServer)
{
    Serial.println("BLE: Client connected.");
    Workflow::setState(BLE_PAIRED);
}

void CustomBleServerCallbacks::onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    void* peer;
    pServer->addPeerDevice(peer, true, param->connect.conn_id);
    onConnect(pServer);
}

void CustomBleServerCallbacks::onDisconnect(BLEServer *pServer)
{
    Serial.println("BLE: Client disconnected.");
    Workflow::setState(BLE_WAITING_TO_PAIR);
    
}

void CustomBleServerCallbacks::onDisconnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    pServer->removePeerDevice(param->disconnect.conn_id, true);
    pServer->startAdvertising();
    onDisconnect(pServer);
}

void CustomBleServerCallbacks::onMtuChanged(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    Serial.println("BLE: MTU Changed");
}

CustomBleServerCallbacks::~CustomBleServerCallbacks()
{
}

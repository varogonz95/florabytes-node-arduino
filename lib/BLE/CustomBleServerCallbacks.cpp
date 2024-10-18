#include <Arduino.h>
#include <BLEDevice.h>
#include <SerialLogger.h>
#include <Workflow.h>

#include "CustomBleServerCallbacks.h"

CustomBleServerCallbacks::CustomBleServerCallbacks()
{
}

void CustomBleServerCallbacks::onConnect(BLEServer *pServer)
{
    Logger.Info(" [BLE Server] Client connected.");
    Workflow::setState(BLE_PAIRED);
}

void CustomBleServerCallbacks::onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    void *peer;
    pServer->addPeerDevice(peer, true, param->connect.conn_id);
    onConnect(pServer);
}

void CustomBleServerCallbacks::onDisconnect(BLEServer *pServer)
{
    Logger.Info(" [BLE Server] Client disconnected.");
    Workflow::setState(BLE_ADVERTISING);
}

void CustomBleServerCallbacks::onDisconnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    if (Workflow::getState() != WIFI_CREDENTIALS_RECEIVED) {
        pServer->startAdvertising();
    }
    
    pServer->removePeerDevice(param->disconnect.conn_id, true);
    onDisconnect(pServer);
}

void CustomBleServerCallbacks::onMtuChanged(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    Logger.Info(" [BLE Server] MTU Changed");
}

CustomBleServerCallbacks::~CustomBleServerCallbacks()
{
}

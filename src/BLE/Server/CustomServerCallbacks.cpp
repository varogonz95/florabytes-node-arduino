#include "CustomServerCallbacks.h"

CustomServerCallbacks::CustomServerCallbacks()
{
}

void CustomServerCallbacks::onConnect(BLEServer * pServer)
{
}

void CustomServerCallbacks::onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{

}

void CustomServerCallbacks::onDisconnect(BLEServer *pServer)
{
}

void CustomServerCallbacks::onDisconnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
}

void CustomServerCallbacks::onMtuChanged(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
}

CustomServerCallbacks::~CustomServerCallbacks()
{
}

/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

using namespace std;

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#define BLE_BASE_UUID "00001101-0000-1000-8000-00805f9b34fb"

// Constants
const uint32_t ServiceUuid = 0x181C;
const uint32_t CharacteristicUuid = 0x2A9F;

// Global variables
string deviceName;

void setup()
{
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    // String macAddress = WiFi.macAddress();
    // char *c = new char[macAddress.length()];
    // macAddress.toCharArray(c, sizeof(c), 0);

    BLEUUID baseUuid = BLEUUID::fromString(BLE_BASE_UUID);

    BLEDevice::init("FloraBytes Node");
    BLEServer *bleServer = BLEDevice::createServer();
    BLEService *bleService = bleServer->createService(ServiceUuid);
    BLECharacteristic *bleCharacteristic = bleService->createCharacteristic(CharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    try
    {
        bleCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE);
        bleService->addCharacteristic(bleCharacteristic);
        bleServer->startAdvertising();

        digitalWrite(LED_BUILTIN, HIGH);
    }
    catch (const std::exception &e)
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void loop()
{
}

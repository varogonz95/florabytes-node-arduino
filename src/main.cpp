#include <string>
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

// Constants
const uint32_t ServiceUuid = 0x181C;
const uint32_t CharacteristicUuid = 0x2A9F;

void initBLE() {
    const char *macAddress = WiFi.macAddress().c_str();
    BLEDevice::init(strcat("FloraBytes-", macAddress));
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
        Serial.println(e.what());
    }
}

void setup()
{
    // enable serial port
    Serial.begin(115200);

    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    initBLE();
}

void loop()
{
}

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

// #include "CustomServerCallbacks.h"
#include <UserDataCharacteristicsCallbacks.h>

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

#define SERVICE_UUID "0000181c-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002a9f-0000-1000-8000-00805f9b34fb"

void initBLE()
{
    Serial.println("Initializing BLE service.");

    const char *macAddress = WiFi.macAddress().c_str();

    BLEDevice::init("FloraBytes");
    // BLEDevice::init(strcat("FloraBytes-", macAddress));

    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setCallbacks(new UserDataCharacteristicsCallbacks());

    // CustomServerCallbacks *serverCallbacks;
    // pServer->setCallbacks(serverCallbacks);
    pService->start();
    Serial.println("BLE service started.");

    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    // BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    // pAdvertising->addServiceUUID(SERVICE_UUID);
    // pAdvertising->setScanResponse(true);
    // pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    // pAdvertising->setMinPreferred(0x12);

    BLEDevice::startAdvertising();
    Serial.println("BLE service advertisement started.");
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
    delay(1000);
}

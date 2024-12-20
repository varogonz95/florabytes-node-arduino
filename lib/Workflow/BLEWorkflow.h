#ifndef _BLE_WORKFLOW_
#define _BLE_WORKFLOW_

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <CustomBleServerCallbacks.h>
#include <UserDataCharacteristicsCallbacks.h>

#include <WirelessMutex.h>

// BLE Advertised services defines
#define BLE_SERVICE_UUID "0000181c-0000-1000-8000-00805f9b34fb"
#define BLE_CHARACTERISTIC_UUID "00002a9f-0000-1000-8000-00805f9b34fb"

namespace BLEWorkflow
{
    static BLEServer *pServer = nullptr;
    static BLEService *pService = nullptr;

    void startAdvertising(String device_id)
    {
        String bleDeviceName = "FloraBytes (" + device_id + ")";
        BLEDevice::init(bleDeviceName.c_str());

        Serial.println("Initializing BLE service.");
        pServer = BLEDevice::createServer();
        pService = pServer->createService(BLE_SERVICE_UUID);

        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
            BLE_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
        pCharacteristic->setCallbacks(new UserDataCharacteristicsCallbacks());

        pServer->setCallbacks(new CustomBleServerCallbacks);
        pService->start();
        Serial.println("BLE service started.");

        BLEAdvertising *pAdvertising = pServer->getAdvertising(); // this still is working for backward compatibility
        // pAdvertising->setAppearance(0x0700);
        pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        // pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
        // pAdvertising->setMinPreferred(0x12);
        pAdvertising->start();
        // BLEDevice::startAdvertising();

        // Workflow::setState(BLE_ADVERTISING);
        Serial.println("BLE service advertisement started.");
    }

    void freeResources()
    {
        // Free BLE resource stack prior to
        // activating WiFi stack
        BLEDevice::deinit(true);
        free(nullptr);
    }
};
#endif

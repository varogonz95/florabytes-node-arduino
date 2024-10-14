#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BuiltInLed.h>
#include <CustomBleServerCallbacks.h>
#include <GSON.h>
#include <UserDataCharacteristicsCallbacks.h>
#include <WiFi.h>
#include <Workflow.h>

#define SERVICE_UUID "0000181c-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002a9f-0000-1000-8000-00805f9b34fb"

BLEServer *pServer = nullptr;
BLEService *pService = nullptr;

void initBLE()
{
    Serial.println("Initializing BLE service.");

    const char *macAddress = WiFi.macAddress().c_str();

    BLEDevice::init("FloraBytes");
    // BLEDevice::init(strcat("FloraBytes-", macAddress));

    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristic->setCallbacks(new UserDataCharacteristicsCallbacks());

    pServer->setCallbacks(new CustomBleServerCallbacks);
    pService->start();
    Serial.println("BLE service started.");

    BLEAdvertising *pAdvertising = pServer->getAdvertising(); // this still is working for backward compatibility
    pAdvertising->setAppearance(0x0700);
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    // pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    // pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
    // BLEDevice::startAdvertising();

    Workflow::setState(BLE_WAITING_TO_PAIR);
    Serial.println("BLE service advertisement started.");
}

void connectToWifi(String ssid, String pwd)
{
    if (!WiFi.enableSTA(true))
        throw std::runtime_error("Could not enable WiFi in STA mode.");

    Serial.printf("Connecting to [%s]  network", ssid);
    WiFi.begin(ssid, pwd);
    
    while (WiFi.status() != WL_CONNECTED)
    {
        BuiltInLed::toggle();
        Serial.print(".");
        delay(500);
    }

    WiFi.setAutoReconnect(true);
    BuiltInLed::on();

    Serial.println("WiFi connected, IP address: " + WiFi.localIP().toString());
    Workflow::setState(WIFI_CONNECTED);
}

void setup()
{
    // enable serial port
    Serial.begin(115200);

    BuiltInLed::setup();

    initBLE();
}

void loop()
{
    switch (Workflow::getState())
    {
    case BLE_WAITING_TO_PAIR:
    {
        BuiltInLed::toggle();
        delay(1000);
    }
    break;

    case BLE_PAIRED:
    {
        BuiltInLed::blink(200, 3);
        delay(500);
    }
    break;

    case WIFI_CREDENTIALS_RECEIVED:
    {
        auto wfDataPtr = Workflow::getData();
        auto rawData = wfDataPtr.get();
        auto len = Workflow::getDataLength();
        std::string wifiJson(rawData, rawData + len);
        auto rawJson = wifiJson.c_str();
        Serial.printf("Wifi Credentials received:\n%s\n", rawJson);

        GSON::Parser parser;
        parser.parse(rawJson);
        wfDataPtr.reset();

        if (parser.hasError())
        {
            Serial.printf("Could not parse WiFi credentials. Reason: %s\n", parser.readError());
            throw std::runtime_error("Could not parse WiFi credentials.");
        }

        auto ssid = parser["Ssid"].toString();
        auto pwd = parser["Password"].toString();

        // Free BLE memory stack before enabling WiFi
        BLEDevice::deinit(true);

        connectToWifi(ssid, pwd);
    }
    break;

    case WIFI_CONNECTED:
    {
    }
    break;

    default:
        delay(1000);
        break;
    }
}

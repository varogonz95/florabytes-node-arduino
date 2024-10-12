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

void initWifi()
{
    auto wfDataPtr = Workflow::getData();
    auto rawData = wfDataPtr.get();
    auto len = Workflow::getDataLength();
    Serial.printf("Wifi Credentials received:\n%s\n", rawData);

    GSON::Parser parser;
    parser.parse((const char *)rawData);
    // wfDataPtr.reset();

    if (parser.hasError())
    {
        Serial.printf("Could not parse WiFi credentials. Reason: %s\n", parser.readError());
        throw std::runtime_error("Could not parse WiFi credentials.");
    }

    auto ssid = parser["Ssid"];
    auto pwd = parser["Password"];

    Serial.println("Connecting to Wifi network:");
    Serial.printf("Ssid: %s\n", ssid.c_str());
    Serial.printf("Password: %s\n", pwd.c_str());

    auto isSTAEnabled = WiFi.enableSTA(true);

    wl_status_t connectionState = WL_IDLE_STATUS;
    if (isSTAEnabled)
        connectionState = WiFi.begin(ssid, pwd);
    else
        throw std::runtime_error("Could not enable WiFi in STA mode.");

    while (true)
    {
        if (WL_CONNECTED == connectionState)
        {
            WiFi.setAutoReconnect(true);
            BuilInLed::on();
            break;
        }
        else if (WL_CONNECT_FAILED == WiFi.status())
        {
            throw std::runtime_error("Could not connect to WiFi network.");
        }
        else
            BuilInLed::toggle();

        delay(500);
    }

    Serial.println("Wifi connected.");
}

void setup()
{
    // enable serial port
    Serial.begin(115200);

    BuilInLed::setup();

    initBLE();
}

void loop()
{
    switch (Workflow::getState())
    {
    case BLE_WAITING_TO_PAIR:
        BuilInLed::toggle();
        delay(1000);
        break;

    case BLE_PAIRED:
        BuilInLed::blink(200, 3);
        delay(500);
        break;

    case WIFI_CREDENTIALS_RECEIVED:
        BLEDevice::stopAdvertising();
        pService->stop();
        initWifi();

    default:
        delay(1000);
        break;
    }
}

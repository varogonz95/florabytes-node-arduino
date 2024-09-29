// /*
//  * Blink
//  * Turns on an LED on for one second,
//  * then off for one second, repeatedly.
//  */
// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEServer.h>
// #include <WiFi.h>

// using namespace std;

// // Set LED_BUILTIN if it is not defined by Arduino framework
// #ifndef LED_BUILTIN
// #define LED_BUILTIN 2
// #endif

// // Constants
// const string BleBaseUuid = "00001101-0000-1000-8000-00805f9b34fb";
// const uint32_t ServiceUuid = 0x181C;
// const uint32_t CharacteristicUuid = 0x2A9F;

// // Global variables
// string deviceName;

// void setup()
// {
//     // initialize LED digital pin as an output.
//     pinMode(LED_BUILTIN, OUTPUT);

//     // String macAddress = WiFi.macAddress();
//     // char *c = new char[macAddress.length()];
//     // macAddress.toCharArray(c, sizeof(c), 0);

//     BLEDevice::init("FloraBytes Node");
//     BLEServer *bleServer = BLEDevice::createServer();
//     BLEService *bleService = bleServer->createService(ServiceUuid);
//     BLECharacteristic *bleCharacteristic = bleService->createCharacteristic(CharacteristicUuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

//     try
//     {
//         bleCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE);
//         bleService->addCharacteristic(bleCharacteristic);
//         bleServer->startAdvertising();

//         digitalWrite(LED_BUILTIN, HIGH);
//     }
//     catch (const std::exception &e)
//     {
//         digitalWrite(LED_BUILTIN, LOW);
//     }
// }

// void loop()
// {
// }

#include "unity.h"
#include "BLEHelpers.h"

void setUp(void)
{
    // set stuff up here
}

void test_(void)
{
    char *baseUuid = "00000000-0000-1000-8000-00805F9B34FB";
    int uuidAlias = 0x1234;
    std::string expectedUuid = "00001234-0000-1000-8000-00805F9B34FB";
    auto uuidBytes = BLEHelpers::mapFromAlias(uuidAlias, baseUuid);
    std::string actualUuid = BLEHelpers::toUUIDString(uuidBytes);

    TEST_ASSERT_EQUAL_STRING(expectedUuid.data(), actualUuid.data());
}

void test_function_should_doAlsoDoBlah(void)
{
    // more test stuff
}

void tearDown(void)
{
    // clean stuff up here
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_);
    return UNITY_END();
}

int main(void)
{
    return runUnityTests();
}
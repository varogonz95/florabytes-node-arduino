#include <Arduino.h>

#include <AzClientWorkflow.h>
#include <BLEWorkflow.h>
#include <ConnectionWorkflow.h>
#include <WifiWorkflow.h>
#include <Workflow.h>

#include <WirelessMutex.h>

void connectWifiTask(void *params)
{
    xSemaphoreTake(xWirelessMutex, portMAX_DELAY);

    auto device_id = (String *)params;
    auto data_ptr = Workflow::getData();
    auto data_bytes = data_ptr.get();
    auto len = Workflow::getDataLength();

    std::string wifi_json_str(data_bytes, data_bytes + len);
    String wifi_json_string(wifi_json_str.c_str());

    Logger.Info("Wifi Credentials received: ");
    Serial.println(wifi_json_string);

    JSONVar json;
    String ssid, pwd;

    try
    {
        json = JSON.parse(wifi_json_string);
        ssid = (std::remove_const<const char>::type *)((const char *)json["Ssid"]);
        pwd = (std::remove_const<const char>::type *)((const char *)json["Password"]);

        Logger.Info("Setting Ssid = ");
        Serial.println(ssid);
        Logger.Info("Setting Password = ");
        Serial.println(pwd);
    }
    catch (const std::exception &e)
    {
        Serial.printf("Could not parse WiFi credentials. Reason: %s\n", e.what());
        throw std::runtime_error("Could not parse WiFi credentials.");
    }

    BLEWorkflow::freeResources();
    WifiWorkflow::connect(ssid, pwd);
    ConnectionWorkflow::initializeTime();

    auto device_key = ConnectionWorkflow::registerDevice(*device_id);
    AzClientWorkflow::initializeIoTHubClient(*device_id);
    AzClientWorkflow::initializeMqttClient(device_key);

    xSemaphoreGive(xWirelessMutex);
}
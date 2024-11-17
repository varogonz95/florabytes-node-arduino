#include <Arduino.h>

// BLE advertisement libraries
#include <CustomBleServerCallbacks.h>
#include <UserDataCharacteristicsCallbacks.h>

// C99 libraries
#include <cstdlib>
#include <string.h>
#include <time.h>

// Libraries for MQTT client and WiFi connection
#include <mqtt_client.h>

// Azure IoT SDK for C includes
#include <az_core.h>
#include <az_iot.h>

#include <macros.h>
#include <BuiltInLed.h>
#include <DeviceInfo.h>
#include <SerialLogger.h>
#include <Workflow.h>

#include <AzClientWorkflow.h>
#include <BLEWorkflow.h>
#include <ConnectionWorkflow.h>
#include <TelemetryWorkflow.h>
#include <WifiWorkflow.h>

// When developing for your own Arduino-based platform,
// please follow the format '(ard;<platform>)'.
#define AZURE_SDK_CLIENT_USER_AGENT "c%2F" AZ_SDK_VERSION_STRING "(ard;esp32)"

// Utility macros and defines
#define NTP_SERVERS "pool.ntp.org", "time.nist.gov"
#define MQTT_QOS1 1
#define DO_NOT_RETAIN_MSG 0
#define SAS_TOKEN_DURATION_IN_MINUTES 60
#define UNIX_TIME_NOV_13_2017 1510592825

#define PST_TIME_ZONE -8
#define PST_TIME_ZONE_DAYLIGHT_SAVINGS_DIFF 1

#define GMT_OFFSET_SECS (PST_TIME_ZONE * 3600)
#define GMT_OFFSET_SECS_DST ((PST_TIME_ZONE + PST_TIME_ZONE_DAYLIGHT_SAVINGS_DIFF) * 3600)



// Wifi stuff
#define SSID_IDX 0
#define PWD_IDX 1

// Translate iot_configs.h defines into variables used by the sample
static String ssid = IOT_CONFIG_WIFI_SSID;
static String password = IOT_CONFIG_WIFI_PASSWORD;
static String device_id = DeviceInfo::DeviceId;
static char *device_key = IOT_CONFIG_DEVICE_KEY;
static const char *host = IOT_CONFIG_IOTHUB_FQDN;
static const char *mqtt_broker_uri = "mqtts://" IOT_CONFIG_IOTHUB_FQDN;
static const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;

// Memory allocated for variables and structures.
static esp_mqtt_client_handle_t mqtt_client;
static az_iot_hub_client client;

static char mqtt_client_id[128];
static char mqtt_username[128];
static char mqtt_password[256];
static uint8_t sas_signature_buffer[256];
static unsigned long next_telemetry_send_time_ms = 0;
static char telemetry_topic[128];
static uint32_t telemetry_send_count = 0;
static String telemetry_payload = "{}";

#define INCOMING_DATA_BUFFER_SIZE 128
static char incoming_data[INCOMING_DATA_BUFFER_SIZE];

static void setWifiCredentials()
{
    auto data_ptr = Workflow::getData();
    auto data_bytes = data_ptr.get();
    auto len = Workflow::getDataLength();

    std::string wifi_json_str(data_bytes, data_bytes + len);
    String wifi_json_string(wifi_json_str.c_str());

    Logger.Info("Wifi Credentials received: ");
    Serial.println(wifi_json_string);

    JSONVar json = null;
    try
    {
        json = JSON.parse(wifi_json_string);
        ssid = (std::remove_const<const char>::type *)((const char *)json["Ssid"]);
        password = (std::remove_const<const char>::type *)((const char *)json["Password"]);

        Logger.Info("Setting Ssid = ");
        Serial.println(ssid);
        Logger.Info("Setting Password = ");
        Serial.println(password);
    }
    catch (const std::exception &e)
    {
        Serial.printf("Could not parse WiFi credentials. Reason: %s\n", e.what());
        throw std::runtime_error("Could not parse WiFi credentials.");
    }
}

void receivedCallback(char *topic, byte *payload, unsigned int length)
{
    Logger.Info("Received [");
    Logger.Info(topic);
    Logger.Info("]: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println("");
}

/*
 * @brief           Gets the number of seconds since UNIX epoch until now.
 * @return uint32_t Number of seconds.
 */
static uint32_t getEpochTimeInSecs() { return (uint32_t)time(NULL); }

static void generateTelemetryPayload()
{
    // You can generate the JSON using any lib you want. Here we're showing how to do it manually, for simplicity.
    // This sample shows how to generate the payload using a syntax closer to regular delevelopment for Arduino, with
    // String type instead of az_span as it might be done in other samples. Using az_span has the advantage of reusing the
    // same char buffer instead of dynamically allocating memory each time, as it is done by using the String type below.
    telemetry_payload = "{ \"msgCount\": " + String(telemetry_send_count++) + " }";
}

static void sendTelemetry()
{
    Logger.Info("Sending telemetry ...");

    // The topic could be obtained just once during setup,
    // however if properties are used the topic need to be generated again to reflect the
    // current values of the properties.
    if (az_result_failed(az_iot_hub_client_telemetry_get_publish_topic(
            &client, NULL, telemetry_topic, sizeof(telemetry_topic), NULL)))
    {
        Logger.Error("Failed az_iot_hub_client_telemetry_get_publish_topic");
        return;
    }

    generateTelemetryPayload();

    if (esp_mqtt_client_publish(
            mqtt_client,
            telemetry_topic,
            (const char *)telemetry_payload.c_str(),
            telemetry_payload.length(),
            MQTT_QOS1,
            DO_NOT_RETAIN_MSG) == 0)
    {
        Logger.Error("Failed publishing");
    }
    else
    {
        Logger.Info("Message published successfully");
    }
}


// Arduino setup and loop main functions.

void setup()
{
    BuiltInLed::setup();
    Logger.Begin(SERIAL_LOGGER_BAUD_RATE);
    
    device_id = WiFi.macAddress();
    BLEWorkflow::startAdvertising(device_id);
    // establishConnection();
}

void loop()
{
    try
    {
        switch (Workflow::getState())
        {
        case BLE_ADVERTISING:
        {
            BuiltInLed::toggle();
            delay(500);
        }
        break;

        case BLE_PAIRED:
        {
            BuiltInLed::blink(250, 3, LOW);
            delay(500);
        }
        break;

        case WIFI_CREDENTIALS_RECEIVED:
        {
            setWifiCredentials();
            BLEWorkflow::freeResources();
            WifiWorkflow::connect(ssid, password);
            
            ConnectionWorkflow::initializeTime();
            device_key = ConnectionWorkflow::registerDevice(device_id);

            AzClientWorkflow::initializeIoTHubClient(device_id);
            AzClientWorkflow::initializeMqttClient(device_key);
        }
        break;

        default:
        {
            delay(1000);
            Logger.Info("Device in IDLE.");
            //             if (WiFi.status() != WL_CONNECTED)
            //             {
            //                 connectToWiFi(ssid, password);
            //             }
            // #ifndef IOT_CONFIG_USE_X509_CERT
            //             else if (sasToken.IsExpired())
            //             {
            //                 Logger.Info("SAS token expired; reconnecting with a new one.");
            //                 (void)esp_mqtt_client_destroy(mqtt_client);
            //                 initializeMqttClient();
            //             }
            // #endif
            //             else if (millis() > next_telemetry_send_time_ms)
            //             {
            //                 sendTelemetry();
            //                 next_telemetry_send_time_ms = millis() + TELEMETRY_FREQUENCY_MILLISECS;
            //             }
        }
        break;
        }
    }
    catch (const std::exception &e)
    {
        Logger.Error(e.what());
        throw;
    }
}

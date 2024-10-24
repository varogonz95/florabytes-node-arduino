#include <Arduino.h>

// BLE advertisement libraries
#include <BLEDevice.h>
#include <BLEServer.h>
#include <CustomBleServerCallbacks.h>
#include <UserDataCharacteristicsCallbacks.h>

// C99 libraries
#include <cstdlib>
#include <string.h>
#include <time.h>

// Libraries for MQTT client and WiFi connection
#include <WiFi.h>
#include <mqtt_client.h>

// Azure IoT SDK for C includes
#include <az_core.h>
#include <az_iot.h>
#include <azure_ca.h>
#include <az_result_codes.h>
#include <AzIoTSasToken.h>
#include <AzureIotHubConfigs.h>

// Additional helpers and utils
#include <macros.h>
#include <BuiltInLed.h>
#include <GSON.h>
#include <SerialLogger.h>
#include <Workflow.h>

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

// BLE Advertised services defines
#define BLE_SERVICE_UUID "0000181c-0000-1000-8000-00805f9b34fb"
#define BLE_CHARACTERISTIC_UUID "00002a9f-0000-1000-8000-00805f9b34fb"

// Wifi stuff
#define SSID_IDX 0
#define PWD_IDX 1

// Translate iot_configs.h defines into variables used by the sample
static String ssid = IOT_CONFIG_WIFI_SSID;
static String password = IOT_CONFIG_WIFI_PASSWORD;
static const char *device_id = WiFi.macAddress().c_str();
static const char *host = IOT_CONFIG_IOTHUB_FQDN;
static const char *mqtt_broker_uri = "mqtts://" IOT_CONFIG_IOTHUB_FQDN;
static const int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;

// Memory allocated for the sample's variables and structures.
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

// Auxiliary functions
#ifndef IOT_CONFIG_USE_X509_CERT
static AzIoTSasToken sasToken(
    &client,
    AZ_SPAN_FROM_STR(IOT_CONFIG_DEVICE_KEY),
    AZ_SPAN_FROM_BUFFER(sas_signature_buffer),
    AZ_SPAN_FROM_BUFFER(mqtt_password));
#endif // IOT_CONFIG_USE_X509_CERT

static std::tuple<String, String> getWifiTuple()
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

    return std::make_tuple(ssid, pwd);
}

static void connectToWiFi(String ssid, String password)
{
    Logger.Info("Connecting to WIFI SSID " + ssid);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    WiFi.begin(ssid, password);
    const uint max_retries = 5;
    const uint conn_check_cycle = 500;
    uint retriesCount = max_retries * (1000 / conn_check_cycle);
    wl_status_t status;

    while (retriesCount-- > 0 || status != WL_CONNECTED)
    {
        BuiltInLed::toggle();
        status = status = WiFi.status();

        Serial.print(".");
        delay(conn_check_cycle);
    }

    if (status == WL_CONNECT_FAILED)
    {
        BuiltInLed::off();
        char *fmtError;
        sprintf(fmtError, "Could not connect to [%s] network. Reason: %s", ssid.c_str(), String(status));
        throw std::runtime_error(fmtError);
    }

    Serial.println("");
    Logger.Info("WiFi connected, IP address: " + WiFi.localIP().toString());

    BuiltInLed::on();
    Workflow::setState(WIFI_CONNECTED);
}

static void initializeTime()
{
    Logger.Info("Setting time using SNTP");

    configTime(GMT_OFFSET_SECS, GMT_OFFSET_SECS_DST, NTP_SERVERS);
    time_t now = time(NULL);
    while (now < UNIX_TIME_NOV_13_2017)
    {
        BuiltInLed::toggle();
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("");
    Logger.Info("Time initialized!");
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

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    (void)handler_args;
    (void)base;
    (void)event_id;

    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
#else  // ESP_ARDUINO_VERSION_MAJOR
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
#endif // ESP_ARDUINO_VERSION_MAJOR
    switch (event->event_id)
    {
        int i, r;

    case MQTT_EVENT_ERROR:
        Logger.Info("MQTT event MQTT_EVENT_ERROR");
        break;
    case MQTT_EVENT_CONNECTED:
        Logger.Info("MQTT event MQTT_EVENT_CONNECTED");

        r = esp_mqtt_client_subscribe(mqtt_client, AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC, 1);
        if (r == -1)
        {
            Logger.Error("Could not subscribe for cloud-to-device messages.");
        }
        else
        {
            Logger.Info("Subscribed for cloud-to-device messages; message id:" + String(r));
        }

        break;
    case MQTT_EVENT_DISCONNECTED:
        Logger.Info("MQTT event MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        Logger.Info("MQTT event MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        Logger.Info("MQTT event MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        Logger.Info("MQTT event MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        Logger.Info("MQTT event MQTT_EVENT_DATA");

        for (i = 0; i < (INCOMING_DATA_BUFFER_SIZE - 1) && i < event->topic_len; i++)
        {
            incoming_data[i] = event->topic[i];
        }
        incoming_data[i] = '\0';
        Logger.Info("Topic: " + String(incoming_data));

        for (i = 0; i < (INCOMING_DATA_BUFFER_SIZE - 1) && i < event->data_len; i++)
        {
            incoming_data[i] = event->data[i];
        }
        incoming_data[i] = '\0';
        Logger.Info("Data: " + String(incoming_data));

        break;
    case MQTT_EVENT_BEFORE_CONNECT:
        Logger.Info("MQTT event MQTT_EVENT_BEFORE_CONNECT");
        break;
    default:
        Logger.Error("MQTT event UNKNOWN");
        break;
    }

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
#else  // ESP_ARDUINO_VERSION_MAJOR
    return ESP_OK;
#endif // ESP_ARDUINO_VERSION_MAJOR
}

static void initializeIoTHubClient()
{
    if (az_result_failed(az_iot_hub_client_init(
            &client,
            az_span_create((uint8_t *)host, strlen(host)),
            az_span_create((uint8_t *)device_id, strlen(device_id)),
            NULL)))
    {
        Logger.Error("Failed initializing Azure IoT Hub client");
        throw std::runtime_error("");
    }

    size_t client_id_length;
    if (az_result_failed(az_iot_hub_client_get_client_id(
            &client, mqtt_client_id, sizeof(mqtt_client_id) - 1, &client_id_length)))
    {
        Logger.Error("Failed getting client id");
        throw std::runtime_error("");
    }

    // Get the MQTT user name used to connect to IoT Hub
    if (az_result_failed(az_iot_hub_client_get_user_name(
            &client, mqtt_username, sizeofarray(mqtt_username), NULL)))
    {
        Logger.Error("Failed to get MQTT clientId, return code");
        throw std::runtime_error("");
    }

    Logger.Info("Client ID: " + String(mqtt_client_id));
    Logger.Info("Username: " + String(mqtt_username));
}

static int initializeMqttClient()
{
#ifndef IOT_CONFIG_USE_X509_CERT
    if (sasToken.Generate(SAS_TOKEN_DURATION_IN_MINUTES) != 0)
    {
        Logger.Error("Failed generating SAS token");
        return 1;
    }
#endif

    esp_mqtt_client_config_t mqtt_config;
    memset(&mqtt_config, 0, sizeof(mqtt_config));

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    mqtt_config.broker.address.uri = mqtt_broker_uri;
    mqtt_config.broker.address.port = mqtt_port;
    mqtt_config.credentials.client_id = mqtt_client_id;
    mqtt_config.credentials.username = mqtt_username;

#ifdef IOT_CONFIG_USE_X509_CERT
    LogInfo("MQTT client using X509 Certificate authentication");
    mqtt_config.credentials.authentication.certificate = IOT_CONFIG_DEVICE_CERT;
    mqtt_config.credentials.authentication.certificate_len = (size_t)sizeof(IOT_CONFIG_DEVICE_CERT);
    mqtt_config.credentials.authentication.key = IOT_CONFIG_DEVICE_CERT_PRIVATE_KEY;
    mqtt_config.credentials.authentication.key_len = (size_t)sizeof(IOT_CONFIG_DEVICE_CERT_PRIVATE_KEY);
#else // Using SAS key
    mqtt_config.credentials.authentication.password = (const char *)az_span_ptr(sasToken.Get());
#endif

    mqtt_config.session.keepalive = 30;
    mqtt_config.session.disable_clean_session = 0;
    mqtt_config.network.disable_auto_reconnect = false;
    mqtt_config.broker.verification.certificate = (const char *)ca_pem;
    mqtt_config.broker.verification.certificate_len = (size_t)ca_pem_len;
#else // ESP_ARDUINO_VERSION_MAJOR
    mqtt_config.uri = mqtt_broker_uri;
    mqtt_config.port = mqtt_port;
    mqtt_config.client_id = mqtt_client_id;
    mqtt_config.username = mqtt_username;

#ifdef IOT_CONFIG_USE_X509_CERT
    AzLogger.Info("MQTT client using X509 Certificate authentication");
    mqtt_config.client_cert_pem = IOT_CONFIG_DEVICE_CERT;
    mqtt_config.client_key_pem = IOT_CONFIG_DEVICE_CERT_PRIVATE_KEY;
#else // Using SAS key
    mqtt_config.password = (const char *)az_span_ptr(sasToken.Get());
#endif
    mqtt_config.keepalive = 30;
    mqtt_config.disable_clean_session = 0;
    mqtt_config.disable_auto_reconnect = false;
    mqtt_config.event_handle = mqtt_event_handler;
    mqtt_config.user_context = NULL;
    mqtt_config.cert_pem = (const char *)ca_pem;
#endif // ESP_ARDUINO_VERSION_MAJOR

    mqtt_client = esp_mqtt_client_init(&mqtt_config);

    if (mqtt_client == NULL)
    {
        Logger.Error("Failed creating mqtt client");
        return 1;
    }

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    esp_mqtt_client_register_event(mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);
#endif // ESP_ARDUINO_VERSION_MAJOR

    esp_err_t start_result = esp_mqtt_client_start(mqtt_client);

    if (start_result != ESP_OK)
    {
        Logger.Error("Could not start mqtt client; error code:" + start_result);
        return 1;
    }
    else
    {
        Logger.Info("MQTT client started");
        return 0;
    }
}

/*
 * @brief           Gets the number of seconds since UNIX epoch until now.
 * @return uint32_t Number of seconds.
 */
static uint32_t getEpochTimeInSecs() { return (uint32_t)time(NULL); }

static void establishConnection()
{
    initializeTime();
    Serial.println(" [APP] After Time init Free memory: " + String(esp_get_free_heap_size()) + " bytes");
    initializeIoTHubClient();
    (void)initializeMqttClient();
}

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

static BLEServer *pServer = nullptr;
static BLEService *pService = nullptr;

static void initializeBLEAdvertisement()
{
    Serial.println("Initializing BLE service.");
    BLEDevice::init("FloraBytes (" + std::string(device_id) + ")");

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
    pAdvertising->setAppearance(0x0700);
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    // pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    // pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
    // BLEDevice::startAdvertising();

    Workflow::setState(BLE_ADVERTISING);
    Serial.println("BLE service advertisement started.");
}

// Arduino setup and loop main functions.

void setup()
{
    Logger.Begin(SERIAL_LOGGER_BAUD_RATE);
    BuiltInLed::setup();

    Serial.println(" [APP] Initial Free memory: " + String(esp_get_free_heap_size()) + " bytes");
    initializeBLEAdvertisement();
    Serial.println(" [APP] After BLE init Free memory: " + String(esp_get_free_heap_size()) + " bytes");
    // establishConnection();
}

void loop()
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
        auto wifiTuple = getWifiTuple();
        ssid = std::get<SSID_IDX>(wifiTuple);
        password = std::get<PWD_IDX>(wifiTuple);

        // Free BLE resource stack prior to
        // activating WiFi stack
        BLEDevice::deinit(true);
        free(nullptr);
        Serial.println(" [APP] After BLE deinit Free memory: " + String(esp_get_free_heap_size()) + " bytes");

        connectToWiFi(ssid, password);
        Serial.println(" [APP] After Wifi init Free memory: " + String(esp_get_free_heap_size()) + " bytes");
        establishConnection();
    }
    break;

    default:
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            connectToWiFi(ssid, password);
        }
#ifndef IOT_CONFIG_USE_X509_CERT
        else if (sasToken.IsExpired())
        {
            Logger.Info("SAS token expired; reconnecting with a new one.");
            (void)esp_mqtt_client_destroy(mqtt_client);
            initializeMqttClient();
        }
#endif
        else if (millis() > next_telemetry_send_time_ms)
        {
            sendTelemetry();
            next_telemetry_send_time_ms = millis() + TELEMETRY_FREQUENCY_MILLISECS;
        }
    }
    break;
    }
}

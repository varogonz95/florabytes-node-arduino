#ifndef _AZ_CLIENT_WORKFLOW_
#define _AZ_CLIENT_WORKFLOW_

#include <az_core.h>
#include <az_iot.h>
#include <azure_ca.h>
#include <az_result_codes.h>
#include <mqtt_client.h>

#include <macros.h>
#include <AzIoTSasToken.h>
#include <AzureIotHubConfigs.h>
#include <SerialLogger.h>

#define SAS_TOKEN_DURATION_IN_MINUTES 60
#define INCOMING_DATA_BUFFER_SIZE 128

namespace AzClientWorkflow
{
    static az_iot_hub_client client;
    static esp_mqtt_client_handle_t mqtt_client;

    static char mqtt_client_id[128];
    static char mqtt_username[128];
    static char mqtt_password[256];

    static uint8_t sas_signature_buffer[256];
    static char incoming_data[INCOMING_DATA_BUFFER_SIZE];

    const static char *host = IOT_CONFIG_IOTHUB_FQDN;
    const static char *mqtt_broker_uri = "mqtts://" IOT_CONFIG_IOTHUB_FQDN;
    const static int mqtt_port = AZ_IOT_DEFAULT_MQTT_CONNECT_PORT;

    static void initializeIoTHubClient(String device_id)
    {
        auto device_id_chr = device_id.c_str();
        if (az_result_failed(az_iot_hub_client_init(
                &client,
                az_span_create((uint8_t *)host, strlen(host)),
                az_span_create((uint8_t *)device_id_chr, strlen(device_id_chr)),
                NULL)))
        {
            Logger.Error("Failed initializing Azure IoT Hub client", true);
        }

        size_t client_id_length;
        if (az_result_failed(az_iot_hub_client_get_client_id(
                &client, mqtt_client_id, sizeof(mqtt_client_id) - 1, &client_id_length)))
        {
            Logger.Error("Failed getting client id", true);
        }

        // Get the MQTT user name used to connect to IoT Hub
        if (az_result_failed(az_iot_hub_client_get_user_name(
                &client, mqtt_username, sizeofarray(mqtt_username), NULL)))
        {
            Logger.Error("Failed to get MQTT clientId, return code", true);
        }

        Logger.Info("Client ID: " + String(mqtt_client_id));
        Logger.Info("Username: " + String(mqtt_username));
    }

    static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
    {
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

        return ESP_OK;
    }

    static int initializeMqttClient(char *device_key)
    {
#ifndef IOT_CONFIG_USE_X509_CERT
        Logger.Info("Device Key: ");
        Serial.print(device_key);
        Serial.println();

        AzIoTSasToken sasToken(
            &client,
            az_span_create_from_str(device_key),
            AZ_SPAN_FROM_BUFFER(sas_signature_buffer),
            AZ_SPAN_FROM_BUFFER(mqtt_password));

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
};
#endif

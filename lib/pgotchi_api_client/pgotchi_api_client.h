#include <map>
#include <Arduino_JSON.h>
#include <HTTPClient.h>

#include <SerialLogger.h>
#include <azurecert.h>

#include "pgotchi_api_client_config.h"

class PgotchiApiClient
{
private:
    const char *m_base_url = PGOTCHI_API_URL;
    HTTPClient *m_http_client = new HTTPClient;

public:
    PgotchiApiClient() {}

    inline JSONVar RegisterDevice(String deviceId)
    {
        String url = String(m_base_url) + "Device";
        String fmt_template = "{\"deviceId\":\"%s\"}";
        const char *device_id_chr = deviceId.c_str();
        char buffer[(fmt_template.length()) + (deviceId.length())];
        sprintf(buffer, fmt_template.c_str(), deviceId.c_str());
        String request_payload(buffer);

        Logger.Info("Performing Http request...");
        Logger.Info("POST " + url);
        Logger.Info("Request-Body: " + request_payload);

#ifndef PGOTCHI_USE_LOCALHOST
        if (!m_http_client->begin(url, AZURE_ROOT_CA_CERT))
#else
        if (!m_http_client->begin(url))
#endif
        {
            Logger.Error("Could not begin Http connection.", true);
        }

        m_http_client->addHeader("Content-Type", "application/json");
        auto http_status_code = m_http_client->POST(request_payload);

        Logger.Info("Http response code: ");
        Serial.print(http_status_code);
        Serial.println();
        if (http_status_code < 200 || http_status_code > 201)
        {
            Logger.Error("Http Error Found.");
            Logger.Error(m_http_client->errorToString(http_status_code), true);
        }

        auto response_payload = m_http_client->getString();
        m_http_client->end();
        Logger.Info("Payload: ");
        Serial.print(response_payload);
        Serial.println();

        JSONVar json = null;
        try
        {
            json = JSON.parse(response_payload);
        }
        catch (const std::exception &e)
        {
            Logger.Error("Could not parse RegisterDevice response. Reason: ");
            Serial.print(e.what());
            Serial.println();
            throw std::runtime_error(e.what());
        }

        return json;
    }

    ~PgotchiApiClient()
    {
        m_http_client->end();
        m_http_client = nullptr;
    }
};

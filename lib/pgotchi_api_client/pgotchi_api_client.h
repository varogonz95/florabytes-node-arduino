#include <map>
#include <GSON.h>
#include <HTTPClient.h>

#include <SerialLogger.h>

#include <azurecert.h>

#include "pgotchi_api_client_config.h"

class PgotchiApiClient
{
private:
    static const char *m_base_url;
    HTTPClient *m_http_client;

public:
    PgotchiApiClient();

    void RegisterDevice(String deviceId);
    void RegisterDevice(String deviceId, std::map<std::string, std::string> properties);

    ~PgotchiApiClient();
};

const char *PgotchiApiClient::m_base_url = PGOTCHI_API_URL;

inline PgotchiApiClient::PgotchiApiClient()
{
    m_http_client = new HTTPClient();
}

inline void PgotchiApiClient::RegisterDevice(String deviceId)
{
    bool hasClientBegun = false;
    String url = String(m_base_url) + "Device";

#ifndef PGOTCHI_USE_LOCALHOST
    if (url.startsWith("https"))
        hasClientBegun = m_http_client->begin(url, AZURE_ROOT_CA_CERT);
    else
#endif
        hasClientBegun = m_http_client->begin(url);

    if (!hasClientBegun)
    {
        Logger.Error("Could not begin Http connection.", true);
    }

    String fmt_template = "{\"deviceId\":\"%s\"}";
    const char *device_id_chr = deviceId.c_str();
    char buffer[(fmt_template.length()) + (deviceId.length())];
    sprintf(buffer, fmt_template.c_str(), deviceId.c_str());
    String payload(buffer);

    Logger.Info("Performing Http request...");
    Logger.Info("POST " + url);
    Logger.Info("Request-Body: " + payload);
    delay(100);

    m_http_client->addHeader("Content-Type", "application/json");
    auto http_response = m_http_client->POST(payload);
    delay(100);

    Logger.Info("Http response code: " + http_response);
    if (http_response < 200 || http_response > 201)
    {
        Logger.Error("Http Error: " + String(http_response), true);
    }

    // auto response_size = m_http_client->getSize();
    // auto stream = m_http_client->getStream();
    // char *response_buffer = (char *)malloc(sizeof(char) * response_size);
    // Logger.Info("Content-Length: " + response_size);

    // if (response_buffer == nullptr)
    // {
    //     Logger.Error("Could not allocate memory for HTTP response", true);
    // }

    // auto chars = stream.readBytes(response_buffer, response_size);
    // m_http_client->end();

    // if (chars == 0)
    // {
    //     Logger.Error("Could not read Http response from stream.", true);
    // }

    delay(100);
    auto response_data = m_http_client->getString();
    m_http_client->end();
    Logger.Info("Response: " + response_data);

    // std::string json(response_buffer, response_buffer + response_size);
    // Logger.Info("Response-Body: " + String(chars));
}

inline void PgotchiApiClient::RegisterDevice(String deviceId, std::map<std::string, std::string> properties)
{
}

inline PgotchiApiClient::~PgotchiApiClient()
{
    m_http_client->end();
    m_http_client = nullptr;
}

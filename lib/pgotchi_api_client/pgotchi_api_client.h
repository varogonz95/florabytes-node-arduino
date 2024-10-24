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
        String error = "Could not begin Http connection.";
        throw std::runtime_error(error.c_str());
    }

    const char *fmt_template = "{\"deviceId\":\"%s\"}";
    const char *device_id_chr = deviceId.c_str();
    uint32_t fmt_template_len = (sizeof(fmt_template) / sizeof(fmt_template[0]));
    char buffer[fmt_template_len + (sizeof(device_id_chr) / sizeof(device_id_chr[0]))];
    sprintf(buffer, fmt_template, device_id_chr);
    String payload(buffer);

    Logger.Info("Performing Http request...");
    Logger.Info("POST " + url);
    Logger.Info("Request-Body:\n" + payload);
    m_http_client->addHeader("Content-Type", "application/json");
    auto http_response = m_http_client->POST(payload);
    
    delay(100);
    
    Logger.Info("Http response code: " + http_response);
    if (http_response < 200 || http_response > 201)
    {
        String error = "Http Error: " + String(http_response);
        throw std::runtime_error(error.c_str());
    }

    auto response_size = m_http_client->getSize();
    auto stream = m_http_client->getStream();
    char *response_buffer = (char *)malloc(sizeof(char) * response_size);
    Logger.Info("Content-Length: " + response_size);

    if (response_buffer == nullptr)
    {
        String error = "Could not allocate memory for HTTP response";
        throw std::runtime_error(error.c_str());
    }

    auto chars = stream.readBytes(response_buffer, response_size);
    m_http_client->end();
    
    delay(100);

    // std::string json(response_buffer, response_buffer + response_size);
    Logger.Info("Response-Body: " + String(response_buffer));
}

inline void PgotchiApiClient::RegisterDevice(String deviceId, std::map<std::string, std::string> properties)
{
}

inline PgotchiApiClient::~PgotchiApiClient()
{
    m_http_client->end();
    m_http_client = nullptr;
}

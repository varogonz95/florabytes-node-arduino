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

    String* RegisterDevice(String deviceId);

    ~PgotchiApiClient();
};

const char *PgotchiApiClient::m_base_url = PGOTCHI_API_URL;

inline PgotchiApiClient::PgotchiApiClient()
{
    m_http_client = new HTTPClient();
}

inline String* PgotchiApiClient::RegisterDevice(String deviceId)
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
    String request_payload(buffer);

    Logger.Info("Performing Http request...");
    Logger.Info("POST " + url);
    Logger.Info("Request-Body: " + request_payload);
    delay(100);

    m_http_client->addHeader("Content-Type", "application/json;charset=utf8");
    auto http_status_code = m_http_client->POST(request_payload);
    delay(100);

    Logger.Info("Http response code: " + http_status_code);
    if (http_status_code < 200 || http_status_code > 201)
    {
        auto err = m_http_client->errorToString(http_status_code);
        Logger.Error("Http Error: " + err, true);
    }

    auto response_payload = m_http_client->getString();
    m_http_client->end();
    Logger.Info("Payload: " + response_payload);

    GSON::Parser parser;
    parser.parse(response_payload);

    if (parser.hasError())
    {
        Logger.Error("Could not parse RegisterDevice response. Reason: " + String(parser.readError()), true);
    }
    
    auto result = new String[2] {parser["symmetricPrimaryKey"], parser["symmetricSecondaryKey"]};
    // parser.reset();

    return result;
}

inline PgotchiApiClient::~PgotchiApiClient()
{
    m_http_client->end();
    m_http_client = nullptr;
}

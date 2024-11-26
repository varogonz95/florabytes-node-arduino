#ifndef _WIFI_WORKFLOW_
#define _WIFI_WORKFLOW_

#include <Arduino.h>
#include <BuiltInLed.h>
#include <SerialLogger.h>
#include <WiFi.h>

namespace WifiWorkflow
{
    void connect(String ssid, String pwd)
    {
        Logger.Info("Connecting to WIFI SSID ");
        Serial.print(ssid);
        Serial.println();

        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        auto status = WiFi.begin(ssid, pwd);
        if (status == WL_CONNECT_FAILED)
        {
            Logger.Error("Could not connect to ");
            Serial.print(ssid);
            throw std::runtime_error("");
        }

        const uint max_retries = 5;
        const uint conn_check_cycle = 500;
        uint retriesCount = max_retries * (1000 / conn_check_cycle);

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
            sprintf(fmtError, "Could not connect to [%s] network. Reason: %s", ssid, String(status));
            throw std::runtime_error(fmtError);
        }

        Serial.println("");
        Logger.Info("WiFi connected, IP address: " + WiFi.localIP().toString());

        BuiltInLed::on();
        // Workflow::setState(WIFI_CONNECTED);
    }
};

#endif

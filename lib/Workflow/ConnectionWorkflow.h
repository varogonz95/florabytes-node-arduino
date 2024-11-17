#ifndef _CONNECTION_WORKFLOW_
#define _CONNECTION_WORKFLOW_

#include <time.h>

#include <BuiltInLed.h>
#include <SerialLogger.h>
#include <pgotchi_api_client.h>

#define NTP_SERVERS "pool.ntp.org", "time.nist.gov"
#define UNIX_TIME_NOV_13_2017 1510592825

#define PST_TIME_ZONE -8
#define PST_TIME_ZONE_DAYLIGHT_SAVINGS_DIFF 1

#define GMT_OFFSET_SECS (PST_TIME_ZONE * 3600)
#define GMT_OFFSET_SECS_DST ((PST_TIME_ZONE + PST_TIME_ZONE_DAYLIGHT_SAVINGS_DIFF) * 3600)

namespace ConnectionWorkflow
{
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

    static char *registerDevice(String device_id)
    {
        auto pgotchiApi = new PgotchiApiClient();
        auto deviceJson = pgotchiApi->RegisterDevice(device_id);
        const char *symmetricPrimaryKey = deviceJson["symmetricPrimaryKey"];
        const char *symmetricSecondaryKey = deviceJson["symmetricSecondaryKey"];

        if (!sizeof(symmetricPrimaryKey) == 0)
            return (std::remove_const<const char>::type *)symmetricPrimaryKey;
        else if (!sizeof(symmetricSecondaryKey) == 0)
            return (std::remove_const<const char>::type *)symmetricSecondaryKey;

        Logger.Error("Both Device Symmetric keys are empty!", true);
        return nullptr;
    }
} // namespace ConnectionWorkflow

#endif

#ifndef _DEVICE_INFO_
#define _DEVICE_INFO_

#include <WiFi.h>

#include <AzureIotHubConfigs.h>

namespace DeviceInfo
{
    static String DeviceId =
        std::string(IOT_CONFIG_DEVICE_ID).empty() ? WiFi.macAddress() : String(IOT_CONFIG_DEVICE_ID);
} // namespace DeviceInfo

#endif

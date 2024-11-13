#include <WiFi.h>

namespace DeviceInfo
{
    static String DeviceId =
        std::is_empty<String>(IOT_CONFIG_DEVICE_ID) ? WiFi.macAddress() : String(IOT_CONFIG_DEVICE_ID);
} // namespace DeviceInfo

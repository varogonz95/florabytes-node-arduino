#include <iostream>
#include <string>
#include <array>
#include <sstream>
#include <cstdint>

class BLEHelpers
{
    // static char *BluetoothBaseUUID = "00000000-0000-1000-8000-00805F9B34FB";
public:
    static std::array<uint8_t, 16> mapFromAlias(uint16_t alias, char *baseUuid)
    {
        // Convert the alias to a 128-bit value (uint64_t)
        auto aliasValue = static_cast<uint64_t>(alias) << 96;

        // Create a new array for the combined value
        std::array<uint8_t, 16> combinedBytes;

        // Copy the aliasValue bytes into the combinedBytes array
        for (int i = 0; i < 8; ++i)
        {
            combinedBytes[i] = static_cast<uint8_t>(aliasValue >> (56 - 8 * i));
        }

        // Copy the remaining bytes from BluetoothBaseUUID into the combinedBytes array
        auto baseUuidBytes = BLEHelpers::toByteArray(baseUuid);
        for (int i = 0; i < 8; ++i)
        {
            combinedBytes[i + 8] = baseUuidBytes[i + 4];
        }

        return combinedBytes;
    }

    static std::array<uint8_t, 16> toByteArray(char *uuidString)
    {
        std::array<uint8_t, 16> byteArray;

        // Remove hyphens from the UUID string
        std::string cleanedString;
        int len = sizeof(uuidString);
        for (size_t i = 0; i < len; i++)
        {
            if (uuidString[i] != '-')
            {
                cleanedString += uuidString[i];
            }
        }

        // Parse each pair of characters and convert to bytes
        for (size_t i = 0; i < 32; i += 2)
        {
            std::string byteString = cleanedString.substr(i, 2);
            byteArray[i / 2] = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        }

        return byteArray;
    }

    static std::string toUUIDString(std::array<uint8_t, 16> uuidBytes)
    {
        std::string res;

        for (const auto &byte : uuidBytes)
        {
            char hexByte[3];
            snprintf(hexByte, sizeof(hexByte), "%02X", byte);
            res += hexByte;
        }

        return res;
    }
}; // class BLEHelpers

// int main()
// {
//     // Example usage:
//     uint16_t myAlias = 0x1234;                                         // Replace with your desired alias
//     std::string myUuidString = "550e8400-e29b-41d4-a716-446655440000"; // Replace with your UUID string

//     auto calculatedUUID = BLEHelpers::FromAlias(myAlias);
//     auto uuidBytes = BLEHelpers::toByteArray(myUuidString);

//     // Print the calculated UUID and the converted UUID string
//     std::cout << "Calculated UUID: ";
//     for (const auto &byte : calculatedUUID)
//     {
//         printf("%02X", byte);
//     }
//     std::cout << std::endl;

//     std::cout << "UUID as byte array: ";
//     for (const auto &byte : uuidBytes)
//     {
//         printf("%02X", byte);
//     }
//     std::cout << std::endl;

//     return 0;
// }

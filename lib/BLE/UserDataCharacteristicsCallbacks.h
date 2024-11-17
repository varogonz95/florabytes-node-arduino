#ifndef _USER_DATA_CHARACTERISTICS_CALLBACKS_
#define _USER_DATA_CHARACTERISTICS_CALLBACKS_

#include <BLECharacteristic.h>

class UserDataCharacteristicsCallbacks : public BLECharacteristicCallbacks
{
private:
    /* data */
public:
    UserDataCharacteristicsCallbacks(/* args */);

    /**
     * @brief Callback function to support a read request.
     * @param [in] pCharacteristic The characteristic that is the source of the event.
     * @param [in] param The BLE GATTS param. Use param->read.
     */
    void onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param);
    /**
     * @brief DEPRECATED! Callback function to support a read request. Called only if onRead(,) not overrided.
     * @param [in] pCharacteristic The characteristic that is the source of the event.
     */
    void onRead(BLECharacteristic *pCharacteristic);

    /**
     * @brief Callback function to support a write request.
     * @param [in] pCharacteristic The characteristic that is the source of the event.
     * @param [in] param The BLE GATTS param. Use param->write.
     */
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param);
    /**
     * @brief DEPRECATED! Callback function to support a write request. Called only if onWrite(,) not overrided.
     * @param [in] pCharacteristic The characteristic that is the source of the event.
     */
    void onWrite(BLECharacteristic *pCharacteristic);

    /**
     * @brief Callback function to support a Notify request.
     * @param [in] pCharacteristic The characteristic that is the source of the event.
     */
    void onNotify(BLECharacteristic *pCharacteristic);

    /**
     * @brief Callback function to support a Notify/Indicate Status report.
     * @param [in] pCharacteristic The characteristic that is the source of the event.
     * @param [in] s Status of the notification/indication
     * @param [in] code Additional code of underlying errors
     */
    void onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code);

    ~UserDataCharacteristicsCallbacks();
};

#endif

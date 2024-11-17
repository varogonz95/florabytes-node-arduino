#ifndef _CUSTOM_BLE_SERVER_CALLBACKS_
#define _CUSTOM_BLE_SERVER_CALLBACKS_

#include <BLEServer.h>

class CustomBleServerCallbacks : public BLEServerCallbacks
{
private:
    /* data */
public:
    CustomBleServerCallbacks(/* args */);

    /**
	 * @brief Handle a new client connection.
	 *
	 * When a new client connects, we are invoked.
	 *
	 * @param [in] pServer A reference to the %BLE server that received the client connection.
	 */
	void onConnect(BLEServer* pServer);
	void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param);
	/**
	 * @brief Handle an existing client disconnection.
	 *
	 * When an existing client disconnects, we are invoked.
	 *
	 * @param [in] pServer A reference to the %BLE server that received the existing client disconnection.
	 */
	void onDisconnect(BLEServer* pServer);
	void onDisconnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param);

	/**
	 * @brief Handle a new client connection.
	 *
	 * When the MTU changes this method is invoked.
	 *
	 * @param [in] pServer A reference to the %BLE server that received the client connection.
	 * @param [in] param A reference to esp_ble_gatts_cb_param_t.
	 */
	void onMtuChanged(BLEServer* pServer, esp_ble_gatts_cb_param_t* param);

    ~CustomBleServerCallbacks();
};

#endif

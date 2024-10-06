#include <stdlib.h>

enum WorkflowState
{
    BLE_INITIALIZING,
    BLE_ADVERTISING_STARTED,
    WIFI_CREDENTIALS_RECEIVED,
    WIFI_CONNECTED,
};

class Workflow
{
private:
public:
    static WorkflowState State;
    static uint8_t *Data;
    static size_t DataLength;
};

enum WorkflowState
{
    NONE,
    BLE_WAITING_TO_PAIR,
    BLE_PAIRED,
    WIFI_CREDENTIALS_RECEIVED,
    WIFI_CONNECTED,
};

class Workflow
{
private:
public:
    static WorkflowState State;
    static unsigned char *Data;
    static unsigned int DataLength;
};

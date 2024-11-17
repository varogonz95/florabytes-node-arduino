#ifndef _WORKFLOW_
#define _WORKFLOW_

#include <memory>

enum WorkflowState
{
    NONE,
    WF_IDLE,
    BLE_ADVERTISING,
    BLE_PAIRED,
    WIFI_CREDENTIALS_RECEIVED,
    WIFI_CONNECTED,
};

class Workflow
{
private:
    static WorkflowState m_state;
    static std::shared_ptr<uint8_t> m_data;
    static uint32_t m_dataLength;

public:
    static void setData(uint8_t *m_data, uint32_t length);
    static std::shared_ptr<uint8_t> getData();
    static uint32_t getDataLength();

    static void setState(WorkflowState state);
    static WorkflowState getState();
};

#endif

#include <stdlib.h>

#include "Workflow.h"

std::shared_ptr<uint8_t> Workflow::m_data = nullptr;
uint32_t Workflow::m_dataLength = 0;
WorkflowState Workflow::m_state = NONE;

void Workflow::setData(uint8_t *m_data, uint32_t length)
{
    Workflow::m_data = std::shared_ptr<uint8_t>(m_data);
    Workflow::m_dataLength = length;
}

std::shared_ptr<uint8_t> Workflow::getData()
{
    return Workflow::m_data;
}

uint32_t Workflow::getDataLength()
{
    return Workflow::m_dataLength;
}

void Workflow::setState(WorkflowState state)
{
    Workflow::m_state = state;
}

WorkflowState Workflow::getState()
{
    return Workflow::m_state;
}

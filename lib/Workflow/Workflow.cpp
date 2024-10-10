#include <stdlib.h>

#include "Workflow.h"

WorkflowState Workflow::State = NONE;
uint8_t *Workflow::Data = nullptr;
uint32_t Workflow::DataLength = 0;
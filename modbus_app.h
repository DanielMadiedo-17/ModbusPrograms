/*
 * This file serves the purpose of linking the modbus handling and the UI
 * Currently it is only set up for handling FC03 but other functions will be added
 */

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MB_UI_MAX_REGS 32

typedef struct
{
    uint16_t startAddr;
    uint16_t quantity;
    uint16_t functionCode;

    uint8_t  valid;
    uint8_t  exception;
    uint16_t regsCount;
    uint16_t regs[MB_UI_MAX_REGS];

    uint32_t seq;
} ModbusUiSnapshotC;

// call once during init
void ModbusUiBridge_Init(void);

// UI -> backend: set desired request + arm "send"
void ModbusUiBridge_SetRequest(uint16_t functionCode, uint16_t startAddr, uint16_t quantity);
void ModbusUiBridge_TriggerSend(void);

// backend -> UI: publish newest snapshot
void ModbusUiBridge_Publish(const ModbusUiSnapshotC* s);

// UI/Model: read latest snapshot (thread-safe copy). returns 1 if copied.
int  ModbusUiBridge_GetSnapshot(ModbusUiSnapshotC* out);

// backend: check/consume send trigger. returns 1 if there was a pending send.
int  ModbusUiBridge_ConsumeSendTrigger(uint16_t* functionCode, uint16_t* startAddr, uint16_t* quantity);

#ifdef __cplusplus
}
#endif

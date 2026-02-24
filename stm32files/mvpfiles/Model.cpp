#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include <stdio.h>

#include <string.h>

// bridge to backend
extern "C" {
#include "modbus_app.h"
}

Model::Model() : modelListener(0), lastSeq(0) {}

void Model::SetModbusRequest(uint16_t func, uint16_t startAddr, uint16_t qty)
{

    ModbusUiBridge_SetRequest(func, startAddr, qty);
}

void Model::triggerSend()
{


    ModbusUiBridge_TriggerSend();
}

void Model::tick()
{
    ModbusUiSnapshotC c; //The snapshot from our backend for comparison
    if (!ModbusUiBridge_GetSnapshot(&c)) return;

    if (c.seq != lastSeq) //If it has been updated, we proceed to change the UI
    {


        lastSeq = c.seq;

        ModbusUiSnapshot snap;
        snap.functionCode = c.functionCode;
        snap.startAddr = c.startAddr;
        snap.regCount  = c.quantity; //was c.regsCount
        snap.result     = c.valid;
        snap.exceptionCode = c.exception;
        snap.seq       = c.seq;

        const uint16_t n = (c.regsCount > MB_UI_MAX_REGS) ? MB_UI_MAX_REGS : c.regsCount;
        for (uint16_t i = 0; i < n; i++) snap.regs[i] = c.regs[i];

        if (modelListener)
            modelListener->onModbusUpdate(snap);
    }
}

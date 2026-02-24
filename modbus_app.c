#include "modbus_app.h"
#include "cmsis_os.h"
#include <string.h>
#include<stdio.h>

//GLOBAL VARIABLE DECLARATION
static ModbusUiSnapshotC g_snap;
static uint16_t g_reqStart = 0;
static uint16_t g_reqQty   = 1;
static uint8_t  g_sendPending = 0;
static uint16_t g_reqFunc = 3; //default start function

static osMutexId_t g_mutex;


//the creation of the mutex and its attributes
void ModbusUiBridge_Init(void)
{
	const osMutexAttr_t attr = { .name = "mb_ui_mutex" };
    g_mutex = osMutexNew(&attr);
    memset(&g_snap, 0, sizeof(g_snap));
}

void ModbusUiBridge_SetRequest(uint16_t functionCode,uint16_t startAddr, uint16_t quantity)
{
    osMutexAcquire(g_mutex, osWaitForever);
    g_reqFunc = functionCode;
    g_reqStart = startAddr;
    g_reqQty   = quantity;
    osMutexRelease(g_mutex);
}

void ModbusUiBridge_TriggerSend(void)
{
    osMutexAcquire(g_mutex, osWaitForever);
    g_sendPending = 1;
    osMutexRelease(g_mutex);
}

int ModbusUiBridge_ConsumeSendTrigger(uint16_t* functionCode ,uint16_t* startAddr, uint16_t* quantity)
{
    int has = 0;
    osMutexAcquire(g_mutex, osWaitForever);
    if (g_sendPending)
    {
        g_sendPending = 0;
        if(functionCode) *functionCode = g_reqFunc;
        if (startAddr) *startAddr = g_reqStart;
        if (quantity)  *quantity  = g_reqQty;
        has = 1;

   }
    osMutexRelease(g_mutex);
    return has;
}

void ModbusUiBridge_Publish(const ModbusUiSnapshotC* s)
{

    osMutexAcquire(g_mutex, osWaitForever);
    g_snap = *s; // struct copy
    osMutexRelease(g_mutex);
}

int ModbusUiBridge_GetSnapshot(ModbusUiSnapshotC* out)
{
    if (!out) return 0;
    osMutexAcquire(g_mutex, osWaitForever);
    *out = g_snap;
    osMutexRelease(g_mutex);
    return 1;
}

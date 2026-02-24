
/*
 * This files serves the purpose of creating a read only screenshot of the modbus status
 * from FreeRTOS without attempting to write to it
 */

#pragma once
#include <cstdint>

#ifndef MB_UI_MAX_REGS
#define MB_UI_MAX_REGS 32   // keep this small for UI; you can raise later
#endif


struct ModbusUiSnapshot
{

	//request state that the user wants
	uint16_t startAddr = 0;
	uint16_t regCount = 1;
	uint16_t qty = 1;
	uint16_t functionCode = 3;

	uint16_t regs[MB_UI_MAX_REGS] = {0};

	uint32_t rttMs;
	uint32_t lastUpdateTick;

	uint8_t result = 0;         //1 = last response ok
	uint8_t exceptionCode = 0;  //handling exception code if any

	//a simple counter to detect changes
	uint32_t seq = 0;
};

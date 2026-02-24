#ifndef MODBUS_MASTER_H
#define MODBUS_MASTER_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32h7xx_hal.h"

typedef enum {
    MB_OK = 0,
    MB_BAD_FRAME,
    MB_TIMEOUT,
    MB_CRC_FAIL,
    MB_EXCEPTION,
    MB_HAL_ERROR
} MB_Status;

uint16_t modbus_crc16(const uint8_t *buf, uint16_t len);

HAL_StatusTypeDef Modbus_SendRaw(const uint8_t *data, uint16_t len, uint32_t timeoutMs);

HAL_StatusTypeDef Modbus_WriteMultipleRegisters(uint8_t slave, uint16_t start_addr, const volatile uint16_t *regs, uint16_t qty, uint32_t tx_timeout, uint32_t rx_timeout, uint8_t *rx8);

MB_Status Modbus_ReadRegisters(uint8_t slave, uint16_t start_addr, uint16_t qty, uint16_t *outregs, uint16_t out_cap, uint16_t *out_count, uint8_t *out_exception_code,  uint32_t timeoutMs);

static inline uint32_t mb_remaining_ms(uint32_t t0, uint32_t timeoutMs);
#endif

#include "modbus_master.h"
#include "usart.h"
#include "gpio.h"
#include <stdint.h>
#include "modbus_app.h"

//---------CRC creation------------
uint16_t modbus_crc16(const uint8_t *buf, uint16_t len)
{
	uint16_t crc=0xFFFF;
	for (uint16_t pos=0; pos < len; pos++)
	{
		crc ^= buf[pos];
		for (int i=0; i < 8; i++)
		{
		crc = (crc & 1) ? (crc >> 1) ^ 0xA001 : (crc >> 1);
		}
	}
 	return crc;
}

//Timing handler
static inline uint32_t mb_remaining_ms(uint32_t t0, uint32_t timeoutMs)
{
	uint32_t t_elapsed = HAL_GetTick() - t0;
	return (t_elapsed >= timeoutMs) ? 0u : (timeoutMs- t_elapsed);
}

//Sending raw data, needs crc to be appended before using

HAL_StatusTypeDef Modbus_SendRaw(const uint8_t *data, uint16_t len, uint32_t timeoutMs)
{
    HAL_StatusTypeDef st = HAL_UART_Transmit(&huart4, (uint8_t*)data, len, timeoutMs);

    // Ensure last stop bit is sent
    while (__HAL_UART_GET_FLAG(&huart4, UART_FLAG_TC) == RESET) { }

    return st;
}
// Reads qty holding registers starting at start_addr.
// out_regs is uint16_t words (host-endian), out_cap is number of uint16_t entries available.
MB_Status Modbus_ReadRegisters(uint8_t slave, uint16_t start_addr,
									   uint16_t qty, uint16_t *outregs,
									   uint16_t out_cap, uint16_t *out_count,
									   uint8_t *out_exception_code,  uint32_t timeoutMs)
{

	if(out_count) *out_count = 0;
	if(out_exception_code) *out_exception_code = 0;


	if(qty==0 || qty > 125) return MB_BAD_FRAME;
	if(outregs == NULL || out_cap < qty) return MB_BAD_FRAME;

	//Building the request
	uint8_t tx[8];
	tx[0] = slave;
	tx[1] = 0x03; //read holding registers
	tx[2] = (uint8_t)(start_addr >> 8); //start address hi
	tx[3] = (uint8_t)(start_addr & 0xFF); //start address lo
	tx[4] = (uint8_t)(qty >> 8); //quantity hi
	tx[5] = (uint8_t)(qty & 0xFF); //quantity lo

	uint16_t crc = modbus_crc16(tx, 6);
	tx[6] = (uint8_t)(crc & 0xFF); //crc lo
	tx[7] = (uint8_t)(crc >> 8); //crc hi

	//----Send----
	HAL_StatusTypeDef hst = Modbus_SendRaw(tx, 8, timeoutMs);
	if(hst != HAL_OK)
	{
		if(hst == HAL_TIMEOUT) return MB_TIMEOUT;
		else return MB_HAL_ERROR;
	}

	//Receiving the response
	//Receive the first two bytes: slave + func
	uint32_t t0 = HAL_GetTick();
	uint8_t first2bytes[2];

	//remaining timeout budget
	uint32_t rem = mb_remaining_ms(t0, timeoutMs);

	hst = HAL_UART_Receive(&huart4, first2bytes, 2, timeoutMs);
	if(hst != HAL_OK)
	{
		if(hst == HAL_TIMEOUT) return MB_TIMEOUT;
		else return MB_HAL_ERROR;
	}


	uint8_t rx_slave = first2bytes[0];
	uint8_t rx_func = first2bytes[1];

	if(rx_slave != slave) return MB_BAD_FRAME;


	//Exception response handling
	if (rx_func ==(uint8_t)(0x03 | 0x80)) //0x83
	{
		uint8_t exbuf[3];
		rem = mb_remaining_ms(t0, timeoutMs);
		if(rem == 0) return MB_TIMEOUT;

		hst = HAL_UART_Receive(&huart4, exbuf, 3, rem);
		if(hst != HAL_OK)
			{
				if(hst == HAL_TIMEOUT) return MB_TIMEOUT;
				else return MB_HAL_ERROR;
			}

		uint8_t exc = exbuf[0];
		uint16_t rx_crc = (uint16_t)exbuf[1] | ((uint16_t)exbuf[2] <<8);

		uint8_t crcbuf[3] = {rx_slave, rx_func, exc};
		uint16_t calc_crc = modbus_crc16(crcbuf, 3);

		if(rx_crc != calc_crc) return MB_CRC_FAIL;

		if(out_exception_code) *out_exception_code = exc;
		return MB_EXCEPTION;
	}

	//Normal FC03 Response must be 0x03
	if(rx_func != 0x03) return MB_BAD_FRAME;


	//receiving the third byte of frame
	uint8_t byteCount;
	rem = mb_remaining_ms(t0, timeoutMs);

	hst = HAL_UART_Receive(&huart4, &byteCount, 1, rem);
	if(hst != HAL_OK)
		{
			if(hst == HAL_TIMEOUT) return MB_TIMEOUT;
			else return MB_HAL_ERROR;
		}

	//FC03 is supposed to return 2 bytes per register
	if(byteCount != (uint8_t)(2u*qty)) return MB_BAD_FRAME;

	//receiving the data (bytecount) and crc(hi+lo)
	//Originally we had the rest of the frame have variable length
	//now we have it hold the max possible frames = 2*125 + 2crc
	uint8_t sizeholder = byteCount + 2;
	uint8_t rest[252];
	if(sizeholder>sizeof(rest)) return MB_BAD_FRAME;

	rem = mb_remaining_ms(t0, timeoutMs);
	if (rem == 0) return MB_TIMEOUT;

	hst = HAL_UART_Receive(&huart4, rest, sizeholder, rem);
	if(hst != HAL_OK)
		{
			if(hst == HAL_TIMEOUT) return MB_TIMEOUT;
			else return MB_HAL_ERROR;
		}

	//building the buffer to check crc
	//[slave][func][qty][bytes (byteCount)]
	//1+1+1+byteCount
	uint8_t crc_buf[3+byteCount];
	crc_buf[0] = rx_slave;
	crc_buf[1] = rx_func;
	crc_buf[2] = byteCount;

	uint8_t i;
	for(i=0; i < byteCount; i++)
	{
		crc_buf[3+i] = rest[i];
	}

	uint16_t rx_crc = (uint16_t)rest[byteCount] | ((uint16_t)rest[byteCount+1] << 8);
	uint16_t calc = modbus_crc16(crc_buf,(uint16_t)(3+byteCount));

	if(rx_crc != calc) return MB_CRC_FAIL;

	//unpacking received information into register storage
	for(uint16_t i=0; i<qty; i++)
	{
		uint8_t hi = rest[2u*i];
		uint8_t lo = rest[2u*i +1u];
		outregs[i] = ((uint16_t)hi<<8) | (uint16_t)lo;
	}

	if(out_count) *out_count = qty;
	return MB_OK;
}

HAL_StatusTypeDef Modbus_WriteMultipleRegisters(uint8_t slave,
													   uint16_t start_addr,
													   const volatile uint16_t *regs,
													   uint16_t qty,
													   uint32_t tx_timeout,
													   uint32_t rx_timeout,
													   uint8_t *rx8)
{
	 // Request: [slave][0x10][addrHi][addrLo][qtyHi][qtyLo][byteCount][data...][CRCLo][CRCHi]
	 // Response: 8 bytes: [slave][0x10][addrHi][addrLo][qtyHi][qtyLo][CRCLo][CRCHi]

	uint8_t tx[1 + 1 + 2 + 2 + 1 + (2 * 16) + 2]; // supports up to 16 regs safely; adjust if needed
	    if (qty == 0 || qty > 16) return HAL_ERROR;


	    uint16_t idx = 0;
	    tx[idx++] = slave;
	    tx[idx++] = 0x10;
	    tx[idx++] = (uint8_t)(start_addr >> 8);
	    tx[idx++] = (uint8_t)(start_addr & 0xFF);
	    tx[idx++] = (uint8_t)(qty >> 8);
	    tx[idx++] = (uint8_t)(qty & 0xFF);
	    tx[idx++] = (uint8_t)(qty * 2);

	    for (uint16_t i = 0; i < qty; i++) {
	         tx[idx++] = (uint8_t)(regs[i] >> 8);      // Hi
	         tx[idx++] = (uint8_t)(regs[i] & 0xFF);    // Lo
	     }

	     uint16_t crc = modbus_crc16(tx, idx);
	     tx[idx++] = (uint8_t)(crc & 0xFF);           // CRCLo (Modbus RTU order)
	     tx[idx++] = (uint8_t)((crc >> 8) & 0xFF);    // CRCHi

	   // Send
	   HAL_StatusTypeDef st_tx = Modbus_SendRaw(tx, idx, tx_timeout);
	   if (st_tx != HAL_OK) return st_tx;

	   // Receive 8-byte acknowledgement
	   memset(rx8, 0, 8);
	   HAL_StatusTypeDef st_rx = HAL_UART_Receive(&huart4, rx8, 8, rx_timeout);
	   if (st_rx != HAL_OK) return st_rx;

	   // Validate CRC
	   uint16_t crc_calc = modbus_crc16(rx8, 6);
	   uint16_t crc_rx   = (uint16_t)rx8[6] | ((uint16_t)rx8[7] << 8);
	   if (crc_calc != crc_rx) return HAL_ERROR;

	   // Validate echo fields
	   if (rx8[0] != slave || rx8[1] != 0x10) return HAL_ERROR;
	   if (rx8[2] != (uint8_t)(start_addr >> 8) || rx8[3] != (uint8_t)(start_addr & 0xFF)) return HAL_ERROR;
	   if (rx8[4] != (uint8_t)(qty >> 8) || rx8[5] != (uint8_t)(qty & 0xFF)) return HAL_ERROR;

	  return HAL_OK;


}

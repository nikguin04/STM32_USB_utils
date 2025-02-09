/*
 * st7789_io.c
 *
 *  Created on: Feb 7, 2025
 *      Author: nikla
 */

#include "st7789v_io.h"

/*typedef int32_t (*ST7789V_Write_Func)(void *, volatile uint8_t*, uint32_t);
typedef int32_t (*ST7789V_Read_Func) (void *, volatile uint8_t*, uint32_t);
typedef int32_t (*ST7789V_Send_Func) (void *, uint8_t*, uint32_t);
typedef int32_t (*ST7789V_Recv_Func) (void *, uint8_t*, uint32_t);

typedef struct
{
  ST7789V_Init_Func          Init;
  ST7789V_DeInit_Func        DeInit;
  uint16_t                   Address;
  ST7789V_WriteReg_Func      WriteReg;
  ST7789V_ReadReg_Func       ReadReg;
  ST7789V_SendData_Func      SendData;
  ST7789V_SendData_Func      SendDataDMA;
  ST7789V_RecvData_Func      RecvData;
  ST7789V_RecvData_Func      RecvDataDMA;
  ST7789V_GetTick_Func       GetTick;
  ST7789V_Delay_Func         Delay;
} ST7789V_IO_t;*/


ST7789V_Init_Func Init() {
	return 0;
};

ST7789V_WriteReg_Func WriteReg(volatile uint8_t* reg, uint32_t length) { // See doc 8.4.2 Command write mode
	HAL_GPIO_WritePin(ST7789V_DC_GPIO_Port, ST7789V_DC_Pin, 0); // Command D/CX indicates whether the byte is command (D/CX=’0’) or parameter/RAM data (D/CX=’1’).
	HAL_GPIO_WritePin(ST7789V_CS_GPIO_Port, ST7789V_CS_Pin, 0); // CS LOW = Select display
	HAL_SPI_Transmit(&hspi1, reg, length, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(ST7789V_CS_GPIO_Port, ST7789V_CS_Pin, 1);   // CS HIGH = Deselect display
}

ST7789V_SendData_Func SendData(uint8_t* buf, uint32_t length) {
	HAL_GPIO_WritePin(ST7789V_DC_GPIO_Port, ST7789V_DC_Pin, 1); // Command D/CX indicates whether the byte is command (D/CX=’0’) or parameter/RAM data (D/CX=’1’).
	HAL_GPIO_WritePin(ST7789V_CS_GPIO_Port, ST7789V_CS_Pin, 0); // CS LOW = Select display
	HAL_SPI_Transmit(&hspi1, buf, length, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(ST7789V_CS_GPIO_Port, ST7789V_CS_Pin, 1);   // CS HIGH = Deselect display
}

ST7789V_GetTick_Func Tick() {
	HAL_GetTick();
}

ST7789V_Delay_Func Delay(uint32_t ms) { // I think time is supposed to be ms
	HAL_Delay(ms);
}


ST7789V_IO_t ST7789V_IO = {
	Init,
	NULL,
	0x1,
	WriteReg,
	NULL,
	SendData,
	NULL,
	NULL,
	NULL,
	Tick,
	Delay,
};

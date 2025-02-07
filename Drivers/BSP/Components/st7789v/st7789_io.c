/*
 * st7789_io.c
 *
 *  Created on: Feb 7, 2025
 *      Author: nikla
 */



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


ST7789V_Init_Func Init() { return 0; }

ST7789V_WriteReg_Func WriteReg(volatile uint8_t* reg, uint32_t length) { // See doc 8.2.1
	HAL_GPIO_WritePin(ST7789_DC_GPIO_Port, ST7789_DC_Pin, GPIO_PIN_RESET); // DC LOW = Command
	HAL_GPIO_WritePin(ST7789_CS_GPIO_Port, ST7789_CS_Pin, GPIO_PIN_RESET); // CS LOW = Select display
	HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(ST7789_CS_GPIO_Port, ST7789_CS_Pin, GPIO_PIN_SET);   // CS HIGH = Deselect display
}

ST7789V_IO_t ST7789V_IO = {
		Init,
		NULL,
		0x1,
		WriteReg

};

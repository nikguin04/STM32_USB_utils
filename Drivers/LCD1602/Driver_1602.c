/*
 * Driver_1602.c
 *
 *  Created on: Feb 12, 2025
 *      Author: nikla
 */

// Credit to https://controllerstech.com/interface-lcd-16x2-with-stm32-without-i2c/

#include "Driver_1602.h"

/*#define timer htim1
extern TIM_HandleTypeDef timer;
void delay (uint16_t us)
{
  __HAL_TIM_SET_COUNTER(&timer, 0);
  while (__HAL_TIM_GET_COUNTER(&timer) < us);
}*/

void send_to_lcd (char data, int rs)
{
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, rs);  // rs = 1 for data, rs=0 for command
    /* write the data to the respective pin */
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, ((data>>3)&0x01));
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, ((data>>2)&0x01));
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, ((data>>1)&0x01));
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, ((data>>0)&0x01));
    /* Toggle EN PIN to send the data
     * if the HCLK > 100 MHz, use the  20 us delay
     * if the LCD still doesn't work, increase the delay to 50, 80 or 100..
     */
    HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, 1);
//  delay (20);
    HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, 0);
//  delay (20);
}

void lcd_send_cmd (char cmd)
{
    char datatosend;
    /* send upper nibble first */
    datatosend = ((cmd>>4)&0x0f);
    send_to_lcd(datatosend,0);  // RS must be while sending command
    /* send Lower Nibble */
    datatosend = ((cmd)&0x0f);
    send_to_lcd(datatosend, 0);
}

void lcd_send_data (char data)
{
    char datatosend;

    /* send higher nibble */
    datatosend = ((data>>4)&0x0f);
    send_to_lcd(datatosend, 1);  // rs =1 for sending data
    /* send Lower nibble */
    datatosend = ((data)&0x0f);
    send_to_lcd(datatosend, 1);
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }
    lcd_send_cmd (col);
}

void lcd_put_str(char* buf, int len) {
	for (int i = 0; i < len && buf[i] != NULL; i++) {
		lcd_send_data(buf[i]);
		DELAY_US(40); // Needs 37 nanoseconds to flush "write data to ram"
	}
}

void lcd_put_str_at(char* buf, int len, int row, int col) {
	lcd_put_cur(row, col);
	HAL_Delay(1);
	lcd_put_str(buf, len);
}

void lcd_put_custom_char(uint8_t* charbuffer, uint8_t cgram_ptr) {
	// For the 5x? character setup, the 3 most significant bits for each uint8_t in charbuffer are ignored

	cgram_ptr &= 0b00000111;

	// Write new 0b000 CGRAM address
	lcd_send_cmd(cgram_ptr | 0b01000000);
	DELAY_US(40);
	for (int i = 0; i < 8; i++) {
		lcd_send_data(charbuffer[i]);
		DELAY_US(40);
	}

}


void lcd_clear (void) {
	lcd_send_cmd (0x01);
}

void lcd_init (void)
{
    // 4 bit initialisation
    HAL_Delay(50);  // wait for >40ms
    lcd_send_cmd (0x30);
    HAL_Delay(5);  // wait for >4.1ms
    lcd_send_cmd (0x30);
    HAL_Delay(1);  // wait for >100us
    lcd_send_cmd (0x30);
    HAL_Delay(10);
    lcd_send_cmd (0x20);  // 4bit mode
    HAL_Delay(10);

  // dislay initialisation
    lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
    HAL_Delay(1);
    lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
    HAL_Delay(1);

    lcd_send_cmd (0x01);  // clear display
    HAL_Delay(2);
    lcd_send_cmd (0b00000110); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
    lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
    HAL_Delay(1);
    lcd_send_data(0x00); // CLEAR BUFFER
    HAL_Delay(1);
    lcd_send_cmd (0x01);
    HAL_Delay(2);
}

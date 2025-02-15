/*
 * Driver_1602.h
 *
 *  Created on: Feb 12, 2025
 *      Author: nikla
 */

#ifndef LCD1602_DRIVER_1602_H_
#define LCD1602_DRIVER_1602_H_

#include <stdint.h>
#include "main.h"

void lcd_send_cmd (char cmd);
void lcd_send_data (char data);
void lcd_clear (void);
void lcd_init (void);
void lcd_put_str(char* buf, int len);
void lcd_put_str_at(char* buf, int len, int row, int col);
void lcd_put_custom_char(uint8_t* charbuffer, uint8_t cgram_ptr);


static uint8_t customchar_binarycount[8] =
{
		0b00000001,
		0b00000010,
		0b00000011,
		0b00000100,
		0b00000101,
		0b00000110,
		0b00000111,
		0b00001000,
};


#endif /* LCD1602_DRIVER_1602_H_ */

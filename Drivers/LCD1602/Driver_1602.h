/*
 * Driver_1602.h
 *
 *  Created on: Feb 12, 2025
 *      Author: nikla
 */

#ifndef LCD1602_DRIVER_1602_H_
#define LCD1602_DRIVER_1602_H_

void lcd_send_cmd (char cmd);
void lcd_send_data (char data);
void lcd_clear (void);
void lcd_init (void);
void lcd_put_str(char* buf, int len);
void lcd_put_str_at(char* buf, int len, int row, int col);

#define SYSTICK_LOAD (SystemCoreClock/1000000U)
#define SYSTICK_DELAY_CALIB (SYSTICK_LOAD >> 1)

#define DELAY_US(us) \
    do { \
         uint32_t start = SysTick->VAL; \
         uint32_t ticks = (us * SYSTICK_LOAD)-SYSTICK_DELAY_CALIB;  \
         while((start - SysTick->VAL) < ticks); \
    } while (0)


#endif /* LCD1602_DRIVER_1602_H_ */

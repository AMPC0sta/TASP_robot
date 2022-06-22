/*
 * i2cdisplay.h
 *
 *  Created on: Apr 12,(geti2ctxcplt() == false); 2022
 *      Author: Rui Gomes
 */

#ifndef INC_I2CDISPLAY_H_
#define INC_I2CDISPLAY_H_

#include <stdbool.h>

void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char);  // send command to the lcd

void lcd_write_pos (int, int, void*); // write in the specified position

void lcd_write_pos_1602 (int, int, void*);

void lcd_send_data (char);  // send data to the lcd

void lcd_send_string (char *);  // send string to the lcd

void lcd_put_cur(int, int);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_put_cur_1602 (int, int);

void lcd_clear (void);


#endif /* INC_I2CDISPLAY_H_ */

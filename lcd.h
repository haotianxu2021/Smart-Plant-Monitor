#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>

#define LCD_RS PD0
#define LCD_EN PD1
#define LCD_DB4 PD2
#define LCD_DB5 PD3
#define LCD_DB6 PD4
#define LCD_DB7 PD5

#define LCD_PORT PORTD
#define LCD_DDR DDRD

#define LCD_DISP_ON 0x0C
#define LCD_DISP_OFF 0x08

void lcd_init(uint8_t disp_attr);
void lcd_clrscr(void);
void lcd_home(void);
void lcd_goto(uint8_t pos);
void lcd_putc(char c);
void lcd_puts(const char *s);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);

#endif
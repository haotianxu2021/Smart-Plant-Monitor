#include "lcd.h"

static void lcd_nibble(uint8_t nibble) {
    LCD_PORT &= ~((1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7));
    LCD_PORT |= (nibble & 0x0F) << LCD_DB4;

    LCD_PORT |= (1 << LCD_EN);
    _delay_us(1);
    LCD_PORT &= ~(1 << LCD_EN);
    _delay_us(1);
}

static void lcd_byte(uint8_t byte) {
    lcd_nibble(byte >> 4);
    lcd_nibble(byte);
    _delay_us(40);
}

void lcd_command(uint8_t cmd) {
    LCD_PORT &= ~(1 << LCD_RS);
    lcd_byte(cmd);
}

void lcd_data(uint8_t data) {
    LCD_PORT |= (1 << LCD_RS);
    lcd_byte(data);
}

void lcd_init(uint8_t disp_attr) {
    LCD_DDR |= (1 << LCD_RS) | (1 << LCD_EN) | (1 << LCD_DB4) | (1 << LCD_DB5) | (1 << LCD_DB6) | (1 << LCD_DB7);
    _delay_ms(50);

    LCD_PORT &= ~(1 << LCD_RS);
    lcd_nibble(0x03);
    _delay_ms(5);
    lcd_nibble(0x03);
    _delay_us(110);
    lcd_nibble(0x03);
    _delay_us(40);
    lcd_nibble(0x02);

    lcd_command(0x28);
    lcd_command(LCD_DISP_OFF);
    lcd_clrscr();
    lcd_command(0x06);
    lcd_command(disp_attr);
}

void lcd_clrscr(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_home(void) {
    lcd_command(0x02);
    _delay_ms(2);
}

void lcd_goto(uint8_t pos) {
    lcd_command(0x80 | (pos & 0x7F));
}

void lcd_putc(char c) {
    lcd_data(c);
}

void lcd_puts(const char *s) {
    while (*s) {
        lcd_data(*s++);
    }
}

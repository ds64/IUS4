#ifndef _LCD_H_
#define _LCD_H_

#define DATA_IND_ADDR 0x080001

#define C_IND_ADDR 0x080006

void lcd_clear();
void lcd_creturn();
void lcd_putchar(const char c);
void lcd_movcur(const unsigned char x, const unsigned char y);
unsigned char lcd_acstate();

void lcd_display(unsigned char buf[][]);

char * iconv4lcd(char * str);

#endif

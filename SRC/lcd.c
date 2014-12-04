#include "lcd.h"
#include "max.h"
/**
 * Значение BF
 *
 * Функция возвращает значение флага занятости - BF
 */
unsigned char lcd_bfstate()
{
    unsigned char retval = 0x00;

    write_max(C_IND_ADDR, 0x03); /* RS=0 RW=1 E=1 */
    retval = read_max(DATA_IND_ADDR);
    write_max(C_IND_ADDR, 0x02); /* RS=0 RW=1 E=0 */
    retval = retval >> 7;

    return retval;
}

/**
 * Значение AC
 *
 * Функция возвращает значение счетчика адреса для DDRAM
 */
unsigned char lcd_acstate()
{
    unsigned char retval = 0x00;

    write_max(C_IND_ADDR, 0x03); /* RS=0 RW=1 E=1 */
    retval = read_max(DATA_IND_ADDR);
    write_max(C_IND_ADDR, 0x02); /* RS=0 RW=1 E=0 */
    retval = retval & 0x7f;

    return retval;
}

/**
 * Установка адреса в DDRAM
 */
void lcd_set_ddram_addr(const unsigned char addr)
{
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, addr | 0x80);
    write_max(C_IND_ADDR, 0x01); /* RS=0 RW=0 E=1 */
    write_max(C_IND_ADDR, 0x00);/* RS=0 RW=0 E=0 */
    return;
}

/**
 * Функция для очистки экрана ЖКИ
 *
 * В данной функции происходит очистка экрана и
 * возврат курсора в начало строки.
 */
void lcd_clear()
{
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);

    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x06);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);

    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x0f);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
    return;
}

/**
 * Возврат курсора
 *
 * Возврат курсора в левый верхний угол экрана (координаты
 * X: 0 и Y: 0)
 */
void lcd_creturn()
{
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, 0x02);
    write_max(C_IND_ADDR, 0x01);
    write_max(C_IND_ADDR, 0x00);
    return;
}

/**
 * Вывод символа на дисплей
 */
void lcd_putchar(const char c)
{
    unsigned char ddram_addr = lcd_acstate();

    if ((ddram_addr > 0x0f) && (ddram_addr < 0x40))
    {
        lcd_set_ddram_addr(0x40);
    }
    while(lcd_bfstate()) {}
    write_max(DATA_IND_ADDR, c);
    write_max(C_IND_ADDR, 0x05);
    write_max(C_IND_ADDR, 0x04);
    return;
}


/**
 * Переход в заданную позицию дисплея
 *
 * Функция для перевода курсора в заданные координаты.
 * Если они некорректны, то ничего не происходит.
 */
void lcd_movcur(const unsigned char x,
        const unsigned char y)
{
    if ((x > 15) || (y > 1))
    {
        return;
    }
    lcd_set_ddram_addr(x + 0x40 * y);

    return;
}

void lcd_display(unsigned char buf[][])
{
    unsigned char i, j;

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 16; j++)
        {
            lcd_set_ddram_addr(j + 0x40 * i);
            while(lcd_bfstate()) {}
            write_max(DATA_IND_ADDR, buf[i][j]);
            write_max(C_IND_ADDR, 0x05);
            write_max(C_IND_ADDR, 0x04);
        }
    }
}

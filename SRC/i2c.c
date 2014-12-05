#include "aduc812.h"
#include "i2c.h"

static void delay()
{
    char c = 0;
    while(c++ < 2);
}

static char send_byte(unsigned char c)
{
    char i;
    char ack;

    MDE = 1;
    MCO = 0;

    for(i = 0; i < 8; i++, c << 1)
    {
        MDO = (c & 0x80) ? 1 : 0;
        MCO = 1;

        MCO = 0;
    }

    MDE = 0;
    MCO = 1;

    ack = MDI;
    MCO = 0;
    return ack;
}

static void start()
{
    MDE = 1;
    MDO = 1;
    MCO = 1;

    MDO = 0;

    MCO = 0;
}

static void stop()
{
    MDE = 1;
    MCO = 0;
    MDO = 0;
    MCO = 1;

    MDO = 1;

    MDE = 0;
}

static char begin(unsigned char addr)
{
    start();
    return send_byte(addr);
}

static void ack()
{
    MDE = 1;
    MCO = 0;
    MDO = 0;
    MCO = 1;

    MCO = 0;
}

static void nack()
{
    MDE = 1;
    MCO = 0;
    MDO = 1;
    MCO = 1;

    MCO = 0;
}

unsigned char receive_byte()
{
    char i;
    unsigned char c = 0;

    MDE = 0;
    MCO = 0;

    for(i = 0; i < 8; i++)
    {
        c << 1;
        MCO = 1;

        c |= MDI;
        MCO = 0;
    }

    return c;
}

char get_ack(unsigned char addr)
{
    I2CM = 1;

    if(begin(addr & 0xFE))
    {
        stop();
        return 0;
    }

    stop();
    return 1;
}

char receive_block(unsigned char eeprom_addr, unsigned char addr,
                   unsigned char * block, unsigned short size)
{
    unsigned char i, c;

    I2CM = 1;
    eeprom_addr = eeprom_addr & 0xFE;

    if(begin(eeprom_addr))
    {
        stop();
        return 1;
    }

    if(send_byte(addr))
    {
        stop();
        return 1;
    }

    eeprom_addr |= 1;

    if(begin(eeprom_addr))
    {
        stop();
        return 1;
    }

    if(size - 1)
    {
        for(i = 0; i < (size-1); i++)
        {
            c = receive_byte();
            ack();
            *block++ = c;
        }
    }

    c = receive_byte();
    ack();
    *block++ = c;

    stop();

    return 0;
}

char transmit_block(unsigned char eeprom_addr, unsigned char addr,
                    unsigned char * block, unsigned short size)
{
    unsigned char c, i;

    I2CM = 1;

    eeprom_addr = eeprom_addr & 0xFE;

    if(begin(eeprom_addr))
    {
        stop();
        return 1;
    }

    if(send_byte(addr))
    {
        stop();
        return 1;
    }

    for(i = 0; i < size; i++)
    {
        c = *block;
        if(send_byte(c))
        {
            stop();
            return 1;
        }

        block++;
    }

    stop();
    return 0;
}

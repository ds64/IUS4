#include "i2c.h"
#include "eeprom.h"

unsigned char ReadEEPROM(unsigned long addr, unsigned long size,
                         unsigned char * buf)
{
    unsigned short s = 0;

    if(addr+size > EEPROM_SIZE)
        return 1;

    while(!get_ack(EEPROM_ADDRESS))
    {
        if(++s > 5000)
            return 1;
    }

    if(receive_block(EEPROM_ADDRESS, addr, buf, size) == 1)
        return 1;

    return 0;
}

unsigned char WriteEEPROM(unsigned long addr, unsigned long size,
                          unsigned char * buf)
{
    unsigned short s;
    unsigned short pages, i, r;

    if(addr + size > EEPROM_SIZE)
        return 1;

    pages = size >> 3;
    r = size & 0x7;

    for(i = 0; i < pages; ++i)
    {
        s = 0;

        while(!get_ack(EEPROM_ADDRESS))
        {
            if(++s > 5000)
                return 1;
        }

        if(transmit_block(EEPROM_ADDRESS, addr + (i << 3), buf + (i << 3), 8))
            return 1;
    }

    if(r)
    {
        s = 0;
        while(!get_ack(EEPROM_ADDRESS))
        {
            if(++s > 5000)
            {
                return 1;
            }
        }

        if(transmit_block(EEPROM_ADDRESS, addr + (i << 3), buf + (i << 3), r))
            return 1;
    }

    return 0;
}

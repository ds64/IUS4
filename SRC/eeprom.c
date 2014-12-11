#include "eeprom.h"
#include "i2c.h"

bit ReadEEPROM( unsigned short address, unsigned char xdata *buf, unsigned short length )
{
		unsigned short try;

    if((address+length)>EEPROM_SIZE) return 1;

    try = 0;

    while( !get_ack( EEPROM_ADDRESS ) )
    {
        if(++try > 5000)
            return 1;
    }

    if( receive_block( EEPROM_ADDRESS, address, buf, length) ) return 1;

    return 0;
}

bit WriteEEPROM( unsigned short address, unsigned char xdata *buf, unsigned short length )
{
		unsigned short try;
		unsigned short pages, i, remainder;

    if( ( address + length ) > EEPROM_SIZE ) return 1;

    pages     = length >> 3;
    remainder = length &  0x7;

    for( i = 0; i < pages; ++i )
    {
        try = 0;

        while( !get_ack( EEPROM_ADDRESS ) )
        {
            if( ++try > 5000)
                return 1;
        }

        if( transmit_block(EEPROM_ADDRESS, address + (i << 3), buf + (i << 3), 8) ) return 1;

    }

    if( remainder )
    {
        try = 0;
        while( !get_ack(EEPROM_ADDRESS) )
        {
            if(++try > 5000)
            {
                return 1;
            }
        }

        if( transmit_block( EEPROM_ADDRESS, address + (i << 3), buf + (i << 3), remainder ) ) return 1;
    }

    return 0;
}

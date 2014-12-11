#define EEPROM_16	// ���� EEPROM ����� 16-�� ��������� �������� ������������

#include "eeprom.h" 
#include "i2c.h"    

// ReadEEPROM - ������ ����� ������ �� EEPROM
// ����: address - ����� ������ EEPROM, � ������� ����� �������� ����
//		 buf - ��������� �� ���� � ������� xdata   
//		 length - ����� �����
// ���������: 1 - ������ ������ �������
//			  0 - EEPROM �� ��������, ���� ����� ����� ������ EEPROM_SIZE	
bit ReadEEPROM( unsigned short address, unsigned char xdata *buf, unsigned short length )
{
	unsigned short try;

    if((address+length)>EEPROM_SIZE) return 1; //����� ����� ������ EEPROM_SIZE

    try = 0;

    while( !get_ack( EEPROM_ADDRESS ) )
    {
        if(++try > 5000) // >10 ��
            return 1; //EEPROM failed to respond
    }
    
     
#ifdef EEPROM_16
    if( receive_block16( EEPROM_ADDRESS, address, buf, length) ) return 1; //Error reading
#else
    if( receive_block( EEPROM_ADDRESS, address, buf, length) ) return 1; //Error reading
#endif

    return 0;
}


// WriteEEPROM - ������ ����� ������ � EEPROM
// ����: address - ����� ������ EEPROM, � ������� ����� ��������� ����
//		 buf - ��������� �� ���� � ������� xdata   
//		 length - ����� �����
// ���������: 1 - ������ ������ �������
//			  0 - EEPROM �� ��������, ���� ����� ����� ������ EEPROM_SIZE	
bit WriteEEPROM( unsigned short address, unsigned char xdata *buf, unsigned short length ) 
{
	unsigned short try;
	unsigned short pages, i, remainder;

    if( ( address + length ) > EEPROM_SIZE ) return 1;

    // ������������ ����, ���� �� ������ 8 ����, ����������� �� �������� �� 8 ����
	// ������� ��������������� ���������� EEPROM ��� ������
    pages     = length >> 3;    // length / 8
    remainder = length &  0x7;  // length % 8

    for( i = 0; i < pages; ++i )
    {
        try = 0;

        while( !get_ack( EEPROM_ADDRESS ) )
        {
            if( ++try > 5000)
                return 1; //EEPROM failed to respond
        }
        
#ifdef EEPROM_16
        if( transmit_block16(EEPROM_ADDRESS, address + (i << 3), buf + (i << 3), 8) ) return 1; //Error writing
#else
        if( transmit_block(EEPROM_ADDRESS, address + (i << 3), buf + (i << 3), 8) ) return 1; //Error writing
#endif

    }

    if( remainder )
    {
        try = 0;
        while( !get_ack(EEPROM_ADDRESS) )
        {
            if(++try > 5000)
            {
                return 1; //EEPROM failed to respond
            }
        }

        
#ifdef EEPROM_16
        if( transmit_block16( EEPROM_ADDRESS, address + (i << 3), buf + (i << 3), remainder ) ) return 1;
#else
        if( transmit_block( EEPROM_ADDRESS, address + (i << 3), buf + (i << 3), remainder ) ) return 1;
#endif
                
    }

    return 0;
}

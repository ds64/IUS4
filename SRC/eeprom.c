#define EEPROM_16	// если EEPROM имеет 16-ти разрядное адресное пространство

#include "eeprom.h" 
#include "i2c.h"    

// ReadEEPROM - чтение блока данных из EEPROM
// Вход: address - адрес ячейки EEPROM, с которой нужно прочесть блок
//		 buf - указатель на блок в области xdata   
//		 length - длина блока
// Результат: 1 - чтение прошло успешно
//			  0 - EEPROM не отвечает, либо длина блока больше EEPROM_SIZE	
bit ReadEEPROM( unsigned short address, unsigned char xdata *buf, unsigned short length )
{
	unsigned short try;

    if((address+length)>EEPROM_SIZE) return 1; //длина блока больше EEPROM_SIZE

    try = 0;

    while( !get_ack( EEPROM_ADDRESS ) )
    {
        if(++try > 5000) // >10 мс
            return 1; //EEPROM failed to respond
    }
    
     
#ifdef EEPROM_16
    if( receive_block16( EEPROM_ADDRESS, address, buf, length) ) return 1; //Error reading
#else
    if( receive_block( EEPROM_ADDRESS, address, buf, length) ) return 1; //Error reading
#endif

    return 0;
}


// WriteEEPROM - запись блока данных в EEPROM
// Вход: address - адрес ячейки EEPROM, с которой нужно поместить блок
//		 buf - указатель на блок в области xdata   
//		 length - длина блока
// Результат: 1 - запись прошла успешно
//			  0 - EEPROM не отвечает, либо длина блока больше EEPROM_SIZE	
bit WriteEEPROM( unsigned short address, unsigned char xdata *buf, unsigned short length ) 
{
	unsigned short try;
	unsigned short pages, i, remainder;

    if( ( address + length ) > EEPROM_SIZE ) return 1;

    // Записываемый блок, если он больше 8 байт, разбивается на страницы по 8 байт
	// которые последовательно передаются EEPROM для записи
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

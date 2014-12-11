#ifndef __EEPROM__H
#define __EEPROM__H

#define EEPROM_ADDRESS  0xA2
#define EEPROM_SIZE     32768

/*
	„тение данных из EEPROM с возвратом результата выполнени€ операции:
		addr Ц адрес €чейки пам€ти,
		size Ц размер буфера дл€ чтени€, 
		buf Ц буфер.
*/
extern bit ReadEEPROM(unsigned short address, unsigned char xdata *buf, unsigned short length);

/*
	«апись данных в EEPROM с возвратом результата выполнени€ операции:
		addr Ц адрес €чейки пам€ти,
		size Ц размер буфера записи, 
		buf Ц буфер.
*/
extern bit WriteEEPROM( unsigned short address, unsigned char xdata *buf, unsigned short length );

#endif

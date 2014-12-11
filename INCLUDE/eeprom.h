#ifndef __EEPROM__H
#define __EEPROM__H

#define EEPROM_ADDRESS  0xA2
#define EEPROM_SIZE     32768

/*
	������ ������ �� EEPROM � ��������� ���������� ���������� ��������:
		addr � ����� ������ ������,
		size � ������ ������ ��� ������, 
		buf � �����.
*/
extern bit ReadEEPROM(unsigned short address, unsigned char xdata *buf, unsigned short length);

/*
	������ ������ � EEPROM � ��������� ���������� ���������� ��������:
		addr � ����� ������ ������,
		size � ������ ������ ������, 
		buf � �����.
*/
extern bit WriteEEPROM( unsigned short address, unsigned char xdata *buf, unsigned short length );

#endif

#ifndef _EEPROM_H_
#define _EEPROM_H_
#include "aduc812.h"

unsigned char ReadEEPROM(unsigned long addr, unsigned long size,
                         unsigned char * buf);
unsigned char WriteEEPROM(unsigned long addr, unsigned long size,
                          unsigned char * buf);

#endif _EEPROM_H_

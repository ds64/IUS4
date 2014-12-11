#ifndef _MAX_H_
#define _MAX_H_

void WriteMax(unsigned char xdata *regnum, unsigned char val);
unsigned char ReadMax(unsigned char xdata *regnum);
void BEEP();

#define KB          0x0
#define DATA_IND    0x1
#define EXT_LO      0x2
#define EXT_HI      0x3
#define ENA         0x4
#define C_IND       0x6
#define LEDLINE     0x7

void write_max( unsigned char xdata *regnum, unsigned char val );
unsigned char read_max( unsigned char xdata *regnum );

#endif

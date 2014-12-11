#include "aduc812.h"
#include "keys.h"
#include "asio.h"
#include "buf.h"
#include "max.h"
#include "lcd.h"
#include "eeprom.h"
#include <string.h>

#define SERIAL_SPEED 0xFD
#define BUF_ROWS    2
#define SCREEN_ROWS 2
#define COLS        16
// -------------------
unsigned int a = 0, b = 0;
unsigned int n = 0;
xdata unsigned char buf[2][16];
char x = 0, y = 0;
char lcdDisplayBuffer_row = 0;
unsigned char dips[8];
unsigned char xdata anim1[8];
unsigned char xdata anim2[4];

unsigned char A, N1, D, S1, N2, S2;

unsigned char crc = 0xFF;

unsigned char rotl(unsigned char value, char shift)
{
  return (value << shift) | (value >> (8 - shift));
}

unsigned char rotr(unsigned char value, char shift)
{
  return (value >> shift) | (value << (8 - shift));
}

void SET_LED(unsigned char value)
{
    WriteMax(7, value);
}

unsigned char READ_DIP()
{
    unsigned char val = ReadMax(EXT_LO);
    return val;
}

unsigned char CHECK_DIP()
{
    return READ_DIP() & 0x01;
}

void cleanBuffer()
{
    int i, j;
    for(i = 0; i < BUF_ROWS; i++)
    {
        for(j = 0; j < COLS; j++)
        {
            buf[i][j] = ' ';
        }
    }
}

void lcdDisplayBuffer()
{
    char i, j;
    EA = 0;

    if(y < lcdDisplayBuffer_row)
    {
        lcdDisplayBuffer_row--;
    }

    if(y > (lcdDisplayBuffer_row + SCREEN_ROWS - 1))
    {
        lcdDisplayBuffer_row++;
    }

    for(i = 0; i < COLS; i++)
    {
        for(j = 0; j < SCREEN_ROWS; j++)
        {
            lcd_movcur(i, j);
            lcd_putchar(buf[lcdDisplayBuffer_row + j][i]);
        }
    }

    lcd_movcur(x, y - lcdDisplayBuffer_row);

    EA = 1;
}

void initAnim1()
{
  char i = 0;
  unsigned char def = 0;
  for(i = 0; i < N1; i++)
  {
    def = (def << 1) | 0x01;
  }
  anim1[0] = rotl(def,2);
  for(i = 1; i < 8; i++)
  {
    if(D == 0)
      anim1[i] = rotl(anim1[i-1],1);
    else
      anim1[i] = rotr(anim1[i-1],1);
  }
}

void initAnim2()
{
  char i = 0;
  char def1 = 0;
  char def2 = 0;
  for(i = 0; i < N2; i++)
  {
    def1 = (def1 << 1) | 0x01;
    def2 = (def2 >> 1) | 0x80;
  }

  anim2[0] = def1 | def2;
  for(i = 1; i < 4; i++)
  {
    anim2[i] = rotl(anim2[i-1],1) | rotr(anim2[i-1],1);
  }
}

void displayLEDAnim1() {
    int i = 0;
    unsigned int oldTimer = -1;
    unsigned char tmp;

    initAnim1();

    while (1) {
        if ((timer % S1 == 0) && (oldTimer != timer)) {
            SET_LED(anim1[i]);
            i++;
            oldTimer = timer;
        }
        if (i > 7) break;
    }
}

void displayLEDAnim2() {
    int i = 0;
    unsigned int oldTimer = -1;
    unsigned char tmp;

    initAnim2();

    while (1) {
        if ((timer % S2 == 0) && (oldTimer != timer)) {
            //tmp = stab(anim1[i], N);
            SET_LED(anim2[i]);
            i++;
            oldTimer = timer;
        }
        if (i > (4-N2)) break;
    }
    i = 0;
    while (1) {
      if ((timer % S2 == 0) && (oldTimer != timer)) {
        //tmp = stab(anim1[i], N);
        SET_LED(anim2[4-N2-i]);
        i++;
        oldTimer = timer;
      }
      if (i > (4-N2)) break;
    }
}


void runAnim() {
	char uCh = 0;
	char kCh = 0;

	while(1) {
		kCh = READ_BUFFER();
        uCh = ASIO_READ();
        if ((kCh == 0) && (uCh == 0)) {
        	if (!A) displayLEDAnim1();
        	else displayLEDAnim2();
        }
        else break;
	}

}

void showParams() {
	lcd_clear();
    cleanBuffer();

    if(A == 0)
    {
      strcpy(buf[0], "A   N   D   S");
      buf[1][0] = A + '0';
      buf[1][3] = N1 + '0';
      buf[1][7] = D + '0';
      buf[1][10] = S1 + '0';
    }
    else
    {
      strcpy(buf[0], "A   N   S");
      buf[1][0] = A + '0';
      buf[1][3] = N2 + '0';
      buf[1][7] = S2 + '0';
    }

    lcdDisplayBuffer();
}

unsigned char xdata CRC8(unsigned char xdata *pMas, unsigned char buf_len)
{
    unsigned char i;

    crc = 0xFF;

    while (buf_len--)
	  {
        crc ^= *pMas++;
        for (i = 0; i < 8; i++)
        {
            for (i = 0; i < 8; i++)
            {
                crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
            }
		    }
    }

    return crc;
}

void readEE()
{
	  unsigned short addr = 0;
    unsigned char xdata *b1;
    unsigned char xdata *b2;
    static unsigned char xdata crc8;

    // A (0)
    ReadEEPROM(100,b1,6);
    ASIO_WSTRING(b1);
    strcpy(b1, "ANSHA\0");
    WriteEEPROM(100,b1,6);
    ReadEEPROM(addr, b1, 1);
    ReadEEPROM(addr + 1, b2, 1);
    crc8 = CRC8(b1, 1);

    // igithuf (crc8 == *b1)
    // {
    //   A = *b1;
    // } else A = 1;

    A = *b1;

    // N1 (2)
    addr += 2;
    ReadEEPROM(addr, b1, 1);
    ReadEEPROM(addr + 1, b2, 1);
    crc8 = CRC8(b1, 1);

    if (crc8 == *b1) {
    	N1 = *b1;
    } else N1 = 2;

    // D (4)
    addr += 2;
    ReadEEPROM(addr, b1, 1);
    ReadEEPROM(addr + 1, b2, 1);
    crc8 = CRC8(b1, 1);

    if (crc8 == *b1) {
    	D = *b1;
    } else D = 0;

    // S1 (6)
    addr += 2;
    ReadEEPROM(addr, b1, 1);
    ReadEEPROM(addr + 1, b2, 1);
    crc8 = CRC8(b1, 1);

    if (crc8 == *b1) {
    	S1 = *b1;
    } else S1 = 1;

    // N2 (8)
    addr += 2;
    ReadEEPROM(addr, b1, 1);
    ReadEEPROM(addr + 1, b2, 1);
    crc8 = CRC8(b1, 1);

    if (crc8 == *b1) {
    	N2 = *b1;
    } else N2 = 1;

    // S2 (10)
    addr += 2;
    ReadEEPROM(addr, b1, 1);
    ReadEEPROM(addr + 1, b2, 1);
    crc8 = CRC8(b1, 1);

    if (crc8 == *b1) {
    	S2 = *b1;
    } else S2 = 1;
}

void writeEE(unsigned char xdata* bbb, unsigned long addr)
{
    WriteEEPROM(addr, bbb, 1);
    WriteEEPROM(addr + 1, &(CRC8(bbb, 1)), 1);
}

void clearEE() {
	unsigned long addr;
	unsigned char xdata *b1;
	b1 = 0;
	for (addr = 0 ; addr < 12; addr ++) {
		WriteEEPROM(addr, b1, 1);
		WriteEEPROM(addr, b1 + 1, 1);
	}
}

void main(void)
{
    char kCh = 0;
    char uCh = 0;
    int isManualMode = 0;
    int tsel = 0; // Timed mode type selector
    int msel = 0; // Manual mode type selector
    int mselOld = -1;
    int delim = 2; // Timer delimiter
    unsigned int oldTimer = -1;
    unsigned short addr;
    unsigned char N;
    unsigned short size;
    int flag = 1;

    ASIO_INIT( SERIAL_SPEED );
    SCANER_INIT();

    ET2 = 1; // Timer 2 interrupts enabled
    ES = 1; // UART interrupts enabled

    EA = 1; // Enable all interrups
    addr = 1;
    size = 5;

    readEE();

    while (1) {

        if(flag)
        {
          showParams();
          flag = 0;
        }
        kCh = READ_BUFFER();
        uCh = ASIO_READ();

        if ((kCh != 0) || (uCh != 0)) {
            if ((kCh == 'A') || (uCh == 'a')) {
            	if (A) A = 0; else A = 1;
            	writeEE(A, 0);
              flag = 1;
            }
            else if ((kCh == 'D') || (uCh == 'd')) {
            	if (!A) {
            		if (D) D = 0; else D = 1;
            		writeEE(D, 4);
            	}
              flag = 1;
            }
            else if ((kCh == 'B') || uCh == 'n') {
            	if (!A) {
            		N1++;
            		if (N1 > 7) N1 = 1;
            		writeEE(N1, 2);
            	}
            	else {
            		N2++;
            		if (N2 > 3) N2 = 1;
            		writeEE(N2, 8);
            	}
              flag = 1;
            }
            else if ((kCh == 'C') || uCh == 's') {
            	if (!A) {
            		S1++;
            		if (S1 > 9) S1 = 1;
            		writeEE(S1, 6);
            	}
            	else {
            		S2++;
            		if (S2 > 9) S2 = 1;
            		writeEE(S2, 10);
            	}
              flag = 1;
            }
            else if ((kCh == '#') || uCh == '#') {
              flag = 1;
            	clearEE();
            }
            else if ((kCh == '*') || uCh == '*') {
              flag = 1;
            	runAnim();
            }
        }
    }
}

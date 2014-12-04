#include "aduc812.h"
#include "keys.h"
#include "asio.h"
#include "buf.h"
#include "max.h"
#include "lcd.h"

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

void wBs(char * str, char rn)
{
    char i = 0;
    while( *str )
    {
        buf[rn][i] = *str++;
        i++;
    }
}

void printUART()
{
    unsigned char dipVal;
    char wbuffer[16];

    dipVal = READ_DIP();

    ASIO_WSTRING("DIPs ");

    wbuffer[0] = ((dipVal & 0x01)) + '0';
    wbuffer[1] = ((dipVal & 0x02) >> 1) + '0';
    wbuffer[2] = ((dipVal & 0x04) >> 2) + '0';
    wbuffer[3] = ((dipVal & 0x08) >> 3) + '0';
    wbuffer[4] = ((dipVal & 0x10) >> 4) + '0';
    wbuffer[5] = ((dipVal & 0x20) >> 5) + '0';
    wbuffer[6] = ((dipVal & 0x40) >> 6) + '0';
    wbuffer[7] = ((dipVal & 0x80) >> 7) + '0';
    wbuffer[8] = '\n';

    ASIO_WSTRING(wbuffer);

    ASIO_WSTRING("TIME ");

    wbuffer[4] = timer % 10 + '0';
    wbuffer[3] = (timer / 10) % 10 + '0';
    wbuffer[2] = (timer / 100) % 10 + '0';
    wbuffer[1] = (timer / 1000) % 10 + '0';
    wbuffer[0] = (timer / 10000) % 10 + '0';
    wbuffer[5] = ' ';
    wbuffer[6] = ' ';
    wbuffer[7] = ' ';
    wbuffer[8] = '\n';

    ASIO_WSTRING(wbuffer);

    ASIO_WSTRING("TL0 ");

    wbuffer[4] = TL0 % 10 + '0';
    wbuffer[3] = (TL0 / 10) % 10 + '0';
    wbuffer[2] = (TL0 / 100) % 10 + '0';
    wbuffer[1] = (TL0 / 1000) % 10 + '0';
    wbuffer[0] = (TL0 / 10000) % 10 + '0';
    wbuffer[5] = ' ';
    wbuffer[6] = ' ';
    wbuffer[7] = ' ';
    wbuffer[8] = '\n';

    ASIO_WSTRING(wbuffer);
}

void lcdDisplayBufferDips()
{
    unsigned char dipVal;

    dipVal = READ_DIP();

    lcd_clear();
    cleanBuffer();

    buf[0][0] = 'D';
    buf[0][1] = 'I';
    buf[0][2] = 'P';
    buf[0][3] = 's';

    buf[1][0] = ((dipVal & 0x01)) + '0';
    buf[1][1] = ((dipVal & 0x02) >> 1) + '0';
    buf[1][2] = ((dipVal & 0x04) >> 2) + '0';
    buf[1][3] = ((dipVal & 0x08) >> 3) + '0';
    buf[1][4] = ((dipVal & 0x10) >> 4) + '0';
    buf[1][5] = ((dipVal & 0x20) >> 5) + '0';
    buf[1][6] = ((dipVal & 0x40) >> 6) + '0';
    buf[1][7] = ((dipVal & 0x80) >> 7) + '0';

    lcdDisplayBuffer();
}

void lcdDisplayBufferTime()
{
    lcd_clear();
    cleanBuffer();

    buf[0][0] = 'S';
    buf[0][1] = 'Y';
    buf[0][2] = 'S';
    buf[0][4] = 'T';
    buf[0][5] = 'i';
    buf[0][6] = 'm';
    buf[0][7] = 'e';

    buf[1][4] = timer % 10 + '0';
    buf[1][3] = (timer / 10) % 10 + '0';
    buf[1][2] = (timer / 100) % 10 + '0';
    buf[1][1] = (timer / 1000) % 10 + '0';
    buf[1][0] = (timer / 10000) % 10 + '0';

    lcdDisplayBuffer();
}

void lcdDisplayBufferTH0()
{
    lcd_clear();
    cleanBuffer();

    buf[0][0] = 'T';
    buf[0][1] = 'L';

    buf[1][4] = TL0 % 10 + '0';
    buf[1][3] = (TL0 / 10) % 10 + '0';
    buf[1][2] = (TL0 / 100) % 10 + '0';
    buf[1][1] = (TL0 / 1000) % 10 + '0';
    buf[1][0] = (TL0 / 10000) % 10 + '0';

    lcdDisplayBuffer();
}

void main(void)
{
    char ch = 0;
    int isManualMode = 0;
    int tsel = 0; // Timed mode type selector
    int msel = 0; // Manual mode type selector
    int mselOld = -1;
    int delim = 2; // Timer delimiter
    unsigned int oldTimer = -1;

    ASIO_INIT( SERIAL_SPEED );
    SCANER_INIT();

    ET2 = 1; // Timer 2 interrupts enabled
    ES = 1; // UART interrupts enabled

    // Timer 0
    TL0 = 0; // Clean Timer 0
    TH0 = 0;
    TMOD |= 0x05; // Timer 0 external event mode
    TR0 = 1; // Run Timer 0
    //ET0 = 1; // Timer 0 interrupts

    EA = 1; // Enable all interrups

    while (1)
    {
        if (TL0 >= 1)
        {
                lcd_clear();
                cleanBuffer();

                buf[0][0] = 'J';
                buf[1][0] = 'J';
                lcdDisplayBuffer();
        }
        ch = READ_BUFFER();
        if (ch != 0)
        {
            if (ch == 'D')
            {
                if (isManualMode)
                    isManualMode = 0;
                else isManualMode = 1;
            }
            // Mnual mode type select
            else if ((ch == '#') && (isManualMode))
            {
                msel ++;
                if (msel > 2)
                    msel = 0;
            }

            // Change of timer delimiter for auto mode
            else if ((ch == 'A') && (isManualMode))
            {
                delim++;
                if (delim > 10)
                    delim = 10;
            }
            else if ((ch == 'B') && (isManualMode))
            {
                delim--;
                if (delim < 2)
                delim = 2;
            }
        }

        // Manual mode
        if (isManualMode)
        {
            if ((msel == 0) && (mselOld != msel))
            {
                lcdDisplayBufferTime();
                mselOld = msel;
            }
            else if ((msel == 1) && (mselOld != msel))
            {
                lcdDisplayBufferDips();
                mselOld = msel;
            }
            else if (mselOld != msel)
            {
                lcdDisplayBufferTH0();
                mselOld = msel;
            }
        }
        // Auto mode
        else if ((timer % delim == 0) && (timer != oldTimer))
        {
            // System Time
            if (tsel == 0)
            {
                lcdDisplayBufferTime();
                oldTimer = timer;
                tsel++;
            }
            // DIPs
            else if (tsel == 1)
            {
                lcdDisplayBufferDips();
                oldTimer = timer;
                tsel++;
            }
            // TH0
            else
            {
                lcdDisplayBufferTH0();
                oldTimer = timer;
                tsel = 0;
            }
        }
        if ((timer % 10 == 0) && (timer != oldTimer))
        {
            printUART();
        }
    }
}

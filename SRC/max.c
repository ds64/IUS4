#include "ADuC812.h"
#include "max.h"

#define MAXBASE 0x8 

void WriteMax(unsigned char xdata *regnum, unsigned char val)
{
    unsigned char oldDPP=DPP;
    DPP=0x8;
    *regnum=val;
    DPP=oldDPP;
}

unsigned char ReadMax(unsigned char xdata *regnum)
{
    unsigned char oldDPP=DPP;
    unsigned char val=0;
    DPP=0x8;
    val=*regnum;
    DPP=oldDPP;
    return val;
}

void BEEP()
{
    unsigned char i;
    unsigned short duration;

    for(duration = 666; duration > 0; duration--)
    {
        WriteMax(ENA,0x3C);
        for( i = 2; i > 0; i--);
        WriteMax(ENA,0x00);
        for( i = 2; i > 0; i--);
    }
}

void write_max( unsigned char xdata *regnum, unsigned char val )
{
    unsigned char oldDPP = DPP;

    DPP     = MAXBASE;
    *regnum = val;
    DPP     = oldDPP;
}

unsigned char read_max( unsigned char xdata *regnum )
{
    unsigned char oldDPP=DPP;
    unsigned char val;

    DPP = MAXBASE;
    val = *regnum;
    DPP = oldDPP;

    return val;
}

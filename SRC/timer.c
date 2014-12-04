#include "timer.h"
#include "buf.h"

int tm2;
unsigned long xdata timerms;
unsigned long xdata timers;

void INTERRUPT_HANDLER() __interrupt ( 5 )
{
    EA = 0;
    TH2 = 0x00;
    TL2 = 0x00;
    tm2 ++;

    if (timerms >= 1000)
    {
        timerms = 0;
        timers += 1;
    }

    if (tm2 % 50 == 0)
    {
        tm2 = 0;
        timerms = timerms + 1;
    }
}

void InitTimer()
{
    tm2 = 0;
    timerms = 0;
    timers = 0;

    TH2 = 0;
    TL2 = 0;
    TR2 = 1;

    SetVector(0x202B, (void *) INTERRUPT_HANDLER );
}

unsigned long GetMsCounter()
{
    return timerms + timers*1000;
}

unsigned long DTimeMs(unsigned long t0)
{
    return (t0 - (timers*1000 + timerms));
}

void DelayMs(unsigned long t)
{
    tm2 = 0;
    timers = 0;
    timerms = 0;
    while((timers*1000 + timerms) < t)
    {

    }
}

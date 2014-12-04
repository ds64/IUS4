#ifndef _TIMER_H_
#define _TIMER_H_

#include "aduc812.h"

void InitTimer();
unsigned long GetMsCounter();
unsigned long DTimeMs(unsigned long t0);
void DelayMs(unsigned long t);

#endif _TIMER_H_

#ifndef _RTC_H_
#define _RTC_H_

#include "aduc812.h"

struct TimeDate
{

};

void InitRTC();
unsigned char ReadRTC(TimeDate * td);
unsigned char WriteRTC(TimeDate * td);

#endif _RTC_H_

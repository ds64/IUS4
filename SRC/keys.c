#include "keys.h"
#include "aduc812.h"
#include "max.h"
#include "buf.h"
#include "asio.h"
struct BUF io_kb_buff;

bit KB_SCAN_COLUMN(char *ch);
int tm = 0;
unsigned int timer = 0;

char READ_BUFFER(void)
{
    unsigned char ch;
    ES = 0;
    ch = POP_BUF(&io_kb_buff);
    ES = 1;
    return ch;
}

void SCANER_HANDLER() __interrupt ( 5 )
{
    unsigned char ch;
    EA = 0;
    TH2 = 0x00;
    TL2 = 0x00;
    tm ++;

    if (timer >= 32766)
        timer = 0;

    if (tm % 1000 == 0)
    {
        timer = timer + 1;
    }

    if (tm % 1000 == 0)
    {
        if (KB_SCAN_BIT(&ch))
        {
            PUSH_BUF(&io_kb_buff, ch);
        }
    }

    if (tm > 10000)
        tm = 0;
    EA = 1;
}

void SCANER_INIT()
{
    tm = 0; // local key timer
    timer = 0; // Global timer

    TH2 = 0x00;
    TL2 = 0x00;
    TR2 = 1; // Run Timer 2
    INIT_BUF(&io_kb_buff);
    SetVector(0x202B, (void *) SCANER_HANDLER );
}

bit KB_SCAN_COLUMN(char *ch)
{
    const static char KBTable[]="147*2580369#ABCD";
    static unsigned char colnum = 0;
    unsigned char row, col, rownum;
    unsigned int i;

    colnum = ++colnum % 4;

    col = 0x1 << colnum;

    WriteMax(KB, ~col);

    for(rownum = 0; rownum < 4; rownum++)
    {
        row = ReadMax(KB) & (0x10 << rownum);
        if( !row )
        {
            for(i=0; i < 1000; i++);
            row = ReadMax(KB) & (0x10 << rownum);
            if( !row )
            {
                *ch = (KBTable[(colnum<<2) + rownum]);
                return 1;
            }
        }
    }
    return 0;
}

bit KB_SCAN_BIT(char *ch)
{
    const static char KBTable[]="147*2580369#ABCD";
    unsigned char row, col, rownum, colnum;
    unsigned int i;

    for(colnum = 0; colnum < 4; colnum++)
    {
        col = 0x1 << colnum;
        WriteMax(KB, ~col);
        for(rownum = 0; rownum < 4; rownum++)
        {
            row = ReadMax(KB) & (0x10 << rownum);
            if (!row)
            {
                for (i = 0; i < 1000; i++)
                    continue;
                row = ReadMax(KB) & (0x10 << rownum);
                if (!row)
                {
                    BEEP();
                    *ch = (KBTable[(colnum << 2) + rownum]);
                    return 1;
                }
            }
        }

    }

    return 0;
}

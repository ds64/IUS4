#include "aduc812.h"
#include "asio.h"
#include "buf.h"

struct BUF io_write_buff, io_read_buff;
bit async_wr;

void ASIO_ISR() __interrupt ( 4 )
{
    unsigned char c;
    TR2 = 0;
    EA = 0;
    if(TI)
    {
        c = POP_BUF(&io_write_buff);
        if (c != 0)
        {
            SBUF = c;
            TI = 0;
            async_wr = 1;
        }
        else
        {
            async_wr = 0;
        }
        TI = 0;
    }
    if(RI)
    {
        PUSH_BUF(&io_read_buff, SBUF);
        RI=0;
    }
    TR2 = 1;
    EA = 1;
}

void ASIO_INIT( unsigned char speed )
{
    TH1       =  speed;
    TMOD     |=  0x20;
    TCON     |=  0x40;
    PCON     |=  0x00;
    SCON      =  0x50;
    ES        =  0;
    SetVector(0x2023, (void*)ASIO_ISR);
    INIT_BUF(&io_read_buff);
    INIT_BUF(&io_write_buff);
}

char ASIO_READ(void)
{
    unsigned char ch;
    ES = 0;
    ch = POP_BUF(&io_read_buff);
    ES = 1;
    return ch;
}

char ASIO_WCHAR(unsigned char ch)
{
    ES = 0;
    TR2 = 0;
    PUSH_BUF(&io_write_buff, ch);
    if(!async_wr) TI = 1;
    ES = 1;
    TR2 = 1;
    return ch;
}

void ASIO_WSTRING(char * str)
{
    int i = 0;
    while( *str )
    {
        ASIO_WCHAR( *str++ );
    }
}

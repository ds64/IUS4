#include "aduc812.h"
#include "i2c.h"

static bit send_byte( unsigned char ch )
{
		char i;
		bit ack;

    MDE = 1;
    MCO = 0;

    for( i = 0 ; i < 8; i++, ch <<= 1 )
    {
        MDO = ( ch & 0x80 ) ? 1 : 0 ;
        MCO = 1;

        MCO = 0;
    }

    MDE = 0;
    MCO = 1;

    ack = MDI;
    MCO = 0;
    return ack;
}

static void start(void)
{
    MDE = 1;
    MDO = 1;
    MCO = 1;

    MDO = 0;

    MCO = 0;
}

static void stop(void)
{
    MDE = 1;
    MCO = 0;
    MDO = 0;
    MCO = 1;

    MDO = 1;

    MDE = 0;
}

static bit begin( unsigned char addr )
{
    start();
    return send_byte( addr );
}

static void ack( void )
{
    MDE = 1;
    MCO = 0;
    MDO = 0;
    MCO = 1;

    MCO = 0;
}

static void nack(void)
{
    MDE = 1;
    MCO = 0;
    MDO = 1;
    MCO = 1;

    MCO = 0;
}

bit get_ack( unsigned char address )
{
    I2CM = 1;

    if( begin( address & 0xFE ) )
    {
        stop();
        return 0;
    }

    stop();
    return 1;
}

unsigned char recv_byte(void)
{
		char i;
		unsigned char ch = 0;

    MDE = 0;
		MCO = 0;

    for( i = 0; i < 8; i++ )
    {
        ch  <<= 1;
        MCO   = 1;

        ch |= MDI;
        MCO = 0;
    }

    return ch;
}

bit receive_block(unsigned char address, unsigned char addr, unsigned char xdata * block,unsigned char len)
{
		unsigned char i, ch;

    I2CM    = 1;
    address = ( address & 0xFE );

    if(begin(address))
    {
        stop();
        return 1;
    }

    if(send_byte(addr))
    {
        stop();
				return 1;
    }

		address |= 1;

    if(begin(address))
    {
        stop();
        return 1;
    }

    if(len-1)
    {
        for( i = 0; i<(len-1); i++)
        {
            ch = recv_byte();
            ack();
            *block++ = ch;
        }
    }

    ch = recv_byte();
    nack();
    *block = ch;

    stop();

    return 0;
}

bit transmit_block(unsigned char address, unsigned char addr, unsigned char xdata * block,unsigned char len)
{
		unsigned char ch, i;

    I2CM    = 1;
    address = address & 0xFE;

    if(begin(address))
    {
        stop();
        return 1;
    }

    if(send_byte(addr))
    {
				stop();
        return 1;
    }

		for( i = 0; i<len; i++,block++)
    {
        ch = *block;
        if(send_byte(ch))
        {
            stop();
            return 1;
        }
    }

    stop();

    return 0;
}

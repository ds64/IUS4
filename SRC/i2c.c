/* I2C driver */

#include "aduc812.h"
#include "i2c.h"

// delay - �������� �� 25�����
static void delay( void )
{
		char ch = 0;
    while( ch++ < 2 );
}

// send_byte() - ������� ����� �� I2C � �������� �������������
// ����:		ch - ����, ������� ����� �������
// ���������:	0 - ������������� ���������
//				1 - ������������� �� ���������
static bit send_byte( unsigned char ch ) //Returns ack (0 = acknowledged)
{
		char i;
		bit ack;

    MDE = 1;	// �������� ������
    MCO = 0;	// SCLOCK=0

    for( i = 0 ; i < 8; i++, ch <<= 1 )
    {
        MDO = ( ch & 0x80 ) ? 1 : 0 ; // ������ ������ �� SDATA
        MCO = 1;					  // SCLOCK=1

        //delay();

        MCO = 0;					  // SCLOCK=0
    }

    MDE = 0;	// ����� ������
    MCO = 1;	// SCLOCK=1

    //delay();
    ack = MDI;	// ���������� SDATA ���� SCLOCK=0,MDE=0(�����)
    MCO = 0;	// SCLOCK=0
    return ack;
}
// start() - ��������� ��������� Start ���������� I2�
static void start(void)
{
    MDE = 1;	// �������� ������
    MDO = 1;	// SDATA=1
    MCO = 1;	// SCLOCK=1

    //delay();

    MDO = 0;	// SDATA=0

    //delay();

    MCO = 0;	// SCLOCK=0
}

// stop() - ��������� ��������� Stop ���������� I2�
static void stop(void)
{
    MDE = 1;	// �������� ������
    MCO = 0;	// SCLOCK=0
    MDO = 0;	// SDATA=0
    MCO = 1;	// SCLOCK=1

    //delay();
    MDO = 1;	// SDATA=1
    //delay();

    MDE = 0;	// ����� ������
}

// begin - ������ ������ �� ���������� I2C (Start+������� ������ slave-����������)
// ����: addr - ����� I2C ���������
// ���������: 1-��������� ������������
//			  0-��������� �� ������������
static bit begin( unsigned char addr )//Returns ack (0 = acknowledged)
{
    start();
    return send_byte( addr );
}

// ack - ������� ������������� ����������
static void ack( void )//Sends ack
{
    MDE = 1;	// �������� ������
    MCO = 0;	// SCLOCK=0
    MDO = 0;	// SDATA=0
    MCO = 1;	// SCLOCK=1

    //delay();

    MCO = 0;	// SCLOCK=0
}

// nack - ������� "���������������" ����������
static void nack(void) //Sends NAck
{
    MDE = 1;	// �������� ������
    MCO = 0;	// SCLOCK=0
    MDO = 1;	// SDATA=1
    MCO = 1;	// SCLOCK=1

    //delay();

    MCO = 0;	// SCLOCK=0
}

// get_ack - �������� �� ���������� slave ���������� � ������
// (������ + ��������� ������� + ���������� ������)
bit get_ack( unsigned char address ) //Returns 1 if there was an ACK
{
    I2CM = 1;	//������ ����� I2C Master mode(����������� �������)

    if( begin( address & 0xFE ) )
    {
        stop();
        return 0;
    }

    stop();
    return 1;
}

// recv_byte - ��������� 8 ��� � ���� ������ I2C c �������������/���������������� ������
unsigned char recv_byte(void)
{
		char i;
		unsigned char ch = 0;

    MDE = 0;	// ����� ������
    MCO = 0;	// SCLOCK=0

    for( i = 0; i < 8; i++ )
    {
        ch  <<= 1;
        MCO   = 1;	// SCLOCK=1

        //delay();

        ch |= MDI;	// ������ ������ � SDATA
        MCO = 0;	// SCLOCK=0
    }

    return ch;
}

// receive_block - ��������� ����� ������ �� I2C-���������� � 8-��������� ���������� �������
// �������� ���� ���������� � ������� ������ xdata
// ����: address - ����� I2C ����������
//		 addr - ����� �� ���������� �������� ������������ ����������
//		 block - ����� � xdata ���� ����� �������� �������� ���� ������
//		 len - ����� ������������ �����
// ���������: 0 - ��������� ������ ������ �������
//			  1 - ���������� �� �����������
bit receive_block(unsigned char address, unsigned char addr, unsigned char xdata * block,unsigned char len)
{ //addr - address in target
		unsigned char i, ch;

    I2CM    = 1;                    // I2C Master mode
    address = ( address & 0xFE );   // Write

    if(begin(address))
    {
        stop();				// ������ ��� �������� DEVICE ADDRESS
        return 1;			// ���������� �� �����������
    }

    if(send_byte(addr))
    {
        stop();				// ������ ��� �������� WORD ADDRESS
        return 1;			// �� ���� ��������� ������������� ack
    }

// ������ ������

		address |= 1;

    if(begin(address))
    {
        stop();				// ������ ��� �������� DEVICE ADDRESS
        return 1;			// ���������� �� �����������
    }

    if(len-1)
    {
        for( i = 0; i<(len-1); i++)
        {
            ch = recv_byte();	// ��������� ����� ������
            ack();				// �������� ACK
            *block++ = ch;		// ������ ������ � ������
        }
    }

    ch = recv_byte();	// ��������� ����� ������
    nack();				// �������� noACK
    *block = ch;		// ������ ������ � ������

    stop();				// ������ stop-���� ������ ���� �������������

    return 0;
}

// transmit_block - ������ ����� ������ �� I2C-���������� � 8-��������� ���������� �������
//
// ����: address - ����� I2C ����������
//		 addr - ����� �� ���������� �������� ������������ ����������
//		 block - ����� � xdata ��� ������������� ������ ��� ���������
//		 len - ����� ������������� �����
// ���������: 0 - �������� ������ ������ �������
//			  1 - ���������� �� �����������
bit transmit_block(unsigned char address, unsigned char addr, unsigned char xdata * block,unsigned char len)
{ //addr - address in target
		unsigned char ch, i;

    I2CM    = 1;                //I2C Master mode
    address = address & 0xFE;   //Write

    if(begin(address))
    {
        stop();				// ������ ��� �������� DEVICE ADDRESS
        return 1;			// ���������� �� �����������
    }

    if(send_byte(addr))
    {
				stop();				// ������ ��� �������� WORD ADDRESS
        return 1;			// ���������� �� �����������
    }

		for( i = 0; i<len; i++,block++)
    {
        ch = *block;
        if(send_byte(ch))
        {
            stop();			// ������ ��� �������� - �� ��� �������� ��� �������������
            return 1;
        }
    }

    stop();					// ������ stop-���� ������ ���� �������������

    return 0;
}

// receive_block16 - ��������� ����� ������ �� I2C-���������� � 16-��������� ���������� �������
// �������� ���� ���������� � ������� ������ xdata
// ����: address - ����� I2C ����������
//		 addr - ����� �� ���������� �������� ������������ ����������
//		 block - ����� � xdata ���� ����� �������� �������� ���� ������
//		 len - ����� ������������ �����
// ���������: 0 - ��������� ������ ������ �������
//			  1 - ���������� �� �����������
bit receive_block16(unsigned char address, unsigned short addr, unsigned char xdata * block,unsigned short len)
{ //addr - address in target
		unsigned char ch;
		unsigned short i;

    I2CM     = 1;       // I2C Master mode
    address &= 0xFE;    // Write

    if(begin(address))
    {
        stop();				// ������ ��� �������� DEVICE ADDRESS
        return 1;			// ���������� �� �����������
    }

    if(send_byte((unsigned char)(addr >> 8)))
    {
        stop();				// ������ ��� �������� WORD ADDRESS
        return 1;			// �� ���� ��������� ������������� ack
    }

    if(send_byte((unsigned char)addr))
    {
        stop();				// ������ ��� �������� WORD ADDRESS
        return 1;			// �� ���� ��������� ������������� ack
    }


 // ������ ������

		address |= 1;

    if(begin(address))
    {
        stop();				// ������ ��� �������� DEVICE ADDRESS
        return 1;			// ���������� �� �����������
    }


		if(len-1)
		{
        for( i = 0; i<(len-1); i++)
        {
            ch = recv_byte();	// ��������� ����� ������
            ack();				// �������� ACK
            *block++ = ch;		// ������ ������ � ������
        }
		}

		ch = recv_byte();	// ��������� ����� ������
    nack();				// �������� noACK
    *block = ch;		// ������ ������ � ������

    stop();				// ������ stop-���� ������ ���� �������������

    return 0;
}

// transmit_block - ������ ����� ������ �� I2C-���������� � 16-��������� ���������� �������
// �� ���������� ������
// ����: address - ����� I2C ����������
//		 addr - ����� �� ���������� �������� ������������ ����������
//		 block - ����� � xdata ��� ������������� ������ ��� ���������
//		 len - ����� ������������� �����
// ���������: 0 - �������� ������ ������ �������
//			  1 - ���������� �� �����������
bit transmit_block16(unsigned char address, unsigned short addr, unsigned char xdata * block,unsigned short len)
{ //addr - address in target
		unsigned char ch;
		unsigned short i;

    I2CM     = 1;    // I2C Master mode
    address &= 0xFE; // Write

    if(begin(address))
    {
        stop();				// ������ ��� �������� DEVICE ADDRESS
        return 1;			// ���������� �� �����������
    }

    if( send_byte( ( unsigned char )( addr >> 8) ) )
    {
				stop();				// ������ ��� �������� WORD ADDRESS
        return 1;			// ���������� �� �����������
    }

    if( send_byte( ( unsigned char )addr ) )
    {
				stop();				// ������ ��� �������� WORD ADDRESS
        return 1;			// ���������� �� �����������
    }

		for( i = 0; i<len; i++,block++)
    {
        ch = *block;
        if(send_byte(ch))
        {
            stop();			// ������ ��� �������� - �� ��� �������� ��� �������������
            return 1;
        }
    }

		stop();					// ������ stop-���� ������ ���� �������������

    return 0;
}

/* I2C driver */

#include "aduc812.h"
#include "i2c.h"

// delay - задержка на 25мксек
static void delay( void )
{
	char ch = 0;
    while( ch++ < 2 );
}

// send_byte() - посылка байта по I2C и ожидание подтверждения
// Вход:		ch - байт, который нужно послать
// Результат:	0 - подтверждение полученно
//				1 - подтверждение не полученно
static bit send_byte( unsigned char ch ) //Returns ack (0 = acknowledged)
{
	char i;
	bit ack;

    MDE = 1;	// передача данных
    MCO = 0;	// SCLOCK=0

    for( i = 0 ; i < 8; i++, ch <<= 1 )
    {
        MDO = ( ch & 0x80 ) ? 1 : 0 ; // запись данных на SDATA
        MCO = 1;					  // SCLOCK=1	

        //delay();

        MCO = 0;					  // SCLOCK=0
    }

    MDE = 0;	// прием данных
    MCO = 1;	// SCLOCK=1

    //delay();
    ack = MDI;	// считывание SDATA если SCLOCK=0,MDE=0(прием)
    MCO = 0;	// SCLOCK=0
    return ack;
}
// start() - установка состояния Start интерфейса I2С
static void start(void)
{
    MDE = 1;	// передача данных
    MDO = 1;	// SDATA=1
    MCO = 1;	// SCLOCK=1

    //delay();

    MDO = 0;	// SDATA=0

    //delay();

    MCO = 0;	// SCLOCK=0
}

// stop() - установка состояния Stop интерфейса I2С
static void stop(void)
{
    MDE = 1;	// передача данных
    MCO = 0;	// SCLOCK=0
    MDO = 0;	// SDATA=0
    MCO = 1;	// SCLOCK=1

    //delay();
    MDO = 1;	// SDATA=1
    //delay();

    MDE = 0;	// прием данных
}

// begin - начало сессии на интерфейсе I2C (Start+Посылка адреса slave-устройства)
// Вход: addr - адрес I2C утройства
// Результат: 1-устроство откликнулось
//			  0-устроство не откликнулось
static bit begin( unsigned char addr )//Returns ack (0 = acknowledged)
{
    start();
    return send_byte( addr );
}

// ack - посылка подтверждения устройству
static void ack( void )//Sends ack
{
    MDE = 1;	// передача данных
    MCO = 0;	// SCLOCK=0
    MDO = 0;	// SDATA=0
    MCO = 1;	// SCLOCK=1

    //delay();

    MCO = 0;	// SCLOCK=0
}

// nack - посылка "неподтверждения" устройству
static void nack(void) //Sends NAck
{
    MDE = 1;	// передача данных
    MCO = 0;	// SCLOCK=0
    MDO = 1;	// SDATA=1
    MCO = 1;	// SCLOCK=1

    //delay();

    MCO = 0;	// SCLOCK=0
}

// get_ack - проверка на готовность slave устройства к обмену
// (начало + получение отклика + завершении сессии)
bit get_ack( unsigned char address ) //Returns 1 if there was an ACK
{
    I2CM = 1;	//Выбран режим I2C Master mode(программный ведущий)

    if( begin( address & 0xFE ) ) 
    { 
        stop(); 
        return 0;	
    }

    stop();
    return 1;
}

// recv_byte - получение 8 бит с шины данных I2C c подтвержением/неподтверждением приема
unsigned char recv_byte(void)
{
	char i;
	unsigned char ch = 0;

    MDE = 0;	// прием данных
    MCO = 0;	// SCLOCK=0

    for( i = 0; i < 8; i++ )
    {
        ch  <<= 1;
        MCO   = 1;	// SCLOCK=1

        //delay();

        ch |= MDI;	// чтение данных с SDATA
        MCO = 0;	// SCLOCK=0
    }

    return ch;
}

// receive_block - получение блока данных от I2C-устройства с 8-разрядным внутренним адресом
// принятый блок помещается в область памяти xdata
// Вход: address - адрес I2C устройства
//		 addr - адрес во внутреннем адресном пространстве устройства
//		 block - адрес в xdata куда будет помещенн принятый блок данных
//		 len - длина принимаемого блока
// Результат: 0 - получение данных прошло успешно
//			  1 - устройство не откликается
bit receive_block(unsigned char address, unsigned char addr, unsigned char xdata * block,unsigned char len)
{ //addr - address in target
	unsigned char i, ch;

    I2CM    = 1;                    // I2C Master mode
    address = ( address & 0xFE );   // Write

    if(begin(address)) 
    {
        stop();				// ошибка при отправке DEVICE ADDRESS
        return 1;			// устройство не откликается
    }   

    if(send_byte(addr))		
    {
        stop();				// ошибка при отправке WORD ADDRESS 
        return 1;			// не было полученно подтверждения ack
    }

    //delay();
    //delay();

// чтение данных
    
	address |= 1;			 

    if(begin(address)) 
    {
        stop();				// ошибка при отправке DEVICE ADDRESS
        return 1;			// устройство не откликается
    }   

    //delay();

    if(len-1)
    {
        for( i = 0; i<(len-1); i++)
        {
            ch = recv_byte();	// получение байта данных
            ack();				// отправка ACK
            *block++ = ch;		// запись данных в память
        }
    }

    ch = recv_byte();	// получение байта данных
    nack();				// отправка noACK
    *block = ch;		// запись данных в память

    stop();				// выдача stop-бита вместо бита подтверждения

    return 0;
}

// transmit_block - запись блока данных от I2C-устройства с 8-разрядным внутренним адресом
// 
// Вход: address - адрес I2C устройства
//		 addr - адрес во внутреннем адресном пространстве устройства
//		 block - адрес в xdata где располагаются данные для пересылки
//		 len - длина записываемого блока
// Результат: 0 - отправка данных прошла успешно
//			  1 - устройство не откликается
bit transmit_block(unsigned char address, unsigned char addr, unsigned char xdata * block,unsigned char len)
{ //addr - address in target
	unsigned char ch, i;

    I2CM    = 1;                //I2C Master mode
    address = address & 0xFE;   //Write

     if(begin(address)) 
    {
        stop();				// ошибка при отправке DEVICE ADDRESS
        return 1;			// устройство не откликается
    }   

    if(send_byte(addr)) 
    {
		stop();				// ошибка при отправке WORD ADDRESS
        return 1;			// устройство не откликается
    }
//--------------------------????
//	for( i = 0; i<(len-1); i++,block++)					//--------------------------????    
	for( i = 0; i<len; i++,block++)
    {
        ch = *block;
        if(send_byte(ch))
        { 
            stop();			// ошибка при отправки - не был полученн бит подтверждения
            return 1;
        }   
    }

    stop();					// выдача stop-бита вместо бита подтверждения

    return 0;
}

// receive_block16 - получение блока данных от I2C-устройства с 16-разрядным внутренним адресом
// принятый блок помещается в область памяти xdata
// Вход: address - адрес I2C устройства
//		 addr - адрес во внутреннем адресном пространстве устройства
//		 block - адрес в xdata куда будет помещенн принятый блок данных
//		 len - длина принимаемого блока
// Результат: 0 - получение данных прошло успешно
//			  1 - устройство не откликается
bit receive_block16(unsigned char address, unsigned short addr, unsigned char xdata * block,unsigned short len)
{ //addr - address in target
	unsigned char ch;
	unsigned short i;

    I2CM     = 1;       // I2C Master mode
    address &= 0xFE;    // Write

    if(begin(address)) 
    {
        stop();				// ошибка при отправке DEVICE ADDRESS
        return 1;			// устройство не откликается
    }   

    if(send_byte((unsigned char)(addr >> 8)))
    {
        stop();				// ошибка при отправке WORD ADDRESS 
        return 1;			// не было полученно подтверждения ack
    }

    if(send_byte((unsigned char)addr))
    {
        stop();				// ошибка при отправке WORD ADDRESS 
        return 1;			// не было полученно подтверждения ack
    }

    //delay();
    //delay();

 // чтение данных
    
	address |= 1;			 

    if(begin(address)) 
    {
        stop();				// ошибка при отправке DEVICE ADDRESS
        return 1;			// устройство не откликается
    }   

    //delay();

	if(len-1)
	{
        for( i = 0; i<(len-1); i++)
        {
            ch = recv_byte();	// получение байта данных
            ack();				// отправка ACK
            *block++ = ch;		// запись данных в память
        }
	}
	
	ch = recv_byte();	// получение байта данных
    nack();				// отправка noACK
    *block = ch;		// запись данных в память

    stop();				// выдача stop-бита вместо бита подтверждения

    return 0;
}

// transmit_block - запись блока данных от I2C-устройства с 16-разрядным внутренним адресом
// во внутреннюю память
// Вход: address - адрес I2C устройства
//		 addr - адрес во внутреннем адресном пространстве устройства
//		 block - адрес в xdata где располагаются данные для пересылки
//		 len - длина записываемого блока
// Результат: 0 - отправка данных прошла успешно
//			  1 - устройство не откликается
bit transmit_block16(unsigned char address, unsigned short addr, unsigned char xdata * block,unsigned short len)
{ //addr - address in target
unsigned char ch;
unsigned short i;

    I2CM     = 1;    // I2C Master mode
    address &= 0xFE; // Write

    if(begin(address)) 
    {
        stop();				// ошибка при отправке DEVICE ADDRESS
        return 1;			// устройство не откликается
    }

    if( send_byte( ( unsigned char )( addr >> 8) ) ) 
    {
		stop();				// ошибка при отправке WORD ADDRESS
        return 1;			// устройство не откликается
    }

    if( send_byte( ( unsigned char )addr ) ) 
    {
		stop();				// ошибка при отправке WORD ADDRESS
        return 1;			// устройство не откликается
    }

//--------------------------????
//	for( i = 0; i<(len-1); i++,block++)					//--------------------------????    
	for( i = 0; i<len; i++,block++)
    {
        ch = *block;
        if(send_byte(ch))
        { 
            stop();			// ошибка при отправки - не был полученн бит подтверждения
            return 1;
        }   
    }

	stop();					// выдача stop-бита вместо бита подтверждения

    return 0;
}

#ifndef _I2C_H_
#define _I2C_H_

static void delay();

static char send_byte(unsigned char c);
static void start();
static void stop();
static char begin(unsigned char addr);

static void ack();
static void nack();

unsigned char receive_byte();

char get_ack(unsigned char addr);
char receive_block(unsigned char eeprom_addr, unsigned char addr,
                   unsigned char * block, unsigned short size);
char transmit_block(unsigned char eeprom_addr, unsigned char addr,
                   unsigned char * block, unsigned short size);

#endif _I2C_H_

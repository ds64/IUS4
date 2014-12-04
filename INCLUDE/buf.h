#ifndef _BUF_H_
#define _BUF_H_

#define BUFFSIZE 15

void SetVector(unsigned char xdata * address, void * vector);

struct BUF
{
    unsigned char buff[BUFFSIZE];
    char write_index;
    char count;
};

void PUSH_BUF(struct BUF * buffer, unsigned char ch);
unsigned char POP_BUF(struct BUF * buffer);
void INIT_BUF(struct BUF * buffer);

#endif _BUF_H_

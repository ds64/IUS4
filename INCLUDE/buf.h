#ifndef _BUF_H_
#define _BUF_H_

#define BUFFSIZE 15
#define FIFOSIZE 64

void SetVector(unsigned char xdata * address, void * vector);

struct BUF
{
    unsigned char buff[BUFFSIZE];
    char write_index;
    char count;
};

struct FIFO
{
  unsigned char buf[FIFOSIZE];
  char start;
  char end;
};

void PUSH_BUF(struct BUF * buffer, unsigned char ch);
unsigned char POP_BUF(struct BUF * buffer);
void INIT_BUF(struct BUF * buffer);

unsigned char POP_FIFO(struct FIFO * fifo_buf);
void PUSH_FIFO(struct FIFO * fifo_buf, unsigned char c);

#endif _BUF_H_

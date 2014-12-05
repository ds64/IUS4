#include "buf.h"

void SetVector(unsigned char xdata * address, void * vector)
{
    unsigned char xdata * tmp_vector;
    *address    = 0x02;
    tmp_vector  = (unsigned char xdata *)(address+1);
    *tmp_vector = (unsigned char) ((unsigned short)vector >> 8);
    ++tmp_vector;
    *tmp_vector = (unsigned char) vector;
}

void PUSH_BUF(struct BUF * buffer, unsigned char ch)
{
    buffer->buff[buffer->write_index] = ch;
    buffer->write_index = (buffer->write_index + 1) % 15;
    if (buffer->count < 15)
    {
        buffer->count++;
    }
}

unsigned char POP_BUF(struct BUF * buffer)
{
    if (buffer->count)
    {
        unsigned char read_index;
        if ((buffer->write_index - buffer->count) < 0)
        {
            read_index = 15 + (buffer->write_index - buffer->count);
        }
        else
        {
            read_index = (buffer->write_index - buffer->count);
        }
        buffer->count--;
        return buffer->buff[read_index];
    }
    return 0;
}

void INIT_BUF(struct BUF * buffer)
{
    buffer->write_index = 0;
    buffer->count = 0;
}

unsigned char POP_FIFO(struct FIFO * fifo_buf)
{
    unsigned char c;
    if(fifo_buf->end == fifo_buf->start)
        return 0;
    c = fifo_buf->buf[fifo_buf->start];
    if(++fifo_buf->start > FIFOSIZE - 1)
        fifo_buf->start = 0;
    return c;
}

void PUSH_FIFO(struct FIFO * fifo_buf, unsigned char c)
{
    fifo_buf->buf[fifo_buf->end] = c;
    if(++fifo_buf->end > FIFOSIZE - 1)
        fifo_buf->end = 0;
    if(fifo_buf->start == fifo_buf->end)
    {
        if(++fifo_buf->start > FIFOSIZE - 1)
            fifo_buf->end = 0;
    }
}

#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stdint.h>

#define min(x, y) ((x) < (y) ? (x) : (y))
#define ROUND_UP_2(num)  (((num)+1)&~1)
#define DEFAULT_BUF_SIZE (2*1024*1024)

typedef struct cycle_buffer 
{  
    unsigned char* buf;
    unsigned int   size;
    unsigned int   in;
    unsigned int   out;
}RingBuffer;  

RingBuffer *RingBuffer_create(int length);
void RingBuffer_destroy(RingBuffer *buffer);

int RingBuffer_read(RingBuffer *buffer, uint8_t *target,unsigned int amount);
int RingBuffer_write(RingBuffer *buffer, uint8_t *data,unsigned int length);

int RingBuffer_empty(RingBuffer *buffer);
int RingBuffer_Reset(RingBuffer *buffer);

#endif

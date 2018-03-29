#include "ringbuffer.h"
#include <stdlib.h>
#include <string.h>



RingBuffer *RingBuffer_create(int length)
{
    unsigned int size = ROUND_UP_2(length);

    if ( (size&(size-1)) || (size < DEFAULT_BUF_SIZE) )
    {
        size = DEFAULT_BUF_SIZE;
    }
 
    RingBuffer *buffer = (RingBuffer *)malloc(sizeof(RingBuffer));  
    if (!buffer) 
    {
        return NULL; 
    }
    memset(buffer, 0, sizeof(RingBuffer)); 
 
    buffer->size = size;  
    buffer->in   = 0;
    buffer->out  = 0;  
 
    buffer->buf = (unsigned char *)malloc(size);  
    if (!buffer->buf)
    {
        free(buffer);
        return NULL;
    }

    memset(buffer->buf, 0, size);

    return buffer;
}

void RingBuffer_destroy(RingBuffer *buffer)
{
    if(buffer) {
        free(buffer->buf);
        free(buffer);
    }
}

int RingBuffer_Reset(RingBuffer *buffer)
{
    if (buffer == NULL)
    {
        return -1;
    }
     
    buffer->in   = 0;
    buffer->out  = 0;
    memset(buffer->buf, 0, buffer->size);

    return 0;
}

int RingBuffer_empty(RingBuffer *buffer)
{
    return buffer->in == buffer->out;
}

int RingBuffer_write(RingBuffer *buffer, uint8_t *data,unsigned int length)
{
    unsigned int len = 0;

    length = min(length, buffer->size - buffer->in + buffer->out);  
    len    = min(length, buffer->size - (buffer->in & (buffer->size - 1)));

 
    memcpy(buffer->buf + (buffer->in & (buffer->size - 1)), data, len);
    memcpy(buffer->buf, data + len, length - len);
 
    buffer->in += length;
 
    return length;
}

int RingBuffer_read(RingBuffer *buffer, uint8_t *target,unsigned int amount)
{
    unsigned int len = 0;  

    amount = min(amount, buffer->in - buffer->out);
    len    = min(amount, buffer->size - (buffer->out & (buffer->size - 1)));
 
    memcpy(target, buffer->buf + (buffer->out & (buffer->size - 1)), len);
    memcpy(target + len, buffer->buf, amount - len);
 
    buffer->out += amount;
 
    return amount;
}

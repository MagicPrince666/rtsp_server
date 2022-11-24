#include "ringbuffer.h"
#include <new>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RingBuff::RingBuff(uint64_t rbuf)
{
    Create(rbuf);
}

RingBuff::~RingBuff()
{
    Destroy();
}

bool RingBuff::Create(uint64_t length)
{
    uint64_t size = ROUND_UP_2(length);

    if ((size & (size - 1)) || (size < DEFAULT_BUF_SIZE)) {
        size = DEFAULT_BUF_SIZE;
    }

    p_buffer_ = new (std::nothrow) RingBuffer;
    if (!p_buffer_) {
        return false;
    }
    memset(p_buffer_, 0, sizeof(RingBuffer));

    p_buffer_->size = size;
    p_buffer_->in   = 0;
    p_buffer_->out  = 0;

    p_buffer_->buf = new (std::nothrow) uint8_t[size];
    if (!p_buffer_->buf) {
        delete p_buffer_;
        return false;
    }

    memset(p_buffer_->buf, 0, size);

    return true;
}

void RingBuff::Destroy()
{
    if (p_buffer_) {
        delete[] p_buffer_->buf;
        delete p_buffer_;
    }
}

bool RingBuff::Reset()
{
    if (p_buffer_ == nullptr) {
        return false;
    }

    p_buffer_->in  = 0;
    p_buffer_->out = 0;
    // memset(p_buffer_->buf, 0, p_buffer_->size);

    return true;
}

uint64_t RingBuff::Length()
{
    uint64_t len = 0;
    if (p_buffer_->in >= p_buffer_->out) {
        len = p_buffer_->in - p_buffer_->out;
    } else {
        len = p_buffer_->size - p_buffer_->out + p_buffer_->in;
    }
    return len;
}

bool RingBuff::Empty()
{
    return p_buffer_->in == p_buffer_->out;
}

uint64_t RingBuff::Write(uint8_t *data, uint64_t length)
{
    uint64_t len  = 0;
    uint64_t size = Length();
    if (length > size) {
        printf("buf size %ld\n", size);
        Reset();
        printf("Reset buf\n");
    }

    length = Min(length, p_buffer_->size - p_buffer_->in + p_buffer_->out);
    len    = Min(length, p_buffer_->size - (p_buffer_->in & (p_buffer_->size - 1)));

    memcpy(p_buffer_->buf + (p_buffer_->in & (p_buffer_->size - 1)), data, len);
    memcpy(p_buffer_->buf, data + len, length - len);

    p_buffer_->in += length;

    return length;
}

uint64_t RingBuff::Read(uint8_t *target, uint64_t amount)
{
    uint64_t len = 0;

    amount = Min(amount, p_buffer_->in - p_buffer_->out);
    len    = Min(amount, p_buffer_->size - (p_buffer_->out & (p_buffer_->size - 1)));

    memcpy(target, p_buffer_->buf + (p_buffer_->out & (p_buffer_->size - 1)), len);
    memcpy(target + len, p_buffer_->buf, amount - len);

    p_buffer_->out += amount;

    return amount;
}

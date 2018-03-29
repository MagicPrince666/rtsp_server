/*
 * =====================================================================================
 *
 *       Filename:  ring_buffer_class.h
 *        Version:  1.0
 *        Created:  2013年11月28日 13时08分04秒
 *       Revision:  none
 *       Compiler:  clang
 *         Author:  sim szm, xianszm007@gmail.com
 *
 * =====================================================================================
 */
#include <iostream>

class ring_buffer {
public:
    ring_buffer( unsigned char* buffer, unsigned int buffer_size );
    void buffer_data( const void* data, unsigned int& len );
    void get_Data( void* outData, unsigned int& len );
    void skip_data( unsigned int& len );
    inline unsigned int free_space();
    inline unsigned int buffered_bytes();
private:
    void flush_state();
    unsigned char *read_ptr, *write_ptr;
    unsigned char *end_pos;
    unsigned char *buffer;
    int max_read, max_write, buffer_data_;
};
ring_buffer::ring_buffer( unsigned char* buffer, unsigned int buffer_size ) {
    buffer = (unsigned char*)buffer;
    end_pos = buffer + buffer_size;
    read_ptr = write_ptr = buffer;
    max_read = buffer_size;
    max_write = buffer_data_ = 0;
    flush_state();
}
void ring_buffer::buffer_data( const void* data, unsigned int& len ) {
    if ( len > (unsigned int)max_read )
        len = (unsigned int)max_read;
    memcpy( read_ptr, data, len );
    read_ptr += len;
    buffer_data_ += len;
    flush_state();
}
void ring_buffer::get_Data( void* outData, unsigned int& len ) {
    if ( len > (unsigned int)max_write )
        len = (unsigned int)max_write;
    memcpy( outData, write_ptr, len );
    write_ptr += len;
    buffer_data_ -= len;
    flush_state();
}
void ring_buffer::skip_data( unsigned int& len ) {
    unsigned int requestedSkip = len;
    for ( int i=0; i<2; ++i ) {            // 可能会覆盖，做两次
        int skip = (int)len;
        if ( skip > max_write )
            skip = max_write;
        write_ptr += skip;
        buffer_data_ -= skip;
        len -= skip;
        flush_state();
    }
    len = requestedSkip - len;
}
inline unsigned int ring_buffer::free_space() {
    return (unsigned int)max_read;
}
inline unsigned int ring_buffer::buffered_bytes() {
    return (unsigned int)buffer_data_;
}
void ring_buffer::flush_state() {
    if (write_ptr == end_pos)	
	  write_ptr = buffer;
    if (read_ptr == end_pos)		
	  read_ptr = buffer;
    if (read_ptr == write_ptr) {
        if ( buffer_data_ > 0 ) {
            max_read = 0;
            max_write = end_pos - write_ptr;
        } else {
            max_read = end_pos - read_ptr;
            max_write = 0;
        }
    } else if ( read_ptr > write_ptr ) {
        max_read = end_pos - read_ptr;
        max_write = read_ptr - write_ptr;
    } else {
        max_read = write_ptr - read_ptr;
        max_write = end_pos - write_ptr;
    }
}

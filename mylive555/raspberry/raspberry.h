#ifndef __RASPBERRY_H__
#define __RASPBERRY_H__

#define FIFO_NAME "/tmp/ras_fifo"

extern int ras_fd;
void rasclose(int fd);

#endif
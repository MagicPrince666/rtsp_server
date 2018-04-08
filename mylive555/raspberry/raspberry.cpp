#include "raspberry.h"
#include <stdio.h>
#include <unistd.h>

int ras_fd = -1;

void rasclose(int fd)
{
    printf("close %s\n",FIFO_NAME);
    
    // if(fd)
    //     close(fd);
    // fd = -1;
}
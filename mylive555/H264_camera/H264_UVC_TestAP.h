#ifndef __H264_UVC_TESTAP_H_
#define __H264_UVC_TESTAP_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <pthread.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

struct buffer {
	void *         start;
	size_t         length;
};

extern struct buffer *buffers;
extern FILE *rec_fp1;
int Init_264camera(void);
void * Cap_H264_Video (void *arg);
int read_buf(void * opaque,uint8_t *buf, int buf_size);

class FetchData
{
public:
	FetchData();
	virtual ~FetchData();
    
    static void startCap();
    static void stopCap();

    static void EmptyBuffer();
    static int getData(void* fTo, unsigned fMaxSize, unsigned& fFrameSize, unsigned& fNumTruncatedBytes);

public:
    //static cbuf_t data; 

    static void* s_source;

    static void setSource(void* _p)
    {
        s_source = _p;
    }

    static bool s_b_running;
    static int bit_rate_setting(int rate);

};


#endif
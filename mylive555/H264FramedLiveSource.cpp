/*
 * Filename:  H264FramedLiveSource.cpp
 * Auther:  mlj
 * Create date: 2013/ 1/22
 */

#include "H264FramedLiveSource.hh"
#include "h264_camera.h"
#include "ringbuffer.h"
#include "H264_UVC_TestAP.h"
#include "v4l2uvc.h"
#include "h264_xu_ctrls.h"
 
extern RingBuffer* rbuf;
bool start = false;

H264FramedLiveSource::H264FramedLiveSource( UsageEnvironment& env,  
    char const* fileName, 
    unsigned preferredFrameSize, 
    unsigned playTimePerFrame )
    : FramedSource(env)
{
    start = true;
}

H264FramedLiveSource* H264FramedLiveSource::createNew( UsageEnvironment& env,
                                           char const* fileName, 
                                           unsigned preferredFrameSize /*= 0*/, 
                                           unsigned playTimePerFrame /*= 0*/ )
{ 
    H264FramedLiveSource* newSource = new H264FramedLiveSource(env, fileName, preferredFrameSize, playTimePerFrame);
 
    return newSource;
}

H264FramedLiveSource::~H264FramedLiveSource()
{
    printf("close stream\n");
    start = false;
}

extern struct vdIn *vd;
extern struct buffer *buffers;

void H264FramedLiveSource::doGetNextFrame()
{
#if 1
    while(RingBuffer_empty(rbuf))usleep(100);//等待数据
	fFrameSize = RingBuffer_read(rbuf,fTo,fMaxSize);
#else
    int ret;
	struct v4l2_buffer buf;
	//Init_264camera();

	struct timeval tv;
    
	fd_set rfds;
    int retval=0;
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    CLEAR (buf);
        
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    FD_ZERO(&rfds);
    FD_SET(vd->fd, &rfds);
    
    retval=select(vd->fd + 1, &rfds, NULL, NULL, &tv);
    if(retval<0)
    {  
        perror("select error\n");  
    }
    else//有数据要收
    {		
        ret = ioctl(vd->fd, VIDIOC_DQBUF, &buf);
        if (ret < 0) 
        {
            printf("Unable to dequeue buffer!\n");
            exit(1);
        }	  
        
        //fwrite(buffers[buf.index].start, buf.bytesused, 1, rec_fp1);
        fFrameSize = memcpy((buffers[buf.index].start),fTo,buf.bytesused);
        //RingBuffer_write(rbuf,(uint8_t*)(buffers[buf.index].start),buf.bytesused);

        ret = ioctl(vd->fd, VIDIOC_QBUF, &buf);
        
        if (ret < 0) 
        {
            printf("Unable to requeue buffer");
            exit(1);
        }
    }
#endif
    //fFrameSize = fMaxSize;
    nextTask() = envir().taskScheduler().scheduleDelayedTask( 0,
        (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
    return;
}
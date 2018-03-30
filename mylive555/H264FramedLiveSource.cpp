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
extern struct vdIn *vd;

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



void H264FramedLiveSource::doGetNextFrame()
{
#if 0
    while(RingBuffer_empty(rbuf))usleep(100);//等待数据
	fFrameSize = RingBuffer_read(rbuf,fTo,fMaxSize);

    //fFrameSize = fMaxSize;
    nextTask() = envir().taskScheduler().scheduleDelayedTask( 0,
        (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
#else

    int ret;
	struct v4l2_buffer buf;
	//Init_264camera();

    CLEAR (buf);
        
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
		
    ret = ioctl(vd->fd, VIDIOC_DQBUF, &buf);
    if (ret < 0) 
    {
        printf("Unable to dequeue buffer!\n");
        exit(1);
    }	  
    
    //fwrite(buffers[buf.index].start, buf.bytesused, 1, rec_fp1);
    unsigned len = buf.bytesused;

    //拷贝视频到live555缓存
    if(len < fMaxSize)
    {            
        memcpy(fTo, buffers[buf.index].start, len);
        fFrameSize = len;
        fNumTruncatedBytes = 0;
    }        
    else        
    {           
        memcpy(fTo, buffers[buf.index].start, fMaxSize);
        fNumTruncatedBytes = len - fMaxSize; 
        fFrameSize = fMaxSize;
    }
    //RingBuffer_write(rbuf,(uint8_t*)(buffers[buf.index].start),buf.bytesused);

    //fFrameSize = fMaxSize;
    nextTask() = envir().taskScheduler().scheduleDelayedTask( 0,
    (TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数

    ret = ioctl(vd->fd, VIDIOC_QBUF, &buf);
    
    if (ret < 0) 
    {
        printf("Unable to requeue buffer");
        exit(1);
    }
#endif
    
    return;
}

void H264FramedLiveSource::doStopGettingFrames()
{
	//DBGFUNS("H264FramedLiveSource STOP FRAME 1  tid:%d\n",pthread_self());
	//启动获取视频数据线程
	// FetchData::stopCap();
	// emptyBufferFlag = true;
	m_can_get_nextframe = false;

	while(!m_is_queue_empty && m_started)
	{
		// DBGFUNS("H264FramedLiveSource STOP FRAME 2  tid:%d\n",pthread_self());
		usleep(10000);
	}

	//DBGFUNS("H264FramedLiveSource STOP FRAME 2\n");
}

//网络包尺寸，注意尺寸不能太小，否则会崩溃
unsigned int H264FramedLiveSource::maxFrameSize() const
{
	//printf("H264FramedLiveSource::maxFrameSize \n");
	return 150000;
}
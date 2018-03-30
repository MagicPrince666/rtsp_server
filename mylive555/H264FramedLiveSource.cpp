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

extern struct vdIn *vd;

bool emptyBufferFlag = true;

H264FramedLiveSource::H264FramedLiveSource( UsageEnvironment& env)
    : FramedSource(env),m_pToken(0)
{
    m_can_get_nextframe = true;
	m_is_queue_empty =false;
	bVideoFirst = true;
	m_started = false;
    gettimeofday(&sPresentationTime, NULL);

	//启动获取视频数据线程
	emptyBufferFlag = true;
	m_eventTriggerId = envir().taskScheduler().createEventTrigger(H264FramedLiveSource::updateDataNotify);
}


H264FramedLiveSource::~H264FramedLiveSource()
{
    //printf("close stream\n");
    //start = false;
    printf("H264FramedLiveSource::~H264FramedLiveSource() \n");
	
	envir().taskScheduler().deleteEventTrigger(m_eventTriggerId);
}

int timeval_substract(struct timeval* result, struct timeval*t2, struct timeval* t1)
{
	long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);

	result->tv_sec = diff/1000000;
	result->tv_usec = diff % 1000000;

	return diff<0;
}


void  timeval_add(struct timeval* result, struct timeval*t2, struct timeval* t1)
{
	long int total = (t2->tv_usec + 1000000 * t2->tv_sec) + (t1->tv_usec + 1000000 * t1->tv_sec);

	result->tv_sec = total/1000000;
	result->tv_usec = total % 1000000;
}

struct timeval H264FramedLiveSource::sPresentationTime;
struct timeval H264FramedLiveSource::sdiff;
bool H264FramedLiveSource::sbTimeUpdate =false;
void H264FramedLiveSource::updateTime(struct timeval& p)
{
	struct timeval now;
	gettimeofday(&now, NULL);
    sPresentationTime.tv_sec = p.tv_sec;
	sPresentationTime.tv_usec = p.tv_usec;

	int i = timeval_substract(&sdiff, &now, &sPresentationTime);
	printf("DIFF:%d \n",sdiff);

    sbTimeUpdate = true;
}

void H264FramedLiveSource::doUpdateStart()
{
	envir().taskScheduler().triggerEvent(m_eventTriggerId, this);
}


void H264FramedLiveSource::doUpdateDataNotify()
{
	// nextTask() = envir().taskScheduler().scheduleDelayedTask(0,(TaskFunc*)FramedSource::afterGetting,this);  
	afterGetting(this);
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
    if(!m_started)
	{
		m_started =true;
	}
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

    gettimeofday(&fPresentationTime, NULL);
	afterGetting(this);

	if(!m_can_get_nextframe)
	{
		envir().taskScheduler().unscheduleDelayedTask(nextTask());
		// DBGFUNS("Tiam335xH264Source m_is_queue_empty=true tid:%d\n",pthread_self());
		
		m_is_queue_empty=true;
	}
#endif
    
    //return;
}

void H264FramedLiveSource::doStopGettingFrames()
{
	printf("H264FramedLiveSource STOP FRAME 1  \n");

	m_can_get_nextframe = false;

	while(!m_is_queue_empty && m_started)
	{
		// DBGFUNS("Tiam335xH264Source STOP FRAME 2  tid:%d\n",pthread_self());
		usleep(10000);
	}

	printf("H264FramedLiveSource STOP FRAME 2\n");
}

//网络包尺寸，注意尺寸不能太小，否则会崩溃
unsigned int H264FramedLiveSource::maxFrameSize() const
{
	//printf("H264FramedLiveSource::maxFrameSize \n");
	return 150000;
}
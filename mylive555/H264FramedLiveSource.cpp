#include "H264FramedLiveSource.hh"


#if SOFT_H264

#include "h264_camera.h"
#include "ringbuffer.h"
extern RingBuffer* rbuf;

#else

#include "H264_UVC_TestAP.h"

#endif

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
#if SOFT_H264

	printf("Soft wave H264FramedLiveSource::H264FramedLiveSource \n");
	Soft_FetchData::startCap();
	emptyBufferFlag = true;

#else

	printf("Hart wave H264FramedLiveSource::H264FramedLiveSource \n");
	FetchData::startCap();
	emptyBufferFlag = true;
	//FetchData::setSource(this);

#endif

	m_eventTriggerId = envir().taskScheduler().createEventTrigger(H264FramedLiveSource::updateDataNotify);
}


H264FramedLiveSource::~H264FramedLiveSource()
{
    printf("close stream\n");
    printf("H264FramedLiveSource::~H264FramedLiveSource() \n");

#if SOFT_H264
	Soft_FetchData::stopCap();
#else
	FetchData::stopCap();
#endif

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

void H264FramedLiveSource::GetFrameData()
{
	
#if SOFT_H264
	unsigned len = Soft_FetchData::getData(fTo,fMaxSize, fFrameSize, fNumTruncatedBytes);
#else
	unsigned len = FetchData::getData(fTo,fMaxSize, fFrameSize, fNumTruncatedBytes);
#endif

	gettimeofday(&fPresentationTime, NULL);
	afterGetting(this);

	if(!m_can_get_nextframe)
	{
		envir().taskScheduler().unscheduleDelayedTask(nextTask());	
		m_is_queue_empty=true;
	}
			
} 


void H264FramedLiveSource::doGetNextFrame()
{
    if(!m_started)
	{
		m_started =true;
	}
    GetFrameData();
}

void H264FramedLiveSource::doStopGettingFrames()
{
	printf("H264FramedLiveSource STOP FRAME 1  \n");

	m_can_get_nextframe = false;

	while(!m_is_queue_empty && m_started)
	{
		usleep(10000);
	}

	printf("H264FramedLiveSource STOP FRAME 2\n");
}

//网络包尺寸，注意尺寸不能太小，否则会崩溃
unsigned int H264FramedLiveSource::maxFrameSize() const
{
	printf("H264FramedLiveSource::maxFrameSize \n");
	return 150000;
}
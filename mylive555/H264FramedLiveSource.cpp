#include "H264FramedLiveSource.hh"

#if SOFT_H264 == 1
#include "h264_camera.h"
#include "ringbuffer.h"

V4l2H264hData *g_softh264 = nullptr;

#elif SOFT_H264 == 0
#include "H264_UVC_Cap.h"
H264UvcCap *g_h264_uvc_cap;

#elif SOFT_H264 == 2

#include "raspberry.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

FILE *ras_fp = NULL;

#endif

bool emptyBufferFlag = true;

H264FramedLiveSource::H264FramedLiveSource(UsageEnvironment &env)
    : FramedSource(env)
{
    m_can_get_nextframe = true;
    m_is_queue_empty    = false;
    bVideoFirst         = true;
    m_started           = false;
    gettimeofday(&sPresentationTime, NULL);

    //启动获取视频数据线程
#if SOFT_H264 == 1
    g_softh264      = new V4l2H264hData("/dev/video0");
    g_softh264->Init();
    // g_softh264->SetSource(this);
	printf("Soft wave H264FramedLiveSource::H264FramedLiveSource\n");
    emptyBufferFlag = true;

#elif SOFT_H264 == 0

    g_h264_uvc_cap = new H264UvcCap();
    g_h264_uvc_cap->Init();
	printf("Hart wave H264FramedLiveSource::H264FramedLiveSource\n");
    emptyBufferFlag = true;

#elif SOFT_H264 == 2

    system("raspivid -t 0 -w 1920 -h 1080 -fps 30 -b 2000000 -n -o " FIFO_NAME " &");
    // system("raspivid -t 0 -w 1280 -h 720 -fps 30 -b 2000000 -n -o "FIFO_NAME" &");
    printf("raspivid -t 0 -w 1920 -h 1080 -fps 30 -b 2000000 -n -o " FIFO_NAME " &\n");
    // if(ras_fp == NULL)
    //	ras_fp = fopen("rec_h264.264","w");

#endif

    m_eventTriggerId = envir().taskScheduler().createEventTrigger(H264FramedLiveSource::updateDataNotify);
}

H264FramedLiveSource::~H264FramedLiveSource()
{
#if SOFT_H264 == 1
    delete g_softh264;
	printf("H264FramedLiveSource::~H264FramedLiveSource() delete g_softh264\n");
#elif SOFT_H264 == 0
    delete g_h264_uvc_cap;
	printf("H264FramedLiveSource::~H264FramedLiveSource() delete g_h264_uvc_cap\n");
#elif SOFT_H264 == 2

    system("killall -9 raspivid");
    printf("killall -9 raspivid\n");
    // rasclose(ras_fd);

#endif

    envir().taskScheduler().deleteEventTrigger(m_eventTriggerId);
}

int timeval_substract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);

    result->tv_sec  = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return diff < 0;
}

void timeval_add(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int total = (t2->tv_usec + 1000000 * t2->tv_sec) + (t1->tv_usec + 1000000 * t1->tv_sec);

    result->tv_sec  = total / 1000000;
    result->tv_usec = total % 1000000;
}

struct timeval H264FramedLiveSource::sPresentationTime;
struct timeval H264FramedLiveSource::sdiff;
bool H264FramedLiveSource::sbTimeUpdate = false;
void H264FramedLiveSource::updateTime(struct timeval &p)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    sPresentationTime.tv_sec  = p.tv_sec;
    sPresentationTime.tv_usec = p.tv_usec;

    timeval_substract(&sdiff, &now, &sPresentationTime);
    printf("DIFF:%d\n", sdiff);

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
#if SOFT_H264 == 1
    unsigned len = g_softh264->getData(fTo, fMaxSize, fFrameSize, fNumTruncatedBytes);
	printf("GetFrameData size:%d\n", fFrameSize);
#elif SOFT_H264 == 0
    unsigned len = g_h264_uvc_cap->getData(fTo, fMaxSize, fFrameSize, fNumTruncatedBytes);
#elif SOFT_H264 == 2

    // fFrameSize = read(ras_fd,fTo,fMaxSize);

    tv.tv_sec  = 0;
    tv.tv_usec = 33000;
    FD_ZERO(&rfds);
    FD_SET(ras_fd, &rfds);
    retval = select(ras_fd + 1, &rfds, NULL, NULL, &tv);
    if (retval) {
        fFrameSize = read(ras_fd, fTo, fMaxSize);
        // fwrite(fTo,fFrameSize,1,ras_fp);
    }

#endif

    gettimeofday(&fPresentationTime, NULL);
    afterGetting(this);

    if (!m_can_get_nextframe) {
        envir().taskScheduler().unscheduleDelayedTask(nextTask());
        m_is_queue_empty = true;
    }
}

void H264FramedLiveSource::doGetNextFrame()
{
    if (!m_started) {
        m_started = true;
    }
    GetFrameData();
}

void H264FramedLiveSource::doStopGettingFrames()
{
    printf("H264FramedLiveSource STOP FRAME 1\n");

    m_can_get_nextframe = false;

    while (!m_is_queue_empty && m_started) {
        usleep(10000);
    }

    printf("H264FramedLiveSource STOP FRAME 2\n");
}

//网络包尺寸，注意尺寸不能太小，否则会崩溃
unsigned int H264FramedLiveSource::maxFrameSize() const
{
    printf("H264FramedLiveSource::maxFrameSize\n");
    return 150000;
}
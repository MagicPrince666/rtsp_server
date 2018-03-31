/*
 * Filename: H264FramedLiveSource.hh
 * Auther: chenbin
 * Create date: 2013/ 1/22
 */

#ifndef _H264FRAMEDLIVESOURCE_HH
#define _H264FRAMEDLIVESOURCE_HH

#include <FramedSource.hh>

#define SOFT_H264 1  //定义是否使用软件压缩H264

class H264FramedLiveSource : public FramedSource
{
public:
    static H264FramedLiveSource* createNew(UsageEnvironment& env)
    {
      return new H264FramedLiveSource(env);
    }

    H264FramedLiveSource(UsageEnvironment& env);
    virtual ~H264FramedLiveSource();

    static void updateTime(struct timeval& p);
    void doUpdateStart();
    static void updateDataNotify(void* d){((H264FramedLiveSource*)d)->doUpdateDataNotify();};
    void doUpdateDataNotify();

protected:
    virtual void doGetNextFrame();
    virtual void doStopGettingFrames();
    virtual unsigned int maxFrameSize() const; 

    void GetFrameData();
    
    void *m_pToken; 
    static struct timeval sPresentationTime;
    static struct timeval sdiff;

    static bool sbTimeUpdate;
    EventTriggerId m_eventTriggerId;
    bool bVideoFirst;
    bool m_can_get_nextframe;
    bool m_is_queue_empty;
    bool m_started;
};

#endif
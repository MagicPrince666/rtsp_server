/*
 * Filename: H264FramedLiveSource.hh
 * Auther: chenbin
 * Create date: 2013/ 1/22
 */

#ifndef _H264FRAMEDLIVESOURCE_HH
#define _H264FRAMEDLIVESOURCE_HH

#include <FramedSource.hh>


class H264FramedLiveSource : public FramedSource
{
public:
    static H264FramedLiveSource* createNew(UsageEnvironment& env,
        char const* fileName,
        unsigned preferredFrameSize = 0,
        unsigned playTimePerFrame = 0); 

protected:
    H264FramedLiveSource(UsageEnvironment& env,
        char const* fileName, 
        unsigned preferredFrameSize,
        unsigned playTimePerFrame);
    virtual void doStopGettingFrames();
    virtual unsigned int maxFrameSize() const; 
    // called only by createNew()
    ~H264FramedLiveSource();

private:
    // redefined virtual functions:
    virtual void doGetNextFrame();
    int TransportData( unsigned char* to, unsigned maxSize );

protected:
    bool m_can_get_nextframe;
    bool m_is_queue_empty;
    bool m_started;
    FILE *fp;
};

#endif
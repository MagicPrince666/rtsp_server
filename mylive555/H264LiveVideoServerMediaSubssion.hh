#ifndef _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH
#define _H264_LIVE_VIDEO_SERVER_MEDIA_SUBSESSION_HH

#include "OnDemandServerMediaSubsession.hh"

class H264LiveVideoServerMediaSubssion : public OnDemandServerMediaSubsession
{

public:
    static H264LiveVideoServerMediaSubssion *
    createNew(UsageEnvironment &env, FramedSource *);

protected: // we're a virtual base class
    H264LiveVideoServerMediaSubssion(UsageEnvironment &env, FramedSource *);
    ~H264LiveVideoServerMediaSubssion();

protected: // redefined virtual functions
    FramedSource *createNewStreamSource(unsigned clientSessionId, unsigned &estBitrate);
    virtual RTPSink *createNewRTPSink(Groupsock *rtpGroupsock,
                                      unsigned char rtpPayloadTypeIfDynamic, FramedSource *inputSource);

private:
    FramedSource *m_pSource;
    // RTPSink * m_pDummyRTPSink;
};

#endif
#include "H264LiveVideoServerMediaSubssion.hh"
#include "H264FramedLiveSource.hh"
#include "H264VideoRTPSink.hh"
#include "H264VideoStreamDiscreteFramer.hh"

H264LiveVideoServerMediaSubssion *
H264LiveVideoServerMediaSubssion::createNew(UsageEnvironment &env, FramedSource *source)
{
    OutPacketBuffer::maxSize = 521366;
    return new H264LiveVideoServerMediaSubssion(env, source);
}

H264LiveVideoServerMediaSubssion::H264LiveVideoServerMediaSubssion(UsageEnvironment &env, FramedSource *source)
    : OnDemandServerMediaSubsession(env, true)
{
    m_pSource = source;
}

H264LiveVideoServerMediaSubssion::~H264LiveVideoServerMediaSubssion()
{
    printf("H264LiveVideoServerMediaSubssion::destructor__\n");
}

FramedSource *H264LiveVideoServerMediaSubssion::createNewStreamSource(unsigned /*clientSessionId*/, unsigned &estBitrate)
{
    printf("H264LiveVideoServerMediaSubssion::createNewStreamSource__\n");
    estBitrate = 1024000; // kbps, estimate

    //视频真正实现类
    return H264VideoStreamDiscreteFramer::createNew(envir(), H264FramedLiveSource::createNew(envir()));
}

RTPSink *H264LiveVideoServerMediaSubssion ::createNewRTPSink(Groupsock *rtpGroupsock,
                                                             unsigned char rtpPayloadTypeIfDynamic,
                                                             FramedSource * /*inputSource*/)
{
    return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}

#include "H264LiveVideoServerMediaSubssion.hh"
#include "H264FramedLiveSource.hh"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"


#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h> 


#if SOFT_H264 == 1  //软件压缩H264

#include "h264encoder.h"
#include "video_capture.h"
#include "h264_camera.h"
#include "ringbuffer.h"

RingBuffer* rbuf;

#elif SOFT_H264 == 0          //UVC输出H264

#include "H264_UVC_TestAP.h"

#elif SOFT_H264 == 2

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "raspberry.h"

#endif
 

UsageEnvironment* env;



// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
//Boolean reuseFirstSource = False;

// To stream *only* MPEG-1 or 2 video "I" frames
// (e.g., to reduce network bandwidth),
// change the following "False" to "True":
//Boolean iFramesOnly = False;

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
               char const* streamName, char const* inputFileName); // fwd

static char newMatroskaDemuxWatchVariable;
static MatroskaFileServerDemux* demux;
static void onMatroskaDemuxCreation(MatroskaFileServerDemux* newDemux, void* /*clientData*/) {
  demux = newDemux;
  newMatroskaDemuxWatchVariable = 1;
}

static void _signal_handler(int signum)  
{  
    void *array[10];  
    size_t size;  
    char **strings;  
    size_t i;  
  
    signal(signum, SIG_DFL); /* 还原默认的信号处理handler */  
  
    size = backtrace (array, 10);  
    strings = (char **)backtrace_symbols (array, size);  
  
    fprintf(stderr, "widebright received SIGSEGV! Stack trace:\n");  
    for (i = 0; i < size; i++) {  
        fprintf(stderr, "%d %s \n",i,strings[i]);  
    }  
      
    free (strings);  
    exit(1);  
} 

UsageEnvironment* video_env;
static pthread_t video_thread;

//視頻採集
void* video_thread_func(void* param)
{
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  	video_env = BasicUsageEnvironment::createNew(*scheduler);
	ServerMediaSession* s = (ServerMediaSession*)param;

   s->addSubsession(H264LiveVideoServerMediaSubssion
		       ::createNew(*video_env, NULL));

   video_env->taskScheduler().doEventLoop(); // does not return
   return NULL;
}

int main(int argc, char** argv) {

  signal(SIGPIPE, _signal_handler);    // SIGPIPE，管道破裂。
  signal(SIGSEGV, _signal_handler);    // SIGSEGV，非法内存访问
  signal(SIGFPE, _signal_handler);     // SIGFPE，数学相关的异常，如被0除，浮点溢出，等等
  signal(SIGABRT, _signal_handler);    // SIGABRT，由调用abort函数产生，进程非正常退出

  

#if SOFT_H264 == 1

  rbuf = RingBuffer_create(DEFAULT_BUF_SIZE);

  if((pthread_create(&thread[0], NULL, video_Capture_Thread, NULL)) != 0)   
    printf("video_Capture_Thread create fail!\n");

  if((pthread_create(&thread[1], NULL, video_Encode_Thread, NULL)) != 0)  
    printf("video_Encode_Thread create fail!\n");
#elif SOFT_H264 == 2

  remove(FIFO_NAME);
  umask(0);
  if (mkfifo (FIFO_NAME, S_IFIFO | 0666) == -1) {
      perror("mkfifo error!");
      exit(1);
  }
  //mknod(FIFO_NAME, S_IFIFO | 0666, 0);
  // if((mkfifo(FIFO_NAME,O_CREAT|O_EXCL) < 0) && (errno != EEXIST))//创建有名管道,并设置相应的权限
  //     printf("cannot create fifoserver \n");
  
  ras_fd = open(FIFO_NAME, O_RDONLY|O_NONBLOCK);
  if(ras_fd <=0)
  {
    printf("open pipe "FIFO_NAME" error\n");
    exit(1);
  }
	printf("open "FIFO_NAME"\n");

#endif

  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;

#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("prince", "67123236"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

  // Create the RTSP server:
  RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  char const* descriptionString
    = "Session streamed by \"testOnDemandRTSPServer\"";

  // Set up each of the possible streams that can be served by the
  // RTSP server.  Each such stream is implemented using a
  // "ServerMediaSession" object, plus one or more
  // "ServerMediaSubsession" objects for each audio/video substream.

 
  // A H.264 video elementary stream:
  {
    char const* streamName = "stream";

    ServerMediaSession* sms
    = ServerMediaSession::createNew(*env, streamName, streamName,descriptionString);

#if 0
    //修改为自己实现的servermedia  H264LiveVideoServerMediaSubssion
    sms->addSubsession(H264LiveVideoServerMediaSubssion::createNew(*env, NULL)); 
#else
    pthread_create(&video_thread, NULL, video_thread_func,sms); 
#endif                 
    
    
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, NULL);

  }

  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
   *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
  } else {
   *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return

#if SOFT_H264 == 1
  v4l2_close(cam);
  RingBuffer_destroy(rbuf);
#endif

  return 0;
}

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
               char const* streamName, char const* inputFileName) {
  char* url = rtspServer->rtspURL(sms);
  UsageEnvironment& env = rtspServer->envir();
  env << "\n\"" << streamName << "\" stream, from the file \""
    << inputFileName << "\"\n";
  env << "Play this stream using the URL \"" << url << "\"\n";
  delete[] url;
}

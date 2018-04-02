#ifndef __H264_CAMERA_H__
#define __H264_CAMERA_H__

//#include "listop.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define DelayTime 40*1000//(33us*1000=0.05s 30f/s)

#define DEVICE "/dev/video0"
#define SET_WIDTH 640
#define SET_HEIGHT 480
//#define BUFF_SIZE 32768

extern struct cam_data Buff[2];
extern pthread_t thread[3];

//int h264_length=0;
extern int framelength;
extern struct camera *cam;

int read_buffer(void * opaque,uint8_t *buf, int buf_size);
void init(struct cam_data *c);
void *video_Capture_Thread(void *arg);
void *video_Encode_Thread(void *arg);
void thread_create(void);
void thread_wait(void);

class Soft_FetchData
{
public:
	Soft_FetchData();
	virtual ~Soft_FetchData();
    
    static void startCap();
    static void stopCap();

    static void EmptyBuffer();
    static int getData(void* fTo, unsigned fMaxSize, unsigned& fFrameSize, unsigned& fNumTruncatedBytes);

public:

    static void* s_source;

    static void setSource(void* _p)
    {
        s_source = _p;
    }
    static bool s_b_running;

};

#endif
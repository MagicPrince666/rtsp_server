#ifndef __H264_CAMERA_H__
#define __H264_CAMERA_H__

//#include "listop.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define DelayTime 40*1000//(33us*1000=0.05s 30f/s)

#define DEVICE "/dev/video0"
#define SET_WIDTH 640
#define SET_HEIGHT 480
#define BUFF_SIZE 32768

extern struct cam_data Buff[2];
extern pthread_t thread[3];
//int flag[2],point=0;

//int h264_length=0;
extern int framelength;
extern struct camera *cam;

/*
struct score
{
	uint8_t *buf;//缓冲区
	uint32_t len;//缓冲区长度
	struct list_head list;//链表链接域
};
*/
int read_buffer(void * opaque,uint8_t *buf, int buf_size);
void init(struct cam_data *c);
void *video_Capture_Thread(void *arg);
void *video_Encode_Thread(void *arg);
void thread_create(void);
void thread_wait(void);

#endif
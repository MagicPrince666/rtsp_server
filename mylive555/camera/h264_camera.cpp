#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>


#include "h264encoder.h"
#include "video_capture.h"
#include "h264_camera.h"
#include "ringbuffer.h"

static bool s_quit = true; 

struct cam_data Buff[2];

pthread_t thread[3];
int flag[2],point=0;

int h264_length=0;
int framelength=0;
extern Encoder en;
struct camera *cam;

extern char * h264_file_name;
extern FILE *h264_fp;
extern uint8_t *h264_buf;

extern RingBuffer* rbuf;//from ringbuffer.cpp
extern bool start;		//from H164FramedLiveSource.cpp


int record_h264()
{
	cam = (struct camera *) malloc(sizeof(struct camera));
	if (!cam) {
		printf("malloc camera failure!\n");
		exit(1);
	}
	cam->device_name = (char *)DEVICE;
	cam->buffers = NULL;
	cam->width = SET_WIDTH;
	cam->height = SET_HEIGHT;
	cam->fps = 25;

	framelength=sizeof(unsigned char)*cam->width * cam->height * 2;

	v4l2_init(cam);

	init(Buff);

	//创建线程
	printf("Making thread...\n");

	thread_create();

	printf("Waiting for thread...\n");

	thread_wait();

	printf("-----------end program------------");
	v4l2_close(cam);

	return 0;

}


void
init(struct cam_data *c)
{
	flag[0]=flag[1]=0;

	c= (struct cam_data *)malloc(sizeof(struct cam_data ));

	pthread_mutex_init(&c->lock,NULL); //以动态方式创建互斥锁

	pthread_cond_init(&c->captureOK,NULL); //初始化captureOK条件变量

	pthread_cond_init(&c->encodeOK,NULL);//初始化encodeOK条件变量

	c->rpos=0;
	c->wpos=0;
}



void
*video_Capture_Thread(void *arg)
{
	cam = (struct camera *) malloc(sizeof(struct camera));
	if (!cam) {
		printf("malloc camera failure!\n");
		exit(1);
	}

	cam->device_name = (char *)DEVICE;
	cam->buffers = NULL;
	cam->width = SET_WIDTH;
	cam->height = SET_HEIGHT;
	cam->fps = 25;

	framelength=sizeof(unsigned char)*cam->width * cam->height * 2;

	v4l2_init(cam);
  	init(Buff);

	int i=0;

	//unsigned char *data;

	int len=framelength;
	
	struct timeval now;

	struct timespec outtime;

	while(1)
	{
		if(s_quit)
		{
			usleep(10);
			continue;
		}
		usleep(DelayTime);

		gettimeofday(&now, NULL);

		outtime.tv_sec =now.tv_sec;

		outtime.tv_nsec =DelayTime * 1000;

		pthread_mutex_lock(&(Buff[i].lock)); /*获取互斥锁,锁定当前缓冲区*/
		//if(i)   printf("----video_Capture_Thread Buff 1\n");
		//if(!i)   printf("----video_Capture_Thread Buff 0\n");

		while((Buff[i].wpos + len)%BUF_SIZE==Buff[i].rpos && Buff[i].rpos != 0) /*等待缓存区处理操作完成*/
		{
			//printf("***********video_Capture_Thread ************阻塞\n");
			//pthread_cond_wait(&(Buff[i].encodeOK),&(Buff[i].lock));
			pthread_cond_timedwait(&(Buff[i].encodeOK),&(Buff[i].lock),&outtime);
		}


		if(buffOneFrame(&Buff[i] , cam))//采集一帧数据
		{

			pthread_cond_signal(&(Buff[i].captureOK)); /*设置状态信号*/

			pthread_mutex_unlock(&(Buff[i].lock)); /*释放互斥锁*/

			flag[i]=1;//缓冲区i已满

			Buff[i].rpos=0;

			i=!i;	//切换到另一个缓冲区
			
			Buff[i].wpos=0;

			flag[i]=0;//缓冲区i为空
		}

		pthread_cond_signal(&(Buff[i].captureOK)); /*设置状态信号*/

		pthread_mutex_unlock(&(Buff[i].lock)); /*释放互斥锁*/
		
	}
}


void
*video_Encode_Thread(void *arg)
{
	int i=-1;


	while(1)
	{	
		if(s_quit)
		{
			usleep(10);
			continue;
		}

		if((flag[1]==0 && flag[0]==0) || (flag[i]==-1)) continue;

		if(flag[0]==1) i=0;

		if(flag[1]==1) i=1;

		pthread_mutex_lock(&(Buff[i].lock)); /*获取互斥锁*/
		//if(i)   printf("-------------video_Encode_Thread Buff 1\n");
		//if(!i)   printf("-------------video_Encode_Thread Buff 0\n");

		/*H.264压缩视频*/
		//encode_frame(Buff[i].cam_mbuf + Buff[i].rpos,0);
		int h264_length = 0;
		h264_length = compress_frame(&en, -1, Buff[i].cam_mbuf + Buff[i].rpos, h264_buf);

		if (h264_length > 0) {
	
			//printf("%s%d\n","-----------h264_length=",h264_length);
			//写h264文件
			//fwrite(h264_buf, h264_length, 1, h264_fp);

			RingBuffer_write(rbuf,h264_buf,h264_length);
			//printf("buf front:%d rear :%d\n",q.front,q.rear);
		}

		Buff[i].rpos+=framelength;

		if(Buff[i].rpos>=BUF_SIZE) { Buff[i].rpos=0;Buff[!i].rpos=0;flag[i]=-1;}

		/*H.264压缩视频*/
		pthread_cond_signal(&(Buff[i].encodeOK));

		pthread_mutex_unlock(&(Buff[i].lock));/*释放互斥锁*/
	}
}

void
thread_create(void)
{
        int temp;

        memset(&thread, 0, sizeof(thread));  

		/*创建线程*/
        if((temp = pthread_create(&thread[0], NULL, video_Capture_Thread, NULL)) != 0)   
                printf("video_Capture_Thread create fail!\n");

        if((temp = pthread_create(&thread[1], NULL, video_Encode_Thread, NULL)) != 0)  
                printf("video_Encode_Thread create fail!\n");
}

void
thread_wait(void)
{
        /*等待线程结束*/
        if(thread[0] !=0) {  
                pthread_join(thread[0],NULL);
        }
        if(thread[1] !=0) {   
                pthread_join(thread[1],NULL);
        }
}

void Soft_init()
{
  	//v4l2_init(cam);
	//init(Buff);
	printf("Camera init\n");
}

int Soft_uinit()
{	
	//v4l2_close(cam);
	printf("Camera uinit\n");
	return 0;
}

void* Soft_FetchData::s_source = NULL;


static bool emptyBuffer = false;

int Soft_FetchData::getData(void* fTo, unsigned fMaxSize, unsigned& fFrameSize, unsigned& fNumTruncatedBytes)
{
	if(!s_b_running)
	{
		printf("Soft_FetchData::getData s_b_running = false  \n");
		return 0;
	}

	if(!RingBuffer_empty(rbuf))
	fFrameSize = RingBuffer_read(rbuf,(uint8_t*)fTo,fMaxSize);
	fNumTruncatedBytes = 0;

	// //拷贝视频到live555缓存
	// if(len < fMaxSize)
	// {            
	// 	fFrameSize = len;
	// 	fNumTruncatedBytes = 0;
	// }        
	// else        
	// {           
	// 	fNumTruncatedBytes = len - fMaxSize; 
	// 	fFrameSize = fMaxSize;
	// }

	return fFrameSize;
}	

bool Soft_FetchData::s_b_running=false;


void Soft_FetchData::EmptyBuffer()
{
	emptyBuffer = true;
}

void Soft_FetchData::startCap()
{
	s_b_running = true;
	if(!s_quit)
		return;
	s_quit = false;
    Soft_init();
	printf("FetchData startCap\n");   
}

void Soft_FetchData::stopCap()
{
	s_b_running = false;
	
    s_quit = true;

    Soft_uinit();

	printf("FetchData stopCap\n");  
}
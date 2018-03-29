#ifndef _VIDEO_CAPTURE_H
#define _VIDEO_CAPTURE_H

#include <linux/videodev2.h>
#include <pthread.h>
#include <stdint.h>


#define BUF_SIZE 614400
/*C270 YUV 4:2:2 frame size(char)
160*120*2  = 38400
176*144*2  = 50688 
320*176*2  = 112640
320*240*2  = 153600
352*288*2  = 202752
432*240*2  = 207360
544*288*2  = 313344
640*360*2  = 460800
640*480*2  = 614400
752*416*2  = 625664
800*448*2  = 716800
800*600*2  = 960000
864*480*2  = 829440
960*544*2  = 1044480
960*720*2  = 1382400
1024*576*2 = 1179648
1184*656*2 = 1553408
*/
//#define FIFO_NAME "/tmp/my_video.h264"

struct buffer {
	void *start;
	size_t length;
};
struct cam_data{

	unsigned char  cam_mbuf[BUF_SIZE] ;	/*缓存区数组5242880=5MB//缓存区数组10485760=10MB//缓存区数组1536000=1.46484375MB,10f*/
	int wpos; 
	int rpos;	/*写与读的位置*/
	pthread_cond_t captureOK;/*线程采集满一个缓冲区时的标志*/
	pthread_cond_t encodeOK;/*线程编码完一个缓冲区的标志*/
	pthread_mutex_t lock;/*互斥锁*/
                 };

struct camera {
	char *device_name;
	int fd;
	int width;
	int height;
	int fps;
	int display_depth;
	int image_size;
	int frame_number;
	struct v4l2_capability v4l2_cap;
	struct v4l2_cropcap v4l2_cropcap;
	struct v4l2_format v4l2_fmt;
	struct v4l2_crop crop;
	struct buffer *buffers;
};

void errno_exit(const char *s);

int xioctl(int fd, int request, void *arg);

void open_camera(struct camera *cam);
void close_camera(struct camera *cam);

int read_and_encode_frame(struct camera *cam);

void start_capturing(struct camera *cam);
void stop_capturing(struct camera *cam);

void init_camera(struct camera *cam);
void uninit_camera(struct camera *cam);

void init_mmap(struct camera *cam);

void v4l2_init(struct camera *cam);
void v4l2_close(struct camera *cam);

int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);

int buffOneFrame(struct cam_data *tmp , struct camera *cam );
void encode_frame(uint8_t *yuv_frame, size_t yuv_length);

#endif


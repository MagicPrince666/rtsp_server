#ifndef __H264_UVC_TESTAP_H_
#define __H264_UVC_TESTAP_H_

#include <inttypes.h>

extern int capturing;
extern FILE *rec_fp1;
void Init_264camera(void);
void * Cap_H264_Video (void *arg);
int read_buf(void * opaque,uint8_t *buf, int buf_size);

#endif
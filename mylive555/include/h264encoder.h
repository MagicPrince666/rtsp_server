#ifndef _H264ENCODER_H
#define _H264ENCODER_H

#include <stdint.h>
#include <stdio.h>
#include "x264.h"

typedef unsigned char uint8_t;

typedef struct {
	x264_param_t *param;
	x264_t *handle;
	x264_picture_t *picture; //说明一个视频序列中每帧特点
	x264_nal_t *nal;
} Encoder;

//初始化编码器，并返回一个编码器对象
void compress_begin(Encoder *en, int width, int height);

//编码一帧
int compress_frame(Encoder *en, int type, uint8_t *in, uint8_t *out);

//释放内存
void compress_end(Encoder *en);

#endif


/**
 * @file h264encoder.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 编码H264
 * @version 0.1
 * @date 2022-11-18
 * @copyright Copyright (c) {2021} 个人版权所有
 */
#pragma once

#include <stdint.h>
#include <stdio.h>

#include "x264.h"

// #define USE_NV12_FORMAT

enum frametype {
    FRAME_TYPE_P = 0,
    FRAME_TYPE_IDR,
    FRAME_TYPE_I,
    FRAME_TYPE_AUTO
};

class H264Encoder
{
public:
    H264Encoder(int32_t width, int32_t height);
    ~H264Encoder();

    /**
     * @brief 初始化编码器，并返回一个编码器对象
     * @param width 视频宽度
     * @param height 视频高度
     */
    void Init();

    /**
     * @brief 编码一帧
     * @param type 
     * @param in 
     * @param out 
     * @return bool 
     */
    bool CompressFrame(frametype type, uint8_t *in, uint8_t *out, uint64_t &length);

private:
    /**
     * @brief 释放内存
     */
    void UnInit();

    /**
     * @brief 设置编码器参数
     * @param param 
     * @param preset 
     * @return int32_t 
     */
    int32_t X264ParamApplyPreset(x264_param_t *param, const char *preset);

private:
    typedef struct {
        x264_param_t *param;
        x264_t *handle;
        // 说明一个视频序列中每帧特点
        x264_picture_t *picture;
        x264_nal_t *nal;
    } EncoderData;

    int32_t video_width_;
    int32_t video_height_;
    EncoderData encode_;
};
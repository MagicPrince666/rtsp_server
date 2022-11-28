/**
 * @file video_capture.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 获取v4l2视频
 * @version 0.1
 * @date 2022-11-18
 * @copyright Copyright (c) {2021} 个人版权所有
 */
#pragma once

#include <linux/videodev2.h>
#include <pthread.h>
#include <stdint.h>
#include <iostream>

#include "h264encoder.h"

struct Buffer {
    void *start;
    size_t length;
};

struct Camera {
    int32_t fd = -1;
    int32_t width = 640;
    int32_t height = 480;
    int32_t fps = 15;
    struct v4l2_capability v4l2_cap;
    struct v4l2_cropcap v4l2_cropcap;
    struct v4l2_format v4l2_fmt;
    struct v4l2_crop crop;
    struct Buffer *buffers;
};

class V4l2VideoCapture
{
public:
    V4l2VideoCapture(std::string dev = "/dev/video0");
    ~V4l2VideoCapture();

    /**
     * @brief 初始化
     */
    bool Init();

    /**
     * @brief 获取一帧数据
     * @param data 拷贝到此地址
     * @param offset 地址偏移
     * @param maxsize 最大长度
     * @return uint64_t 
     */
    uint64_t BuffOneFrame(uint8_t* data);

    /**
     * 获取帧长度
    */
    int32_t GetFrameLength();

    /**
     * 获取视频格式
    */
    struct Camera* GetFormat();

private:
    /**
     * @brief 关闭v4l2资源
     */
    bool V4l2Close();

    /**
     * @brief 打开摄像头
     */
    bool OpenCamera();

    /**
     * @brief 关闭摄像头
     */
    bool CloseCamera();

    /**
     * @brief 开始录制
     */
    bool StartPreviewing();

    /**
     * @brief 停止录制
     */
    bool StopPreviewing();

    /**
     * @brief 初始化摄像头参数
     */
    bool InitCamera();

    /**
     * @brief 退出设置
     */
    bool UninitCamera();

    /**
     * @brief 初始化mmap
     */
    bool InitMmap();

    void ErrnoExit(const char *s);
    int32_t xioctl(int32_t fd, int32_t request, void *arg);

private:
    std::string v4l2_device_;
    uint32_t n_buffers_ = 0;
    struct Camera camera_;
};

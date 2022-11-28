/**
 * @file H264_UVC_TestAP.h
 * @author 黄李全 (846863428@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-11-18
 * @copyright Copyright (c) {2021} 个人版权所有
 */

#pragma once

#include "h264_xu_ctrls.h"
#include <iostream>
#include <thread>

class H264UvcCap
{
public:
    H264UvcCap(std::string dev = "/dev/video2", uint32_t width = 1280, uint32_t height = 720);
    ~H264UvcCap();

    /**
     * @brief 初始化摄像头
     * @return true
     * @return false
     */
    bool Init(void);

    /**
     * @brief 取数据
     * @return int64_t 数据长度
     */
    int64_t CapVideo();

    /**
     * @brief 开始
     */
    void StartCap();

    /**
     * @brief 停止
     */
    void StopCap();

    /**
     * @brief 设置码率
     * @param rate
     * @return int32_t
     */
    int32_t BitRateSetting(int32_t rate);

    /**
     * @brief 给live555用
     * @param fTo
     * @param fMaxSize
     * @param fFrameSize
     * @param fNumTruncatedBytes
     * @return int32_t
     */
    int32_t getData(void *fTo, unsigned fMaxSize, unsigned &fFrameSize, unsigned &fNumTruncatedBytes);

private:
    /**
     * @brief 打开v4l2设备
     * @return true
     * @return false
     */
    bool OpenDevice();

    /**
     * @brief 创建H264记录文件
     * @param yes 是否创建
     * @return true 创建
     * @return false 不创建
     */
    bool CreateFile(bool yes);

    /**
     * @brief 初始化mmap
     * @return int32_t
     */
    int32_t InitMmap(void);

    /**
     * @brief 解除mmap映射
     * @return true
     * @return false
     */
    bool UninitMmap();

    /**
     * @brief 初始化设备参数
     * @param width 宽度
     * @param height 高度
     * @param format 格式
     * @return int32_t
     */
    int32_t InitDevice(int32_t width, int32_t height, int32_t format);

    /**
     * @brief 开启预览
     * @return int32_t
     */
    int32_t StartPreviewing();

    /**
     * @brief 关闭预览
     * @return true
     * @return false
     */
    bool StopPreviewing();

    /**
     * @brief 抓取H264视频
     */
    void VideoCapThread();

    /**
     * @brief 格式化时间
     * @return std::string 
     */
    std::string getCurrentTime8();

private:
    struct buffer {
        void *start;
        size_t length;
    };
    struct vdIn {
        int32_t fd;
        struct v4l2_capability cap;
        struct v4l2_format fmt;
        struct v4l2_buffer buf;
        uint32_t width = 1280;
        uint32_t height = 720;
        uint32_t fps = 30;
        struct buffer *buffers;
        uint32_t n_buffers = 0;
    };

    struct vdIn *video_;
    H264XuCtrls *h264_xu_ctrls_;
    bool capturing_;
    std::string v4l2_device_;
    uint32_t video_width_;
    uint32_t video_height_;
    FILE *rec_fp1_;
    std::thread cat_h264_thread_;
};

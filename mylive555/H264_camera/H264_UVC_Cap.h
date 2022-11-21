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
#include "v4l2uvc.h"
#include <iostream>

class H264UvcCap
{
public:
    H264UvcCap(std::string dev = "/dev/video0", uint32_t width  = 1280, uint32_t height = 720);
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
     * @return int 
     */
    int InitMmap(void);

    /**
     * @brief 初始化设备参数
     * @param width 宽度
     * @param height 高度
     * @param format 格式
     * @return int 
     */
    int InitDevice(int width, int height, int format);

    /**
     * @brief 开启预览
     * @return int 
     */
    int StartPreviewing(void);

    /**
     * @brief 抓取H264视频
     */
    void VideoCapThread();

private:
    struct buffer {
        void *start;
        size_t length;
    };
    struct buffer *buffers_;
    struct vdIn *video_;

    bool capturing_;
    uint32_t n_buffers_;
    std::string v4l2_device_;
    uint32_t video_width_;
    uint32_t video_height_;
    FILE *rec_fp1_;
    std::thread cat_h264_thread_;
};

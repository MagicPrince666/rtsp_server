/*****************************************************************************************

 * 文件名  H264_UVC_TestAP.cpp
 * 描述    ：录制H264裸流
 * 平台    ：linux
 * 版本    ：V1.0.0
 * 作者    ：Leo Huang  QQ：846863428
 * 邮箱    ：Leo.huang@junchentech.cn
 * 修改时间  ：2017-06-28

*****************************************************************************************/
#include <chrono>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <linux/videodev2.h>
#include <memory>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "H264_UVC_Cap.h"
#include "epoll.h"
#include "ringbuffer.h"
#include "spdlog/cfg/env.h"  // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h" // support for user defined types
#include "spdlog/spdlog.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

H264UvcCap::H264UvcCap(std::string dev, uint32_t width, uint32_t height)
    : v4l2_device_(dev),
      video_width_(width),
      video_height_(height)
{
    capturing_     = false;
    buffers_       = nullptr;
    n_buffers_     = 0;
    rec_fp1_       = nullptr;
    h264_xu_ctrls_ = nullptr;
    video_         = nullptr;
}

H264UvcCap::~H264UvcCap()
{
    if (cat_h264_thread_.joinable()) {
        cat_h264_thread_.join();
    }
    spdlog::info("{} close camera", __FUNCTION__);

    if (rec_fp1_) {
        fclose(rec_fp1_);
    }

    UninitMmap();

    if (h264_xu_ctrls_) {
        delete h264_xu_ctrls_;
    }

    if (video_) {
        if (video_->fd) {
            close(video_->fd);
        }
        delete video_;
    }
    RINGBUF.Reset();
}

int32_t errnoexit(const char *s)
{
    spdlog::error("{} error {}, {}", s, errno, strerror(errno));
    return -1;
}

int32_t xioctl(int32_t fd, int32_t request, void *arg)
{
    int32_t r;
    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

bool H264UvcCap::CreateFile(bool yes)
{
    if (!yes) { // 不创建文件
        return false;
    }
    time_t tt        = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string file = std::to_string(tt) + ".h264";
    rec_fp1_         = fopen(file.c_str(), "wa+");
    if (rec_fp1_) {
        return true;
    }
    spdlog::error("Create file {} fail!!", file);
    return false;
}

bool H264UvcCap::OpenDevice()
{
    spdlog::info("Open device {}", v4l2_device_);
    struct stat st;

    if (-1 == stat(v4l2_device_.c_str(), &st)) {
        spdlog::error("Cannot identify '{}': {}, {}", v4l2_device_, errno, strerror(errno));
        return false;
    }

    if (!S_ISCHR(st.st_mode)) {
        spdlog::error("{} is not a device", v4l2_device_);
        return false;
    }

    video_     = new (std::nothrow) vdIn;
    video_->fd = open(v4l2_device_.c_str(), O_RDWR);

    if (-1 == video_->fd) {
        spdlog::error("Cannot open '{}': {}, {}", v4l2_device_, errno, strerror(errno));
        return false;
    }
    return true;
}

int32_t H264UvcCap::InitMmap(void)
{
    spdlog::info("Init mmap");
    struct v4l2_requestbuffers req;

    CLEAR(req);
    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(video_->fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            spdlog::error("{} does not support memory mapping", v4l2_device_);
            return -1;
        } else {
            return errnoexit("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        spdlog::error("Insufficient buffer memory on {}", v4l2_device_);
        return -1;
    }

    buffers_ = new (std::nothrow) buffer[req.count];

    if (!buffers_) {
        spdlog::error("Out of memory");
        return -1;
    }

    for (n_buffers_ = 0; n_buffers_ < req.count; ++n_buffers_) {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = n_buffers_;

        if (-1 == xioctl(video_->fd, VIDIOC_QUERYBUF, &buf)) {
            return errnoexit("VIDIOC_QUERYBUF");
        }

        buffers_[n_buffers_].length = buf.length;
        buffers_[n_buffers_].start =
            mmap(NULL,
                 buf.length,
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED,
                 video_->fd, buf.m.offset);

        if (MAP_FAILED == buffers_[n_buffers_].start) {
            return errnoexit("mmap");
        }
    }

    return 0;
}

bool H264UvcCap::UninitMmap()
{
    if (!buffers_) {
        return false;
    }

    for (uint32_t i = 0; i < n_buffers_; ++i) {
        if (-1 == munmap(buffers_[i].start, buffers_[i].length)) {
            errnoexit("munmap");
        }
        buffers_[i].start = nullptr;
    }

    delete[] buffers_;
    buffers_ = nullptr;
    return true;
}

int32_t H264UvcCap::InitDevice(int32_t width, int32_t height, int32_t format)
{
    spdlog::info("{} width = {} height = {} format = {}", __FUNCTION__, width, height, format);
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    uint32_t min;

    if (-1 == xioctl(video_->fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            spdlog::error("{} is not a V4L2 device", v4l2_device_);
            return -1;
        } else {
            return errnoexit("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        spdlog::error("{} is no video capture device", v4l2_device_);
        return -1;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        spdlog::error("{} does not support streaming i/o", v4l2_device_);
        return -1;
    }

    CLEAR(cropcap);
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(video_->fd, VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c    = cropcap.defrect;

        if (-1 == xioctl(video_->fd, VIDIOC_S_CROP, &crop)) {
            switch (errno) {
            case EINVAL:
                break;
            default:
                break;
            }
        }
    }

    CLEAR(fmt);

    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = width;
    fmt.fmt.pix.height      = height;
    fmt.fmt.pix.pixelformat = format;
    fmt.fmt.pix.field       = V4L2_FIELD_ANY;

    if (-1 == xioctl(video_->fd, VIDIOC_S_FMT, &fmt)) {
        return errnoexit("VIDIOC_S_FMT");
    }

    min = fmt.fmt.pix.width * 2;

    if (fmt.fmt.pix.bytesperline < min) {
        fmt.fmt.pix.bytesperline = min;
    }
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min) {
        fmt.fmt.pix.sizeimage = min;
    }

    struct v4l2_streamparm parm;
    memset(&parm, 0, sizeof parm);
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(video_->fd, VIDIOC_G_PARM, &parm);
    parm.parm.capture.timeperframe.numerator   = 1;
    parm.parm.capture.timeperframe.denominator = 30;
    ioctl(video_->fd, VIDIOC_S_PARM, &parm);

    return InitMmap();
}

int32_t H264UvcCap::StartPreviewing()
{
    spdlog::info("Start Previewing");
    enum v4l2_buf_type type;

    for (uint32_t i = 0; i < n_buffers_; ++i) {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if (-1 == xioctl(video_->fd, VIDIOC_QBUF, &buf)) {
            return errnoexit("VIDIOC_QBUF");
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(video_->fd, VIDIOC_STREAMON, &type)) {
        return errnoexit("VIDIOC_STREAMON");
    }

    return 0;
}

bool H264UvcCap::StopPreviewing()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(video_->fd, VIDIOC_STREAMOFF, &type)) {
        errnoexit("VIDIOC_STREAMOFF");
    }
    return true;
}

bool H264UvcCap::Init(void)
{
    int32_t format = V4L2_PIX_FMT_H264;

    if (!OpenDevice()) {
        return false;
    }

    if (InitDevice(video_width_, video_height_, format) < 0) {
        return false;
    }

    StartPreviewing();

    h264_xu_ctrls_ = new H264XuCtrls(video_->fd);

    struct tm *tdate;
    time_t curdate;
    tdate = localtime(&curdate);
    h264_xu_ctrls_->XuOsdSetCarcamCtrl(0, 0, 0);
    if (h264_xu_ctrls_->XuOsdSetRTC(tdate->tm_year + 1900, tdate->tm_mon + 1, tdate->tm_mday, tdate->tm_hour, tdate->tm_min, tdate->tm_sec) < 0) {
        spdlog::warn("XU_OSD_Set_RTC_fd = {} Failed", video_->fd);
    }
    if (h264_xu_ctrls_->XuOsdSetEnable(1, 1) < 0) {
        spdlog::warn("XU_OSD_Set_Enable_fd = {} Failed", video_->fd);
    }

    int32_t ret = h264_xu_ctrls_->XuInitCtrl();
    if (ret < 0) {
        spdlog::error("XuH264SetBitRate Failed");
    } else {
        double m_BitRate = 4096 * 1024;
        //设置码率
        if (h264_xu_ctrls_->XuH264SetBitRate(m_BitRate) < 0) {
            spdlog::error("XuH264SetBitRate {} Failed", m_BitRate);
        }

        h264_xu_ctrls_->XuH264GetBitRate(&m_BitRate);
        if (m_BitRate < 0) {
            spdlog::error("XuH264GetBitRate {} Failed", m_BitRate);
        }
    }

    CreateFile(false);

    cat_h264_thread_ = std::thread([](H264UvcCap *p_this) { p_this->VideoCapThread(); }, this);

    spdlog::info("-----Init H264 Camera {}-----", v4l2_device_);
    return true;
}

int64_t H264UvcCap::CapVideo()
{
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    int32_t ret = ioctl(video_->fd, VIDIOC_DQBUF, &buf);
    if (ret < 0) {
        spdlog::error("Unable to dequeue buffer!");
        return -1;
    }

    if (rec_fp1_) {
        fwrite(buffers_[buf.index].start, buf.bytesused, 1, rec_fp1_);
    }

    // spdlog::info("Get buffer size = {}", buf.bytesused);

    RINGBUF.Write((uint8_t *)buffers_[buf.index].start, buf.bytesused);

    ret = ioctl(video_->fd, VIDIOC_QBUF, &buf);

    if (ret < 0) {
        spdlog::error("Unable to requeue buffer");
        return -1;
    }

    return buf.bytesused;
}

int32_t H264UvcCap::BitRateSetting(int32_t rate)
{
    int32_t ret = -1;
    spdlog::info("write to the setting");
    if (!capturing_) //未有客户端接入
    {
        if (video_->fd > 0) { //未初始化不能访问
            ret = h264_xu_ctrls_->XuInitCtrl();
        }
        if (ret < 0) {
            spdlog::info("XuH264SetBitRate Failed");
        } else {
            double m_BitRate = (double)rate;

            if (h264_xu_ctrls_->XuH264SetBitRate(m_BitRate) < 0) {
                spdlog::info("XuH264SetBitRate Failed");
            }

            h264_xu_ctrls_->XuH264GetBitRate(&m_BitRate);
            if (m_BitRate < 0) {
                spdlog::info("XuH264GetBitRate Failed");
            }

            spdlog::info("----m_BitRate:{}----", m_BitRate);
        }
    } else {
        spdlog::info("camera no init\n");
        return -1;
    }
    return ret;
}

int32_t H264UvcCap::getData(void *fTo, unsigned fMaxSize, unsigned &fFrameSize, unsigned &fNumTruncatedBytes)
{
    if (!capturing_) {
        spdlog::warn("V4l2H264hData::getData capturing_ = false");
        return 0;
    }

    if (RINGBUF.Empty()) {
        fFrameSize         = 0;
        fNumTruncatedBytes = 0;
        return 0;
    }

    fFrameSize = RINGBUF.Read((uint8_t *)fTo, fMaxSize);

    fNumTruncatedBytes = 0;
    return fFrameSize;
}

void H264UvcCap::StartCap()
{
    if (!capturing_) {
        MY_EPOLL.EpollAdd(video_->fd, std::bind(&H264UvcCap::CapVideo, this));
    }
    capturing_ = true;
}

void H264UvcCap::StopCap()
{
    if (capturing_) {
        MY_EPOLL.EpollDel(video_->fd);
    }
    capturing_ = false;
    spdlog::info("H264UvcCap StopCap");
}

void H264UvcCap::VideoCapThread()
{
    StartCap();
    while (1) {
        if (!capturing_) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

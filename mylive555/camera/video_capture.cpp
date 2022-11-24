#include <asm/types.h> /* for videodev2.h */
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h> /* low-level i/o */
#include <linux/videodev2.h>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "spdlog/cfg/env.h"  // support for loading levels from the environment variable
#include "spdlog/fmt/ostr.h" // support for user defined types
#include "spdlog/spdlog.h"

#include "video_capture.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

V4l2VideoCapture::V4l2VideoCapture(std::string dev)
    : v4l2_device_(dev)
{
}

V4l2VideoCapture::~V4l2VideoCapture()
{
    V4l2Close();
}

void V4l2VideoCapture::ErrnoExit(const char *s)
{
    spdlog::error("{} error {}, {}", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

int32_t V4l2VideoCapture::xioctl(int32_t fd, int32_t request, void *arg)
{
    int32_t r = 0;
    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

bool V4l2VideoCapture::OpenCamera()
{
    struct stat st;

    if (-1 == stat(v4l2_device_.c_str(), &st)) {
        spdlog::error("Cannot identify '{}': {}, {}", v4l2_device_, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode)) {
        spdlog::error("{} is no device", v4l2_device_);
        exit(EXIT_FAILURE);
    }

    camera_.fd = open(v4l2_device_.c_str(), O_RDWR, 0); //  | O_NONBLOCK

    if (camera_.fd <= 0) {
        spdlog::error("Cannot open '{}': {}, {}", v4l2_device_, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return true;
}

bool V4l2VideoCapture::CloseCamera()
{
    if(camera_.fd <= 0) {
        spdlog::error("{} fd = {}", __FUNCTION__, camera_.fd);
        return false;
    }
    spdlog::info("{} fd = {}", __FUNCTION__, camera_.fd);
    if (-1 == close(camera_.fd)) {
        return false;
    }

    camera_.fd = -1;
    return true;
}

uint64_t V4l2VideoCapture::BuffOneFrame(uint8_t *data)
{
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    // this operator below will change buf.index and (0 <= buf.index <= 3)
    if (-1 == ioctl(camera_.fd, VIDIOC_DQBUF, &buf)) {
        spdlog::error("VIDIOC_DQBUF {}", strerror(errno));
        switch (errno) {
        case EAGAIN:
            return 0;
        case EIO:
            /* Could ignore EIO, see spec. */
            /* fall through */
        default:
            ErrnoExit("VIDIOC_DQBUF");
        }
    }

    uint64_t len = buf.bytesused;

    //把一帧数据拷贝到缓冲区
    memcpy(data, (uint8_t *)(camera_.buffers[buf.index].start), buf.bytesused);

    if (-1 == ioctl(camera_.fd, VIDIOC_QBUF, &buf)) {
        ErrnoExit("VIDIOC_QBUF");
    }

    return len;
}

bool V4l2VideoCapture::StartPreviewing()
{
    enum v4l2_buf_type type;

    for (uint32_t i = 0; i < n_buffers_; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        if (-1 == xioctl(camera_.fd, VIDIOC_QBUF, &buf)) {
            ErrnoExit("VIDIOC_QBUF");
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(camera_.fd, VIDIOC_STREAMON, &type)) {
        ErrnoExit("VIDIOC_STREAMON");
    }
    return true;
}

bool V4l2VideoCapture::StopPreviewing()
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(camera_.fd, VIDIOC_STREAMOFF, &type)) {
        ErrnoExit("VIDIOC_STREAMOFF");
    }
    spdlog::info("{} VIDIOC_STREAMOFF", __FUNCTION__);
    return true;
}

bool V4l2VideoCapture::UninitCamera()
{
    for (uint32_t i = 0; i < n_buffers_; ++i) {
        if (-1 == munmap(camera_.buffers[i].start, camera_.buffers[i].length)) {
            ErrnoExit("munmap");
        }
        camera_.buffers[i].start = nullptr;
    }

    delete[] camera_.buffers;
    spdlog::info("{} munmap", __FUNCTION__);
    return true;
}

bool V4l2VideoCapture::InitMmap()
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    //分配内存
    if (-1 == xioctl(camera_.fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            spdlog::error("{} does not support memory mapping", v4l2_device_);
            exit(EXIT_FAILURE);
        } else {
            ErrnoExit("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        spdlog::error("Insufficient buffer memory on {}", v4l2_device_);
        exit(EXIT_FAILURE);
    }

    camera_.buffers = new (std::nothrow) Buffer[req.count];

    if (!camera_.buffers) {
        spdlog::error("Out of memory");
        exit(EXIT_FAILURE);
    }

    for (n_buffers_ = 0; n_buffers_ < req.count; ++n_buffers_) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = n_buffers_;

        //将VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
        if (-1 == xioctl(camera_.fd, VIDIOC_QUERYBUF, &buf)) {
            ErrnoExit("VIDIOC_QUERYBUF");
        }

        camera_.buffers[n_buffers_].length = buf.length;
        camera_.buffers[n_buffers_].start  = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, camera_.fd, buf.m.offset);

        if (MAP_FAILED == camera_.buffers[n_buffers_].start) {
            ErrnoExit("mmap");
        }
    }
    return true;
}

int32_t V4l2VideoCapture::GetFrameLength()
{
    return camera_.width * camera_.height * 2;
}

struct Camera *V4l2VideoCapture::GetFormat()
{
    return &camera_;
}

bool V4l2VideoCapture::InitCamera()
{
    if(camera_.fd <= 0) {
        spdlog::error("Device = {} fd = {} not init", v4l2_device_, camera_.fd);
        return false;
    }
    struct v4l2_format *fmt = &(camera_.v4l2_fmt);

    CLEAR(*fmt);

    fmt->type           = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt->fmt.pix.width  = camera_.width;
    fmt->fmt.pix.height = camera_.height;
#ifdef USE_NV12_FORMAT
    fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_NV12; // 12  Y/CbCr 4:2:0
    fmt->fmt.pix.field       = V4L2_FIELD_ANY;
#else
    fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;     // 16  YUV 4:2:2
    fmt->fmt.pix.field       = V4L2_FIELD_INTERLACED; //隔行扫描
#endif

    if (-1 == xioctl(camera_.fd, VIDIOC_S_FMT, fmt)) {
        ErrnoExit("VIDIOC_S_FMT");
    }

    if (-1 == xioctl(camera_.fd, VIDIOC_G_FMT, fmt)) {
        ErrnoExit("VIDIOC_G_FMT");
    }

    camera_.width  = fmt->fmt.pix.width;
    camera_.height = fmt->fmt.pix.height;

    struct v4l2_streamparm parm;
    memset(&parm, 0, sizeof(struct v4l2_streamparm));
    parm.type                     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;
    parm.parm.capture.timeperframe.denominator = camera_.fps; //时间间隔分母
    parm.parm.capture.timeperframe.numerator   = 1;           //分子
    if (-1 == ioctl(camera_.fd, VIDIOC_S_PARM, &parm)) {
        perror("set param:");
        exit(EXIT_FAILURE);
    }

    // get message
    if (-1 == xioctl(camera_.fd, VIDIOC_G_PARM, &parm)) {
        ErrnoExit("VIDIOC_G_PARM");
    }

    spdlog::info("Device = {}\t width = {}\t height = {}\t fps = {}",
                 v4l2_device_, fmt->fmt.pix.width, fmt->fmt.pix.height,
                 parm.parm.capture.timeperframe.denominator);

    InitMmap();
    return true;
}

bool V4l2VideoCapture::Init()
{
    bool ret = false;
    ret |= OpenCamera();
    ret |= InitCamera();
    ret |= StartPreviewing();
    return ret;
}

bool V4l2VideoCapture::V4l2Close()
{
    bool ret = false;
    ret |= StopPreviewing();
    ret |= UninitCamera();
    ret |= CloseCamera();
    return ret;
}

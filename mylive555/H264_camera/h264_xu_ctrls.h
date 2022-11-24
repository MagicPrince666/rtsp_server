/**
 * @file h264_xu_ctrls.h
 * @author 黄李全 (846863428@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-11-23
 * @copyright Copyright (c) {2021} 个人版权所有
 */
#pragma once

#include <linux/version.h>
#include <linux/videodev2.h>
#include <stdint.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 0, 0)
#include <linux/uvcvideo.h>
#endif

#define LENGTH_OF_RERVISION_XU_SYS_CTR (7)
#define LENGTH_OF_RERVISION_XU_USR_CTR (9)
#define RERVISION_RER9420_SERIES_CHIPID 0x90
#define RERVISION_RER9422_SERIES_CHIPID 0x92
#define RERVISION_RER9422_DDR_64M 0x00
#define RERVISION_RER9422_DDR_16M 0x03

/*
 * Dynamic controls
 */
// copy from uvcvideo.h
#define UVC_CTRL_DATA_TYPE_RAW 0
#define UVC_CTRL_DATA_TYPE_SIGNED 1
#define UVC_CTRL_DATA_TYPE_UNSIGNED 2
#define UVC_CTRL_DATA_TYPE_BOOLEAN 3
#define UVC_CTRL_DATA_TYPE_ENUM 4
#define UVC_CTRL_DATA_TYPE_BITMASK 5

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#define V4L2_CID_BASE_EXTCTR_RERVISION 0x0A0c4501
#define V4L2_CID_BASE_RERVISION V4L2_CID_BASE_EXTCTR_RERVISION
#define V4L2_CID_ASIC_RW_RERVISION V4L2_CID_BASE_RERVISION + 1
#define V4L2_CID_FLASH_CTRL V4L2_CID_BASE_RERVISION + 2
#define V4L2_CID_FRAME_INFO_RERVISION V4L2_CID_BASE_RERVISION + 3
#define V4L2_CID_H264_CTRL_RERVISION V4L2_CID_BASE_RERVISION + 4
#define V4L2_CID_MJPG_CTRL_RERVISION V4L2_CID_BASE_RERVISION + 5
#define V4L2_CID_OSD_CTRL_RERVISION V4L2_CID_BASE_RERVISION + 6
#define V4L2_CID_MOTION_DETECTION_RERVISION V4L2_CID_BASE_RERVISION + 7
#define V4L2_CID_IMG_SETTING_RERVISION V4L2_CID_BASE_RERVISION + 8
#define V4L2_CID_MULTI_STREAM_CTRL_RERVISION V4L2_CID_BASE_RERVISION + 9
#define V4L2_CID_GPIO_CTRL_RERVISION V4L2_CID_BASE_RERVISION + 10
#define V4L2_CID_DYNAMIC_FPS_CTRL_RERVISION V4L2_CID_BASE_RERVISION + 11
#define V4L2_CID_LAST_EXTCTR_RERVISION V4L2_CID_DYNAMIC_FPS_CTRL_RERVISION

/* ---------------------------------------------------------------------------- */

#define UVC_GUID_RERVISION_SYS_HW_CTRL                                                                 \
    {                                                                                                  \
        0x70, 0x33, 0xf0, 0x28, 0x11, 0x63, 0x2e, 0x4a, 0xba, 0x2c, 0x68, 0x90, 0xeb, 0x33, 0x40, 0x16 \
    }
#define UVC_GUID_RERVISION_USR_HW_CTRL                                                                 \
    {                                                                                                  \
        0x94, 0x73, 0xDF, 0xDD, 0x3E, 0x97, 0x27, 0x47, 0xBE, 0xD9, 0x04, 0xED, 0x64, 0x26, 0xDC, 0x67 \
    }
//#define UVC_GUID_RERVISION_USR_HW_CTRL				{0x3F, 0xAE, 0x12, 0x28, 0xD7, 0xBC, 0x11, 0x4E, 0xA3, 0x57, 0x6F, 0x1E, 0xDE, 0xF7, 0xD6, 0x1D}

#define XU_RERVISION_SYS_ID 0x03
#define XU_RERVISION_USR_ID 0x04

// ----------------------------- XU Control Selector ------------------------------------
#define XU_RERVISION_SYS_ASIC_RW 0x01
#define XU_RERVISION_SYS_FLASH_CTRL 0x03
#define XU_RERVISION_SYS_FRAME_INFO 0x06
#define XU_RERVISION_SYS_H264_CTRL 0x07
#define XU_RERVISION_SYS_MJPG_CTRL 0x08
#define XU_RERVISION_SYS_OSD_CTRL 0x09
#define XU_RERVISION_SYS_MOTION_DETECTION 0x0A
#define XU_RERVISION_SYS_IMG_SETTING 0x0B

#define XU_RERVISION_USR_FRAME_INFO 0x01
#define XU_RERVISION_USR_H264_CTRL 0x02
#define XU_RERVISION_USR_MJPG_CTRL 0x03
#define XU_RERVISION_USR_OSD_CTRL 0x04
#define XU_RERVISION_USR_MOTION_DETECTION 0x05
#define XU_RERVISION_USR_IMG_SETTING 0x06
#define XU_RERVISION_USR_MULTI_STREAM_CTRL 0x07
#define XU_RERVISION_USR_GPIO_CTRL 0x08
#define XU_RERVISION_USR_DYNAMIC_FPS_CTRL 0x09

// ----------------------------- XU Control Selector ------------------------------------
// copy from uvcvideo.h
#define UVC_CONTROL_SET_CUR (1 << 0)
#define UVC_CONTROL_GET_CUR (1 << 1)
#define UVC_CONTROL_GET_MIN (1 << 2)
#define UVC_CONTROL_GET_MAX (1 << 3)
#define UVC_CONTROL_GET_RES (1 << 4)
#define UVC_CONTROL_GET_DEF (1 << 5)
/* Control should be saved at suspend and restored at resume. */
#define UVC_CONTROL_RESTORE (1 << 6)
/* Control can be updated by the camera. */
#define UVC_CONTROL_AUTO_UPDATE (1 << 7)

#define UVC_CONTROL_GET_RANGE (UVC_CONTROL_GET_CUR | UVC_CONTROL_GET_MIN | \
                               UVC_CONTROL_GET_MAX | UVC_CONTROL_GET_RES | \
                               UVC_CONTROL_GET_DEF)

struct uvc_xu_control_info {
    uint8_t entity[16];
    uint8_t index;
    uint8_t selector;
    uint16_t size;
    uint32_t flags;
};

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 0, 0)
struct uvc_xu_control_mapping {
    uint32_t id;
    uint8_t name[32];
    uint8_t entity[16];
    uint8_t selector;

    uint8_t size;
    uint8_t offset;
#if LINUX_VERSION_CODE == KERNEL_VERSION(3, 0, 0) || LINUX_VERSION_CODE == KERNEL_VERSION(3, 0, 35)
    uint32_t v4l2_type;
    uint32_t data_type;

    struct uvc_menu_info __user *menu_info;
    uint32_t menu_count;

    uint32_t reserved[4];
#else
    enum v4l2_ctrl_type v4l2_type;
    uint32_t data_type;
#endif
};
#endif

struct uvc_xu_control {
    uint8_t unit;
    uint8_t selector;
    uint16_t size;
    uint8_t *data;
};

#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 0, 0)
#define UVC_SET_CUR 0x01
#define UVC_GET_CUR 0x81
#define UVCIOC_CTRL_MAP _IOWR('u', 0x20, struct uvc_xu_control_mapping)
#define UVCIOC_CTRL_QUERY _IOWR('u', 0x21, struct uvc_xu_control_query)
#else
#define UVCIOC_CTRL_ADD _IOW('U', 1, struct uvc_xu_control_info)
#define UVCIOC_CTRL_MAP _IOWR('U', 2, struct uvc_xu_control_mapping)
#define UVCIOC_CTRL_GET _IOWR('U', 3, struct uvc_xu_control)
#define UVCIOC_CTRL_SET _IOW('U', 4, struct uvc_xu_control)
#endif

class H264XuCtrls
{
public:
    H264XuCtrls(int32_t fd);
    ~H264XuCtrls();

    int32_t XuInitCtrl();

    int32_t XuOsdSetCarcamCtrl(uint8_t SpeedEn, uint8_t CoordinateEn, uint8_t CoordinateCtrl);
    int32_t XuOsdGetCarcamCtrl(uint8_t *SpeedEn, uint8_t *CoordinateEn, uint8_t *CoordinateCtrl);
    int32_t XuOsdSetSpeed(uint32_t Speed);
    int32_t XuOsdGetSpeed(uint32_t *Speed);

    int32_t XuOsdSetCoordinate1(uint8_t Direction, uint8_t *Vaule);
    int32_t XuOsdSetCoordinate2(uint8_t Direction, uint8_t Vaule1, unsigned long Vaule2, uint8_t Vaule3, unsigned long Vaule4);
    int32_t XuOsdGetCoordinate1(uint8_t *Direction, uint8_t *Vaule);
    int32_t XuOsdGetCoordinate2(uint8_t *Direction, uint8_t *Vaule1, unsigned long *Vaule2, uint8_t *Vaule3, unsigned long *Vaule4);

    int32_t XuOsdSetRTC(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
    int32_t XuOsdSetEnable(uint8_t Enable_Line, uint8_t Enable_Block);
    int32_t XuOsdGetEnable(uint8_t *Enable_Line, uint8_t *Enable_Block);
    int32_t XuH264GetBitRate(double *BitRate);
    int32_t XuH264SetBitRate(double BitRate);

private:
    typedef enum {
        CHIP_NONE    = -1,
        CHIP_RER9420 = 0,
        CHIP_RER9421,
        CHIP_RER9422
    } CHIP_RER942X;

    struct Cur_H264Format {
        int32_t FmtId;
        uint16_t wWidth;
        uint16_t wHeight;
        int32_t FrameRateId;
        uint8_t framerate;
        uint32_t FrameSize;
    };

    struct Multistream_Info {
        uint8_t strm_type;
        uint32_t format;
    };

    struct H264Format {
        uint16_t wWidth;
        uint16_t wHeight;
        int32_t fpsCnt;
        uint32_t FrameSize;
        uint32_t *FrPay; // FrameInterval[0|1]Payloadsize[2|3]
    };

    struct H264Format *H264_fmt_;
    int32_t i_h264_format_count_;
    uint8_t current_fps_;
    uint32_t chip_id_;
    int32_t video_fd_;

private:
    int32_t XuCtrlReadChipID();
    int32_t XuCtrlAdd(struct uvc_xu_control_info *info, struct uvc_xu_control_mapping *map);
    int32_t H264GetFormat();
    int32_t H264CountFormat(uint8_t *Data, int32_t len);
    int32_t H264ParseFormat(uint8_t *Data, int32_t len, struct H264Format *fmt);
    int32_t H264GetFps(uint32_t FrPay);

    int32_t XuSet(struct uvc_xu_control xctrl);
    int32_t XuGet(struct uvc_xu_control *xctrl);
    int32_t XuSetCur(uint8_t xu_unit, uint8_t xu_selector, uint16_t xu_size, uint8_t *xu_data);
    int32_t XuGetCur(uint8_t xu_unit, uint8_t xu_selector, uint16_t xu_size, uint8_t *xu_data);

    int32_t XuH264InitFormat();
    int32_t XuH264GetFormatLength(uint16_t *fwLen);
    int32_t XuH264GetFormatData(uint8_t *fwData, uint16_t fwLen);
    int32_t XuH264SetFormat(struct Cur_H264Format fmt);
    int32_t XuH264GetMode(int32_t *Mode);
    int32_t XuH264SetMode(int32_t Mode);
    int32_t XuH264GetQPLimit(int32_t *QP_Min, int32_t *QP_Max);
    int32_t XuH264GetQP(int32_t *QP_Val);
    int32_t XuH264SetQP(int32_t QP_Val);
    int32_t XuH264SetIFrame();
    int32_t XuH264GetSEI(uint8_t *SEI);
    int32_t XuH264SetSEI(uint8_t SEI);
    int32_t XuH264GetGOP(uint32_t *GOP);
    int32_t XuH264SetGOP(uint32_t GOP);

    int32_t XuAsicRead(uint32_t Addr, uint8_t *AsicData);
    int32_t XuAsicWrite(uint32_t Addr, uint8_t AsicData);

    int32_t XuMultiGetStatus(struct Multistream_Info *status);
    int32_t XuMultiGetInfo(struct Multistream_Info *Info);
    int32_t XuMultiSetType(uint32_t format);
    int32_t XuMultiSetEnable(uint8_t enable);
    int32_t XuMultiGetEnable(uint8_t *enable);
    int32_t XuMultiSetBitRate(uint32_t StreamID, uint32_t BitRate);
    int32_t XuMultiGetBitRate(uint32_t StreamID, uint32_t *BitRate);
    int32_t XuMultiSetQP(uint32_t StreamID, uint32_t QP_Val);
    int32_t XuMultiGetQP(uint32_t StreamID, uint32_t *QP_val);
    int32_t XuMultiSetH264Mode(uint32_t StreamID, uint32_t Mode);
    int32_t XuMultiGetH264Mode(uint32_t StreamID, uint32_t *Mode);
    int32_t XuMultiSetSubStreamFrameRate(uint32_t sub_fps);
    int32_t XuMultiSetSubStreamGOP(uint32_t sub_gop);
    int32_t XuMultiGetSubStreamGOP(uint32_t *sub_gop);

    int32_t XuOsdTimerCtrl(uint8_t enable);
    int32_t XuOsdGetRTC(uint32_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second);
    int32_t XuOsdSetSize(uint8_t LineSize, uint8_t BlockSize);
    int32_t XuOsdGetSize(uint8_t *LineSize, uint8_t *BlockSize);
    int32_t XuOsdSetColor(uint8_t FontColor, uint8_t BorderColor);
    int32_t XuOsdGetColor(uint8_t *FontColor, uint8_t *BorderColor);
    int32_t XuOsdSetAutoScale(uint8_t Enable_Line, uint8_t Enable_Block);
    int32_t XuOsdGetAutoScale(uint8_t *Enable_Line, uint8_t *Enable_Block);
    int32_t XuOsdSetStartPosition(uint8_t OSD_Type, uint32_t RowStart, uint32_t ColStart);
    int32_t XuOsdGetStartPosition(uint32_t *LineRowStart, uint32_t *LineColStart, uint32_t *BlockRowStart, uint32_t *BlockColStart);
    int32_t XuOsdSetMultiSize(uint8_t Stream0, uint8_t Stream1, uint8_t Stream2);
    int32_t XuOsdGetMultiSize(uint8_t *Stream0, uint8_t *Stream1, uint8_t *Stream2);
    int32_t XuOsdSetMSStartPosition(uint8_t StreamID, uint8_t RowStart, uint8_t ColStart);
    int32_t XuOsdGetMSStartPosition(uint8_t *S0_Row, uint8_t *S0_Col, uint8_t *S1_Row, uint8_t *S1_Col, uint8_t *S2_Row, uint8_t *S2_Col);
    int32_t XuOsdSetString(uint8_t group, char *String);
    int32_t XuOsdGetString(uint8_t group, char *String);

    int32_t XuMDSetMode(uint8_t Enable);
    int32_t XuMDGetMode(uint8_t *Enable);
    int32_t XuMDSetThreshold(uint32_t MD_Threshold);
    int32_t XuMDGetThreshold(uint32_t *MD_Threshold);
    int32_t XuMDSetMask(uint8_t *Mask);
    int32_t XuMDGetMask(uint8_t *Mask);
    int32_t XuMDSetResult(uint8_t *Result);
    int32_t XuMDGetResult(uint8_t *Result);

    int32_t XuMJPGSetBitrate(uint32_t MJPG_Bitrate);
    int32_t XuMJPGGetBitrate(uint32_t *MJPG_Bitrate);

    int32_t XuIMGSetMirror(uint8_t Mirror);
    int32_t XuIMGGetMirror(uint8_t *Mirror);

    int32_t XuIMGSetFlip(uint8_t Flip);
    int32_t XuIMGGetFlip(uint8_t *Flip);

    int32_t XuIMGSetColor(uint8_t Color);
    int32_t XuIMGGetColor(uint8_t *Color);

    int32_t XuGPIOCtrlSet(uint8_t GPIO_En, uint8_t GPIO_Value);
    int32_t XuGPIOCtrlGet(uint8_t *GPIO_En, uint8_t *GPIO_OutputValue, uint8_t *GPIO_InputValue);

    int32_t XuFrameDropEnSet(uint8_t Stream1_En, uint8_t Stream2_En);
    int32_t XuFrameDropEnGet(uint8_t *Stream1_En, uint8_t *Stream2_En);
    int32_t XuFrameDropCtrlSet(uint8_t Stream1_fps, uint8_t Stream2_fps);
    int32_t XuFrameDropCtrlGet(uint8_t *Stream1_fps, uint8_t *Stream2_fps);

    int32_t XuSFRead(uint32_t Addr, uint8_t *pData, uint32_t Length);
};

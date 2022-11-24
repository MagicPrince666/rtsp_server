/*****************************************************************************************

 * 文件名  h264_xu_ctrls.cpp
 * 描述    ：uvc摄像头控制
 * 平台    ：linux
 * 版本    ：V1.0.0
 * 作者    ：Leo Huang  QQ：846863428
 * 邮箱    ：Leo.huang@junchentech.cn
 * 修改时间  ：2017-06-28

*****************************************************************************************/
#define N_(x) x

#include "h264_xu_ctrls.h"
#include "debug.h"
#include <errno.h>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define Default_fwLen 13
const uint8_t Default_fwData[Default_fwLen] = {
    0x05, 0x00, 0x02, 0xD0, 0x01, // W=1280, H=720, NumOfFrmRate=1
    0xFF, 0xFF, 0xFF, 0xFF,       // Frame size
    0x07, 0xA1, 0xFF, 0xFF,       // 20
};

static struct uvc_xu_control_info rervision_xu_sys_ctrls[] =
    {
        {
            entity : UVC_GUID_RERVISION_SYS_HW_CTRL,
            index : 0,
            selector : XU_RERVISION_SYS_ASIC_RW,
            size : 4,
            flags : UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_MIN | UVC_CONTROL_GET_MAX |
                UVC_CONTROL_GET_DEF | UVC_CONTROL_AUTO_UPDATE | UVC_CONTROL_GET_CUR
        },
        {
            entity : UVC_GUID_RERVISION_SYS_HW_CTRL,
            index : 1,
            selector : XU_RERVISION_SYS_FRAME_INFO,
            size : 11,
            flags : UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_MIN | UVC_CONTROL_GET_MAX |
                UVC_CONTROL_GET_DEF | UVC_CONTROL_AUTO_UPDATE | UVC_CONTROL_GET_CUR
        },
        {
            entity : UVC_GUID_RERVISION_SYS_HW_CTRL,
            index : 2,
            selector : XU_RERVISION_SYS_H264_CTRL,
            size : 11,
            flags : UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_MIN | UVC_CONTROL_GET_MAX |
                UVC_CONTROL_GET_DEF | UVC_CONTROL_AUTO_UPDATE | UVC_CONTROL_GET_CUR
        },
        {
            entity : UVC_GUID_RERVISION_SYS_HW_CTRL,
            index : 3,
            selector : XU_RERVISION_SYS_MJPG_CTRL,
            size : 11,
            flags : UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR
        },
        {
            entity : UVC_GUID_RERVISION_SYS_HW_CTRL,
            index : 4,
            selector : XU_RERVISION_SYS_OSD_CTRL,
            size : 11,
            flags : UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR
        },
        {
            entity : UVC_GUID_RERVISION_SYS_HW_CTRL,
            index : 5,
            selector : XU_RERVISION_SYS_OSD_CTRL,
            size : 11,
            flags : UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_MIN | UVC_CONTROL_GET_MAX |
                UVC_CONTROL_GET_DEF | UVC_CONTROL_AUTO_UPDATE | UVC_CONTROL_GET_CUR
        },
        {
            entity : UVC_GUID_RERVISION_SYS_HW_CTRL,
            index : 6,
            selector : XU_RERVISION_SYS_MOTION_DETECTION,
            size : 11,
            flags : UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR
        },
        {
            entity : UVC_GUID_RERVISION_SYS_HW_CTRL,
            index : 7,
            selector : XU_RERVISION_SYS_IMG_SETTING,
            size : 11,
            flags : UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR
        },
};

static struct uvc_xu_control_info rervision_xu_usr_ctrls[] =
    {
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 0,
         .selector = XU_RERVISION_USR_FRAME_INFO,
         .size     = 11,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_MIN | UVC_CONTROL_GET_MAX |
                  UVC_CONTROL_GET_DEF | UVC_CONTROL_AUTO_UPDATE | UVC_CONTROL_GET_CUR},
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 1,
         .selector = XU_RERVISION_USR_H264_CTRL,
         .size     = 11,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR},
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 2,
         .selector = XU_RERVISION_USR_MJPG_CTRL,
         .size     = 11,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR},
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 3,
         .selector = XU_RERVISION_USR_OSD_CTRL,
         .size     = 11,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_MIN | UVC_CONTROL_GET_MAX |
                  UVC_CONTROL_GET_DEF | UVC_CONTROL_AUTO_UPDATE | UVC_CONTROL_GET_CUR},
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 4,
         .selector = XU_RERVISION_USR_MOTION_DETECTION,
         .size     = 24,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR},
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 5,
         .selector = XU_RERVISION_USR_IMG_SETTING,
         .size     = 11,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR},
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 6,
         .selector = XU_RERVISION_USR_MULTI_STREAM_CTRL,
         .size     = 11,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR},
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 7,
         .selector = XU_RERVISION_USR_GPIO_CTRL,
         .size     = 11,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR},
        {.entity   = UVC_GUID_RERVISION_USR_HW_CTRL,
         .index    = 8,
         .selector = XU_RERVISION_USR_DYNAMIC_FPS_CTRL,
         .size     = 11,
         .flags    = UVC_CONTROL_SET_CUR | UVC_CONTROL_GET_CUR},
};

//  RERVISION XU system Ctrls Mapping
static struct uvc_xu_control_mapping rervision_xu_sys_mappings[] =
    {
        {V4L2_CID_ASIC_RW_RERVISION,
         "RERVISION: Asic Read",
         UVC_GUID_RERVISION_SYS_HW_CTRL,
         XU_RERVISION_SYS_ASIC_RW,
         4,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_SIGNED},
        {V4L2_CID_FLASH_CTRL,
         "RERVISION: Flash Control",
         UVC_GUID_RERVISION_SYS_HW_CTRL,
         XU_RERVISION_SYS_FLASH_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_SIGNED},
        {V4L2_CID_FRAME_INFO_RERVISION,
         "RERVISION: H264 Format",
         UVC_GUID_RERVISION_SYS_HW_CTRL,
         XU_RERVISION_SYS_FRAME_INFO,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_RAW},
        {V4L2_CID_H264_CTRL_RERVISION,
         "RERVISION: H264 Control",
         UVC_GUID_RERVISION_SYS_HW_CTRL,
         XU_RERVISION_SYS_H264_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_MJPG_CTRL_RERVISION,
         "RERVISION: MJPG Control",
         UVC_GUID_RERVISION_SYS_HW_CTRL,
         XU_RERVISION_SYS_MJPG_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_OSD_CTRL_RERVISION,
         "RERVISION: OSD Control",
         UVC_GUID_RERVISION_SYS_HW_CTRL,
         XU_RERVISION_SYS_OSD_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_RAW},
        {V4L2_CID_MOTION_DETECTION_RERVISION,
         "RERVISION: Motion Detection",
         UVC_GUID_RERVISION_SYS_HW_CTRL,
         XU_RERVISION_SYS_MOTION_DETECTION,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_IMG_SETTING_RERVISION,
         "RERVISION: Image Setting",
         UVC_GUID_RERVISION_SYS_HW_CTRL,
         XU_RERVISION_SYS_IMG_SETTING,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED}};

// RERVISION XU user Ctrls Mapping
static struct uvc_xu_control_mapping rervision_xu_usr_mappings[] =
    {
        {V4L2_CID_FRAME_INFO_RERVISION,
         "RERVISION: H264 Format",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_FRAME_INFO,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_RAW},
        {V4L2_CID_H264_CTRL_RERVISION,
         "RERVISION: H264 Control",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_H264_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_MJPG_CTRL_RERVISION,
         "RERVISION: MJPG Control",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_MJPG_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_OSD_CTRL_RERVISION,
         "RERVISION: OSD Control",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_OSD_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_RAW},
        {V4L2_CID_MOTION_DETECTION_RERVISION,
         "RERVISION: Motion Detection",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_MOTION_DETECTION,
         24,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_IMG_SETTING_RERVISION,
         "RERVISION: Image Setting",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_IMG_SETTING,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_MULTI_STREAM_CTRL_RERVISION,
         "RERVISION: Multi Stram Control ",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_MULTI_STREAM_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_GPIO_CTRL_RERVISION,
         "RERVISION: GPIO Control ",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_GPIO_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED},
        {V4L2_CID_DYNAMIC_FPS_CTRL_RERVISION,
         "RERVISION: Dynamic fps Control ",
         UVC_GUID_RERVISION_USR_HW_CTRL,
         XU_RERVISION_USR_DYNAMIC_FPS_CTRL,
         11,
         0,
         V4L2_CTRL_TYPE_INTEGER,
         UVC_CTRL_DATA_TYPE_UNSIGNED}};

H264XuCtrls::H264XuCtrls(int32_t fd) : video_fd_(fd)
{
    H264_fmt_            = nullptr;
    current_fps_         = 24;
    chip_id_             = CHIP_NONE;
    i_h264_format_count_ = 0;
}

H264XuCtrls::~H264XuCtrls()
{
    if (H264_fmt_) {
        for (int32_t i = 0; i < i_h264_format_count_; i++) {
            if (H264_fmt_[i].FrPay) {
                delete[] H264_fmt_[i].FrPay;
            }
        }
        delete[] H264_fmt_;
    }
}

int32_t H264XuCtrls::XuSetCur(uint8_t xu_unit, uint8_t xu_selector, uint16_t xu_size, uint8_t *xu_data)
{
    int32_t err = 0;
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 0, 36)
    struct uvc_xu_control_query xctrl;
    xctrl.unit     = xu_unit;
    xctrl.selector = xu_selector;
    xctrl.query    = UVC_SET_CUR;
    xctrl.size     = xu_size;
    xctrl.data     = xu_data;
    err            = ioctl(video_fd_, UVCIOC_CTRL_QUERY, &xctrl);
#else
    struct uvc_xu_control xctrl;
    xctrl.unit     = xu_unit;
    xctrl.selector = xu_selector;
    xctrl.size     = xu_size;
    xctrl.data     = xu_data;
    err            = ioctl(video_fd_, UVCIOC_CTRL_SET, &xctrl);
#endif
    return err;
}

int32_t H264XuCtrls::XuGetCur(uint8_t xu_unit, uint8_t xu_selector, uint16_t xu_size, uint8_t *xu_data)
{
    int32_t err = 0;
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 0, 36)
    struct uvc_xu_control_query xctrl;
    xctrl.unit     = xu_unit;
    xctrl.selector = xu_selector;
    xctrl.query    = UVC_GET_CUR;
    xctrl.size     = xu_size;
    xctrl.data     = xu_data;
    err            = ioctl(video_fd_, UVCIOC_CTRL_QUERY, &xctrl);
#else
    struct uvc_xu_control xctrl;
    xctrl.unit     = xu_unit;
    xctrl.selector = xu_selector;
    xctrl.size     = xu_size;
    xctrl.data     = xu_data;
    err            = ioctl(video_fd_, UVCIOC_CTRL_GET, &xctrl);
#endif
    return err;
}

// XU ctrls ----------------------------------------------------------

int32_t H264XuCtrls::XuCtrlAdd(struct uvc_xu_control_info *info, struct uvc_xu_control_mapping *map)
{
    int32_t err = 0;
    /* try to add controls listed */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 0, 36)
    TestAp_Printf(TESTAP_DBG_FLOW, "Adding XU Ctrls - %s\n", map->name);
    if ((err = ioctl(video_fd_, UVCIOC_CTRL_ADD, info)) < 0) {
        if (errno == EEXIST) {
            TestAp_Printf(TESTAP_DBG_ERR, "UVCIOC_CTRL_ADD - Ignored, uvc driver had already defined\n");
            return (-EEXIST);
        } else if (errno == EACCES) {
            TestAp_Printf(TESTAP_DBG_ERR, "Need admin previledges for adding extension unit(XU) controls\n");
            TestAp_Printf(TESTAP_DBG_ERR, "please run 'RERVISION_UVC_TestAP --add_ctrls' as root (or with sudo)\n");
            return (-1);
        } else
            perror("Control exists");
    }
#endif
    /* after adding the controls, add the mapping now */
    TestAp_Printf(TESTAP_DBG_FLOW, "Mapping XU Ctrls - %s\n", map->name);
    if ((err = ioctl(video_fd_, UVCIOC_CTRL_MAP, map)) < 0) {
        if ((errno != EEXIST) && (errno != EACCES)) {
            TestAp_Printf(TESTAP_DBG_ERR, "UVCIOC_CTRL_MAP - Error(err= %d)\n", err);
            return (-2);
        } else if (errno == EACCES) {
            TestAp_Printf(TESTAP_DBG_ERR, "Need admin previledges for adding extension unit(XU) controls\n");
            TestAp_Printf(TESTAP_DBG_ERR, "please run 'RERVISION_UVC_TestAP --add_ctrls' as root (or with sudo)\n");
            return (-1);
        } else
            perror("Mapping exists");
    }

    return 0;
}

int32_t H264XuCtrls::XuInitCtrl()
{
    int32_t i   = 0;
    int32_t err = 0;
    int32_t length;
    struct uvc_xu_control_info *xu_infos;
    struct uvc_xu_control_mapping *xu_mappings;

    // Add xu READ ASIC first
    err = XuCtrlAdd(&rervision_xu_sys_ctrls[i], &rervision_xu_sys_mappings[i]);
    if (err == EEXIST) {
    } else if (err < 0) {
        return err;
    }

    // Read chip ID
    err = XuCtrlReadChipID();
    if (err < 0)
        return err;

    // Add xu flash control
    i++;
    err = XuCtrlAdd(&rervision_xu_sys_ctrls[i], &rervision_xu_sys_mappings[i]);
    if (err == EEXIST) {
    } else if (err < 0) {
        return err;
    }

    // Decide which xu set had been add
    if (chip_id_ == CHIP_RER9420) {
        xu_infos    = rervision_xu_sys_ctrls;
        xu_mappings = rervision_xu_sys_mappings;
        i           = 2;
        length      = LENGTH_OF_RERVISION_XU_SYS_CTR;
        TestAp_Printf(TESTAP_DBG_FLOW, "RER9420\n");
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_infos    = rervision_xu_usr_ctrls;
        xu_mappings = rervision_xu_usr_mappings;
        i           = 0;
        length      = LENGTH_OF_RERVISION_XU_USR_CTR;
        TestAp_Printf(TESTAP_DBG_FLOW, "RER9422\n");
    } else {
        TestAp_Printf(TESTAP_DBG_ERR, "Unknown chip id 0x%x\n", chip_id_);
        return -1;
    }

    // Add other xu accroding chip ID
    for (; i < length; i++) {
        err = XuCtrlAdd(&xu_infos[i], &xu_mappings[i]);
        // if (err < 0) break;
    }
    return 0;
}

int32_t H264XuCtrls::XuCtrlReadChipID()
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuCtrlReadChipID ==>\n");
    int32_t ret = 0;
    int32_t err = 0;
    uint8_t ctrldata[4];

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 3;
    uint8_t xu_selector = XU_RERVISION_SYS_ASIC_RW;
    uint16_t xu_size    = 4;
    uint8_t *xu_data    = ctrldata;

    xu_data[0] = 0x1f;
    xu_data[1] = 0x10;
    xu_data[2] = 0x0;
    xu_data[3] = 0xFF; /* Dummy Write */

    /* Dummy Write */
    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "  ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        return err;
    }

    /* Asic Read */
    xu_data[3] = 0x00;
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "   ioctl(UVCIOC_CTRL_GET) FAILED (%i)  \n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "    Invalid arguments\n");
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuCtrlReadChipID Success == \n");
    TestAp_Printf(TESTAP_DBG_FLOW, "      ASIC READ data[0] : %x\n", xu_data[0]);
    TestAp_Printf(TESTAP_DBG_FLOW, "      ASIC READ data[1] : %x\n", xu_data[1]);
    TestAp_Printf(TESTAP_DBG_FLOW, "      ASIC READ data[2] : %x (Chip ID)\n", xu_data[2]);
    TestAp_Printf(TESTAP_DBG_FLOW, "      ASIC READ data[3] : %x\n", xu_data[3]);

    if (xu_data[2] == RERVISION_RER9420_SERIES_CHIPID) {
        chip_id_ = CHIP_RER9420;
    }
    if (xu_data[2] == RERVISION_RER9422_SERIES_CHIPID) {
        xu_data[0] = 0x07; // DRAM SIZE
        xu_data[1] = 0x16;
        xu_data[2] = 0x0;
        xu_data[3] = 0xFF; /* Dummy Write */

        /* Dummy Write */
        if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
            TestAp_Printf(TESTAP_DBG_ERR, "  ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
            return err;
        }

        /* Asic Read */
        xu_data[3] = 0x00;
        if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
            TestAp_Printf(TESTAP_DBG_ERR, "   ioctl(UVCIOC_CTRL_GET) FAILED (%i)  \n", err);
            if (err == EINVAL)
                TestAp_Printf(TESTAP_DBG_ERR, "    Invalid arguments\n");
            return err;
        }

        if (xu_data[2] == RERVISION_RER9422_DDR_64M) {
            chip_id_ = CHIP_RER9422;
        } else if (xu_data[2] == RERVISION_RER9422_DDR_16M) {
            chip_id_ = CHIP_RER9421;
        }
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "ChipID = %d\n", chip_id_);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuCtrlReadChipID <==\n");
    return ret;
}

int32_t H264XuCtrls::H264GetFormat()
{
    int32_t i, j;
    int32_t success = 1;

    uint8_t *fwData = nullptr;
    uint16_t fwLen  = 0;

    // Init H264 XU Ctrl Format
    if (XuH264InitFormat() < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, " H264 XU Ctrl Format failed , use default Format\n");
        fwLen  = Default_fwLen;
        fwData = new (std::nothrow) uint8_t[fwLen];
        memcpy(fwData, Default_fwData, fwLen);
        goto Skip_XU_GetFormat;
    }

    // Probe : Get format through XU ctrl
    success = XuH264GetFormatLength(&fwLen);
    if (success < 0 || fwLen <= 0) {
        TestAp_Printf(TESTAP_DBG_ERR, " XU Get Format Length failed !\n");
    }
    TestAp_Printf(TESTAP_DBG_FLOW, "fwLen = 0x%x\n", fwLen);
    // alloc memory
    fwData = new (std::nothrow) uint8_t[fwLen];

    if (XuH264GetFormatData(fwData, fwLen) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, " XU Get Format Data failed !\n");
    }

Skip_XU_GetFormat:
    // Get H.264 format count
    i_h264_format_count_ = H264CountFormat(fwData, fwLen);

    TestAp_Printf(TESTAP_DBG_FLOW, "H264GetFormat ==> FormatCount : %d \n", i_h264_format_count_);

    if (i_h264_format_count_ > 0) {
        H264_fmt_ = new (std::nothrow) H264Format[i_h264_format_count_];
    } else {
        TestAp_Printf(TESTAP_DBG_ERR, "Get Resolution Data Failed\n");
    }

    // Parse & Save Size/Framerate into structure
    success = H264ParseFormat(fwData, fwLen, H264_fmt_);

    if (success) {
        for (i = 0; i < i_h264_format_count_; i++) {
            TestAp_Printf(TESTAP_DBG_FLOW, "Format index: %d --- (%d x %d) ---\n", i + 1, H264_fmt_[i].wWidth, H264_fmt_[i].wHeight);
            for (j = 0; j < H264_fmt_[i].fpsCnt; j++) {
                if (chip_id_ == CHIP_RER9420) {
                    TestAp_Printf(TESTAP_DBG_FLOW, "(%d) %2d fps\n", j + 1, H264GetFps(H264_fmt_[i].FrPay[j]));
                } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
                    TestAp_Printf(TESTAP_DBG_FLOW, "(%d) %2d fps\n", j + 1, H264GetFps(H264_fmt_[i].FrPay[j * 2]));
                }
            }
        }
    }

    if (fwData) {
        delete[] fwData;
        fwData = nullptr;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "H264GetFormat <== \n");

    return success;
}

int32_t H264XuCtrls::H264CountFormat(uint8_t *Data, int32_t len)
{
    int32_t fmtCnt     = 0;
    int32_t cur_len    = 0;
    int32_t cur_fpsNum = 0;

    if (Data == NULL || len == 0) {
        return 0;
    }

    // count Format numbers
    while (cur_len < len) {
        cur_fpsNum = Data[cur_len + 4];

        TestAp_Printf(TESTAP_DBG_FLOW, "H264CountFormat ==> cur_len = %d, cur_fpsNum= %d\n", cur_len, cur_fpsNum);

        if (chip_id_ == CHIP_RER9420) {
            cur_len += 9 + cur_fpsNum * 4;
        } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
            cur_len += 9 + cur_fpsNum * 6;
        }

        fmtCnt++;
    }

    if (cur_len != len) {
        TestAp_Printf(TESTAP_DBG_FLOW, "H264CountFormat ==> cur_len = %d, fwLen= %d\n", cur_len, len);
        return 0;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "  ========  fmtCnt=%d   ======== \n", fmtCnt);
    return fmtCnt;
}

int32_t H264XuCtrls::H264ParseFormat(uint8_t *Data, int32_t len, struct H264Format *fmt)
{
    int32_t cur_len    = 0;
    int32_t cur_fmtid  = 0;
    int32_t cur_fpsNum = 0;
    int32_t i;

    while (cur_len < len) {
        // Copy Size
        fmt[cur_fmtid].wWidth    = ((uint16_t)Data[cur_len] << 8) + (uint16_t)Data[cur_len + 1];
        fmt[cur_fmtid].wHeight   = ((uint16_t)Data[cur_len + 2] << 8) + (uint16_t)Data[cur_len + 3];
        fmt[cur_fmtid].fpsCnt    = Data[cur_len + 4];
        fmt[cur_fmtid].FrameSize = ((uint32_t)Data[cur_len + 5] << 24) |
                                   ((uint32_t)Data[cur_len + 6] << 16) |
                                   ((uint32_t)Data[cur_len + 7] << 8) |
                                   ((uint32_t)Data[cur_len + 8]);

        TestAp_Printf(TESTAP_DBG_FLOW, "Data[5~8]: 0x%02x%02x%02x%02x \n", Data[cur_len + 5], Data[cur_len + 6], Data[cur_len + 7], Data[cur_len + 8]);
        TestAp_Printf(TESTAP_DBG_FLOW, "fmt[%d].FrameSize: 0x%08x \n", cur_fmtid, fmt[cur_fmtid].FrameSize);

        // Alloc memory for Frame rate
        cur_fpsNum = Data[cur_len + 4];

        if (chip_id_ == CHIP_RER9420) {
            fmt[cur_fmtid].FrPay = new (std::nothrow) uint32_t[cur_fpsNum];
        } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
            fmt[cur_fmtid].FrPay = new (std::nothrow) uint32_t[cur_fpsNum * 2];
        }

        for (i = 0; i < cur_fpsNum; i++) {
            if (chip_id_ == CHIP_RER9420) {
                fmt[cur_fmtid].FrPay[i] = (uint32_t)Data[cur_len + 9 + i * 4] << 24 |
                                          (uint32_t)Data[cur_len + 9 + i * 4 + 1] << 16 |
                                          (uint32_t)Data[cur_len + 9 + i * 4 + 2] << 8 |
                                          (uint32_t)Data[cur_len + 9 + i * 4 + 3];

            } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
                fmt[cur_fmtid].FrPay[i * 2]     = (uint32_t)Data[cur_len + 9 + i * 6] << 8 | (uint32_t)Data[cur_len + 9 + i * 6 + 1];
                fmt[cur_fmtid].FrPay[i * 2 + 1] = (uint32_t)Data[cur_len + 9 + i * 6 + 2] << 24 |
                                                  (uint32_t)Data[cur_len + 9 + i * 6 + 3] << 16 |
                                                  (uint32_t)Data[cur_len + 9 + i * 6 + 4] << 8 |
                                                  (uint32_t)Data[cur_len + 9 + i * 6 + 5];

                TestAp_Printf(TESTAP_DBG_FLOW, "fmt[cur_fmtid].FrPay[%d]: 0x%04x  0x%08x \n", i, fmt[cur_fmtid].FrPay[i * 2], fmt[cur_fmtid].FrPay[i * 2 + 1]);
            }
        }

        // Do next format
        if (chip_id_ == CHIP_RER9420) {
            cur_len += 9 + cur_fpsNum * 4;
        } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
            cur_len += 9 + cur_fpsNum * 6;
        }
        cur_fmtid++;
    }
    if (cur_len != len) {
        TestAp_Printf(TESTAP_DBG_ERR, "H264ParseFormat <==  fail \n");
        return 0;
    }

    return 1;
}

int32_t H264XuCtrls::H264GetFps(uint32_t FrPay)
{
    int32_t fps = 0;
    if (chip_id_ == CHIP_RER9420) {
        uint16_t frH = (FrPay & 0xFF000000) >> 16;
        uint16_t frL = (FrPay & 0x00FF0000) >> 16;
        uint16_t fr  = (frH | frL);
        fps          = ((uint32_t)10000000 / fr) >> 8;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        fps = ((uint32_t)10000000 / (uint16_t)FrPay) >> 8;
    }

    return fps;
}

int32_t H264XuCtrls::XuH264InitFormat()
{
    int32_t i            = 0;
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_FRAME_INFO;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_FRAME_INFO;
    }

    // Switch command : FMT_NUM_INFO
    // data[0] = 0x01;
    xu_data[0] = 0x9A;
    xu_data[1] = 0x01;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "   Set Switch command : FMT_NUM_INFO FAILED (%i)\n", err);
        return err;
    }

    // xu_data[0] = 0;
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "   ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        return err;
    }

    for (i = 0; i < xu_size; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, " Get Data[%d] = 0x%x\n", i, xu_data[i]);
    }
    TestAp_Printf(TESTAP_DBG_FLOW, " ubH264Idx_S1 = %d\n", xu_data[5]);

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264InitFormat <== Success\n");
    return ret;
}

int32_t H264XuCtrls::XuH264GetFormatLength(uint16_t *fwLen)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetFormatLength ==>\n");
    int32_t i            = 0;
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_FRAME_INFO;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_FRAME_INFO;
    }

    // Switch command : FMT_Data Length_INFO
    xu_data[0] = 0x9A;
    xu_data[1] = 0x02;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) >= 0) {
        // for(i=0; i<11; i++)	xu_data[i] = 0;		// clean data
        memset(xu_data, 0, xu_size);

        if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) >= 0) {
            for (i = 0; i < 11; i += 2)
                TestAp_Printf(TESTAP_DBG_FLOW, " Get Data[%d] = 0x%x\n", i, (xu_data[i] << 8) + xu_data[i + 1]);

            // Get H.264 format length
            *fwLen = ((uint16_t)xu_data[6] << 8) + xu_data[7];
            TestAp_Printf(TESTAP_DBG_FLOW, " H.264 format Length = 0x%x\n", *fwLen);
        } else {
            TestAp_Printf(TESTAP_DBG_ERR, "   ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
            return err;
        }
    } else {
        TestAp_Printf(TESTAP_DBG_ERR, "   Set Switch command : FMT_Data Length_INFO FAILED (%i)\n", err);
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetFormatLength <== Success\n");
    return ret;
}

int32_t H264XuCtrls::XuH264GetFormatData(uint8_t *fwData, uint16_t fwLen)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetFormatData ==>\n");
    int32_t i            = 0;
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    int32_t loop     = 0;
    int32_t LoopCnt  = (fwLen % 11) ? (fwLen / 11) + 1 : (fwLen / 11);
    int32_t Copyleft = fwLen;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_FRAME_INFO;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_FRAME_INFO;
    }

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x03; // FRM_DATA_INFO

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "   Set Switch command : FRM_DATA_INFO FAILED (%i)\n", err);
        return err;
    }

    // Get H.264 Format Data
    xu_data[0] = 0x02; // Stream: 1
    xu_data[1] = 0x01; // Format: 1 (H.264)

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) >= 0) {
        // Read all H.264 format data
        for (loop = 0; loop < LoopCnt; loop++) {
            for (i = 0; i < 11; i++) {
                xu_data[i] = 0; // clean data
            }

            TestAp_Printf(TESTAP_DBG_FLOW, "--> Loop : %d <--\n", loop);

            if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) >= 0) {
                for (i = 0; i < 11; i++) {
                    TestAp_Printf(TESTAP_DBG_FLOW, " Data[%d] = 0x%x\n", i, xu_data[i]);
                }

                // Copy Data
                if (Copyleft >= 11) {
                    memcpy(&fwData[loop * 11], xu_data, 11);
                    Copyleft -= 11;
                } else {
                    memcpy(&fwData[loop * 11], xu_data, Copyleft);
                    Copyleft = 0;
                }
            } else {
                TestAp_Printf(TESTAP_DBG_ERR, "   ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
                return err;
            }
        }
    } else {
        TestAp_Printf(TESTAP_DBG_ERR, "   Set Switch command : FRM_DATA_INFO FAILED (%i)\n", err);
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetFormatData <== Success\n");
    return ret;
}

int32_t H264XuCtrls::XuH264SetFormat(struct Cur_H264Format fmt)
{
    // Need to get H264 format first
    if (H264_fmt_ == nullptr) {
        TestAp_Printf(TESTAP_DBG_FLOW, "RERVISION_UVC_TestAP @XuH264SetFormat : Do XU_H264_GetFormat before setting H264 format\n");
        return -EINVAL;
    }

    if (chip_id_ == CHIP_RER9420) {
        TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetFormat ==> %d-%d => (%d x %d):%d fps\n",
                      fmt.FmtId + 1, fmt.FrameRateId + 1,
                      H264_fmt_[fmt.FmtId].wWidth,
                      H264_fmt_[fmt.FmtId].wHeight,
                      H264GetFps(H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId]));
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetFormat ==> %d-%d => (%d x %d):%d fps\n",
                      fmt.FmtId + 1, fmt.FrameRateId + 1,
                      H264_fmt_[fmt.FmtId].wWidth,
                      H264_fmt_[fmt.FmtId].wHeight,
                      H264GetFps(H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId * 2]));
    }

    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_FRAME_INFO;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_FRAME_INFO;
    }

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x21; // Commit_INFO

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XU_H264_Set_FMT ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set H.264 format setting data
    xu_data[0] = 0x02;          // Stream : 1
    xu_data[1] = 0x01;          // Format index : 1 (H.264)
    xu_data[2] = fmt.FmtId + 1; // Frame index (Resolution index), firmware index starts from 1
                                //	xu_data[3] = ( H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId] & 0xFF000000 ) >> 24;	// Frame interval
                                //	xu_data[4] = ( H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId] & 0x00FF0000 ) >> 16;
    xu_data[5] = (H264_fmt_[fmt.FmtId].FrameSize & 0x00FF0000) >> 16;
    xu_data[6] = (H264_fmt_[fmt.FmtId].FrameSize & 0x0000FF00) >> 8;
    xu_data[7] = (H264_fmt_[fmt.FmtId].FrameSize & 0x000000FF);
    //	xu_data[8] = ( H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId] & 0x0000FF00 ) >> 8;
    //	xu_data[9] = ( H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId] & 0x000000FF ) ;

    if (chip_id_ == CHIP_RER9420) {
        xu_data[3] = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId] & 0xFF000000) >> 24; // Frame interval
        xu_data[4] = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId] & 0x00FF0000) >> 16;
        xu_data[8] = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId] & 0x0000FF00) >> 8;
        xu_data[9] = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId] & 0x000000FF);
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_data[3]  = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId * 2] & 0x0000FF00) >> 8; // Frame interval
        xu_data[4]  = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId * 2] & 0x000000FF);
        xu_data[8]  = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId * 2 + 1] & 0xFF000000) >> 24;
        xu_data[9]  = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId * 2 + 1] & 0x00FF0000) >> 16;
        xu_data[10] = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId * 2 + 1] & 0x0000FF00) >> 8;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XU_H264_Set_FMT ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        memset(xu_data, 0, xu_size);
        xu_data[0] = (H264_fmt_[fmt.FmtId].FrPay[fmt.FrameRateId * 2 + 1] & 0x000000FF);

        if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
            TestAp_Printf(TESTAP_DBG_ERR, "XU_H264_Set_FMT____2 ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
            if (err == EINVAL) {
                TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
            }
            return err;
        }
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetFormat <== Success \n");
    return ret;
}

int32_t H264XuCtrls::XuH264GetMode(int32_t *Mode)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetMode ==>\n");
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01; // H264_ctrl_type
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x06; // H264_mode
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetMode ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get mode
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetMode ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuH264GetMode Success == \n");
    TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", 0, xu_data[0]);

    *Mode = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetMode (%s)<==\n", *Mode == 1 ? "CBR mode" : (*Mode == 2 ? "VBR mode" : "error"));
    return ret;
}

int32_t H264XuCtrls::XuH264SetMode(int32_t Mode)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetMode (0x%x) ==>\n", Mode);
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01; // H264_ctrl_type
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x06; // H264_mode
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetMode ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set CBR/VBR Mode
    memset(xu_data, 0, xu_size);
    xu_data[0] = Mode;
    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetMode ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetMode <== Success \n");
    return ret;
}

int32_t H264XuCtrls::XuH264GetQPLimit(int32_t *QP_Min, int32_t *QP_Max)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetQPLimit ==>\n");
    int32_t i            = 0;
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02; // H264_limit
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01; // H264_limit
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetQPLimit ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }
    memset(xu_data, 0, xu_size);
    // Get QP value
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetQPLimit ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuH264GetQPLimit Success == \n");
    for (i = 0; i < 2; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *QP_Min = xu_data[0];
    *QP_Max = xu_data[1];
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetQPLimit (0x%x, 0x%x)<==\n", *QP_Min, *QP_Max);
    return ret;
}

int32_t H264XuCtrls::XuH264GetQP(int32_t *QP_Val)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetQP ==>\n");
    //	int32_t i = 0;
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};
    *QP_Val              = -1;

    if (chip_id_ == CHIP_RER9420) {
        int32_t qp_min, qp_max;
        XuH264GetQPLimit(&qp_min, &qp_max);
    }

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x05; // H264_VBR_QP
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x07; // H264_QP
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetQP ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_FLOW, "Invalid arguments\n");
        }
        return err;
    }

    // Get QP value
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetQP ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuH264GetQP Success == \n");
    TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", 0, xu_data[0]);

    *QP_Val = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetQP (0x%x)<==\n", *QP_Val);
    return ret;
}

int32_t H264XuCtrls::XuH264SetQP(int32_t QP_Val)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetQP (0x%x) ==>\n", QP_Val);
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x05; // H264_VBR_QP
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_H264_CTRL;
        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x07; // H264_QP
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetQP ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set QP value
    memset(xu_data, 0, xu_size);
    xu_data[0] = QP_Val;
    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetQP ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetQP <== Success \n");
    return ret;
}

int32_t H264XuCtrls::XuH264GetBitRate(double *BitRate)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetBitRate ==>\n");
    int32_t i               = 0;
    int32_t ret             = 0;
    int32_t err             = 0;
    uint8_t ctrldata[11]    = {0};
    int32_t BitRate_CtrlNum = 0;
    *BitRate                = -1.0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_H264_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x03; // H264_BitRate
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_H264_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02; // H264_BitRate
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetBitRate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get Bit rate ctrl number
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetBitRate ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuH264GetBitRate Success == \n");

    if (chip_id_ == CHIP_RER9420) {
        for (i = 0; i < 2; i++) {
            TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
        }

        BitRate_CtrlNum = (xu_data[0] << 8) | (xu_data[1]);

        // Bit Rate = BitRate_Ctrl_Num*512*fps*8 /1000(Kbps)
        *BitRate = (double)(BitRate_CtrlNum * 512.0 * current_fps_ * 8) / 1024.0;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        for (i = 0; i < 3; i++) {
            TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
        }

        BitRate_CtrlNum = (xu_data[0] << 16) | (xu_data[1] << 8) | (xu_data[2]);

        *BitRate = BitRate_CtrlNum;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetBitRate (%.2f)<==\n", *BitRate);
    return ret;
}

int32_t H264XuCtrls::XuH264SetBitRate(double BitRate)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetBitRate (%.2f) ==>\n", BitRate);
    int32_t ret             = 0;
    int32_t err             = 0;
    uint8_t ctrldata[11]    = {0};
    int32_t BitRate_CtrlNum = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_H264_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x03; // H264_BitRate
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_H264_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02; // H264_BitRate
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetBitRate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set Bit Rate Ctrl Number
    if (chip_id_ == CHIP_RER9420) {
        // Bit Rate = BitRate_Ctrl_Num*512*fps*8/1000 (Kbps)
        BitRate_CtrlNum = (int32_t)((BitRate * 1024) / (512 * current_fps_ * 8));
        xu_data[0]      = (BitRate_CtrlNum & 0xFF00) >> 8; // BitRate ctrl Num
        xu_data[1]      = (BitRate_CtrlNum & 0x00FF);
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        // Bit Rate = BitRate_Ctrl_Num*512*fps*8/1000 (Kbps)
        xu_data[0] = ((int32_t)BitRate & 0x00FF0000) >> 16;
        xu_data[1] = ((int32_t)BitRate & 0x0000FF00) >> 8;
        xu_data[2] = ((int32_t)BitRate & 0x000000FF);
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetBitRate ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetBitRate <== Success \n");
    return ret;
}

int32_t H264XuCtrls::XuH264SetIFrame()
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetIFrame ==>\n");
    int32_t ret          = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};
    //	int32_t BitRate_CtrlNum = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_H264_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x06; // H264_IFRAME
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_H264_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x04; // H264_IFRAME
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetIFrame ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set IFrame reset
    xu_data[0] = 1;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetIFrame ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetIFrame <== Success \n");
    return ret;
}

int32_t H264XuCtrls::XuH264GetSEI(uint8_t *SEI)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetSEI ==>\n");
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_H264_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        TestAp_Printf(TESTAP_DBG_FLOW, " ==SN9C290 no support get SEI==\n");
        return 0;
    }

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x05; // H264_SEI

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetSEI ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get SEI
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetSEI ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *SEI = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "      SEI : 0x%x\n", *SEI);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetSEI <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuH264SetSEI(uint8_t SEI)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetSEI ==>\n");
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_H264_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        TestAp_Printf(TESTAP_DBG_FLOW, " ==SN9C290 no support Set SEI==\n");
        return 0;
    }

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x05; // H264_SEI

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetSEI ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set SEI
    xu_data[0] = SEI;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetSEI ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetSEI <== Success \n");
    return 0;
}

int32_t H264XuCtrls::XuH264GetGOP(uint32_t *GOP)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetGOP ==>\n");
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_H264_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x03; // H264_GOP

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetGOP ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get GOP
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264GetGOP ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return err;
    }

    *GOP = (xu_data[1] << 8) | xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "      GOP : %d\n", *GOP);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264GetGOP <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuH264SetGOP(uint32_t GOP)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetGOP ==>\n");
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_H264_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x03; // H264_GOP

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetGOP ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set GOP
    xu_data[0] = (GOP & 0xFF);
    xu_data[1] = (GOP >> 8) & 0xFF;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuH264SetGOP ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuH264SetGOP <== Success \n");
    return 0;
}

int32_t H264XuCtrls::XuGet(struct uvc_xu_control *xctrl)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XU Get ==>\n");
    int32_t i   = 0;
    int32_t ret = 0;
    int32_t err = 0;

    // XU Set
    if ((err = XuSetCur(xctrl->unit, xctrl->selector, xctrl->size, xctrl->data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XU Get ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // XU Get
    if ((err = XuGetCur(xctrl->unit, xctrl->selector, xctrl->size, xctrl->data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XU Get ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XU Get Success == \n");
    for (i = 0; i < xctrl->size; i++)
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xctrl->data[i]);
    return ret;
}

int32_t H264XuCtrls::XuSet(struct uvc_xu_control xctrl)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XU Set ==>\n");
    int32_t i   = 0;
    int32_t ret = 0;
    int32_t err = 0;

    // XU Set
    for (i = 0; i < xctrl.size; i++)
        TestAp_Printf(TESTAP_DBG_FLOW, "      Set data[%d] : 0x%x\n", i, xctrl.data[i]);

    if ((err = XuSetCur(xctrl.unit, xctrl.selector, xctrl.size, xctrl.data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XU Set ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }
    TestAp_Printf(TESTAP_DBG_FLOW, "   == XU Set Success == \n");
    return ret;
}

int32_t H264XuCtrls::XuAsicRead(uint32_t Addr, uint8_t *AsicData)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuAsicRead ==>\n");
    int32_t ret = 0;
    uint8_t ctrldata[4];

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 3;
    uint8_t xu_selector = XU_RERVISION_SYS_ASIC_RW;
    uint16_t xu_size    = 4;
    uint8_t *xu_data    = ctrldata;

    xu_data[0] = (Addr & 0xFF);
    xu_data[1] = ((Addr >> 8) & 0xFF);
    xu_data[2] = 0x0;
    xu_data[3] = 0xFF; /* Dummy Write */

    /* Dummy Write */
    if ((ret = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "   ioctl(UVCIOC_CTRL_SET) FAILED (%i) \n", ret);
        if (ret == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "    Invalid arguments\n");
        }
        return ret;
    }

    /* Asic Read */
    xu_data[3] = 0x00;
    if ((ret = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "   ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", ret);
        if (ret == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "    Invalid arguments\n");
        }
        return ret;
    }
    *AsicData = xu_data[2];
    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuAsicRead Success ==\n");
    TestAp_Printf(TESTAP_DBG_FLOW, "      Address:0x%x = 0x%x \n", Addr, *AsicData);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuAsicRead <== Success\n");
    return ret;
}

int32_t H264XuCtrls::XuAsicWrite(uint32_t Addr, uint8_t AsicData)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuAsicWrite ==>\n");
    int32_t ret = 0;
    uint8_t ctrldata[4];

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 3;
    uint8_t xu_selector = XU_RERVISION_SYS_ASIC_RW;
    uint16_t xu_size    = 4;
    uint8_t *xu_data    = ctrldata;

    xu_data[0] = (Addr & 0xFF);        /* Addr Low */
    xu_data[1] = ((Addr >> 8) & 0xFF); /* Addr High */
    xu_data[2] = AsicData;
    xu_data[3] = 0x0; /* Normal Write */

    /* Normal Write */
    if ((ret = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "   ioctl(UVCIOC_CTRL_SET) FAILED (%i) \n", ret);
        if (ret == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "    Invalid arguments\n");
        }
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuAsicWrite <== %s\n", (ret < 0 ? "Failed" : "Success"));
    return ret;
}

int32_t H264XuCtrls::XuMultiGetStatus(struct Multistream_Info *status)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetStatus ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01; // Multi-Stream Status
    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetStatus ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get status
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetStatus ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return err;
    }

    status->strm_type = xu_data[0];
    status->format    = (xu_data[1] << 8) | (xu_data[2]);

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuMultiGetStatus Success == \n");
    TestAp_Printf(TESTAP_DBG_FLOW, "      Get strm_type %d\n", status->strm_type);
    TestAp_Printf(TESTAP_DBG_FLOW, "      Get cur_format %d\n", status->format);

    return 0;
}

int32_t H264XuCtrls::XuMultiGetInfo(struct Multistream_Info *Info)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetInfo ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x02; // Multi-Stream Stream Info
    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetInfo ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get Info
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetInfo ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return err;
    }

    Info->strm_type = xu_data[0];
    Info->format    = (xu_data[1] << 8) | (xu_data[2]);

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuMultiGetInfo Success == \n");
    TestAp_Printf(TESTAP_DBG_FLOW, "      Get Support Stream %d\n", Info->strm_type);
    TestAp_Printf(TESTAP_DBG_FLOW, "      Get Support Resolution 0x%x\n", Info->format);

    return 0;
}

int32_t H264XuCtrls::XuMultiSetType(uint32_t format)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetType (%d) ==>\n", format);

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x02;
    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetType ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    if (chip_id_ == CHIP_RER9421 && (format == 4 || format == 8 || format == 16)) {
        TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return -1;
    }

    // Set format
    xu_data[0] = format;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetType ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetType <== Success \n");
    return 0;
}

int32_t H264XuCtrls::XuMultiSetEnable(uint8_t enable)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetEnable ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};
    //	int32_t BitRate_CtrlNum = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x03; // Enable Multi-Stream Flag

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetEnable ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set enable / disable multistream
    xu_data[0] = enable;
    xu_data[1] = 0;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetEnable ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "Set H264_Multi_Enable = %d \n", (xu_data[0] & 0x01));
    TestAp_Printf(TESTAP_DBG_FLOW, "Set MJPG_Multi_Enable = %d \n", ((xu_data[0] >> 1) & 0x01));
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetEnable <== Success \n");
    return 0;
}

int32_t H264XuCtrls::XuMultiGetEnable(uint8_t *enable)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetEnable ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x03; // Enable Multi-Stream Flag
    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetEnable ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get Enable
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetEnable ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *enable = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "      Get H264 Multi Stream Enable = %d\n", xu_data[0] & 0x01);
    TestAp_Printf(TESTAP_DBG_FLOW, "      Get MJPG Multi Stream Enable =  %d\n", (xu_data[0] >> 1) & 0x01);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetEnable <== Success\n");
    return 0;
}

#if 0
int32_t H264XuCtrls::XuMultiSetBitRate(uint32_t BitRate1, uint32_t BitRate2, uint32_t BitRate3)
{	
	TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetBitRate  BiteRate1=%d  BiteRate2=%d  BiteRate3=%d   ==>\n",BitRate1, BitRate2, BitRate3);

	int32_t err = 0;
	uint8_t ctrldata[11]={0};
	int32_t BitRate_CtrlNum = 0;

	//uvc_xu_control parmeters
	uint8_t xu_unit= XU_RERVISION_USR_ID; 
	uint8_t xu_selector= XU_RERVISION_USR_MULTI_STREAM_CTRL;
	uint16_t xu_size= 11;
	uint8_t *xu_data= ctrldata;

	// Switch command
	xu_data[0] = 0x9A;
	xu_data[1] = 0x04;

	if ((err=XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) 
	{
		TestAp_Printf(TESTAP_DBG_ERR,"XuMultiSetBitRate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n",err);
		if(err==EINVAL) {
			TestAp_Printf(TESTAP_DBG_ERR,"Invalid arguments\n");
        }
		return err;
	}

	// Set BitRate1~3  (unit:bps)
	xu_data[0] = (BitRate1 >> 16)&0xFF;
	xu_data[1] = (BitRate1 >> 8)&0xFF;
	xu_data[2] = BitRate1&0xFF;
	xu_data[3] = (BitRate2 >> 16)&0xFF;
	xu_data[4] = (BitRate2 >> 8)&0xFF;
	xu_data[5] = BitRate2&0xFF;
	xu_data[6] = (BitRate3 >> 16)&0xFF;
	xu_data[7] = (BitRate3 >> 8)&0xFF;
	xu_data[8] = BitRate3&0xFF;
	
	if ((err=XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
		TestAp_Printf(TESTAP_DBG_ERR,"XuMultiSetBitRate ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n",err);
		if(err==EINVAL) {
			TestAp_Printf(TESTAP_DBG_ERR,"Invalid arguments\n");
        }
		return err;
	}

	TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetBitRate <== Success \n");
	return 0;
}

int32_t H264XuCtrls::XuMultiGetBitRate(int32_t fd)
{	
	TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetBitRate  ==>\n");

	int32_t i = 0;
	int32_t err = 0;
	int32_t BitRate1 = 0;
	int32_t BitRate2 = 0;
	int32_t BitRate3 = 0;
	uint8_t ctrldata[11]={0};

	//uvc_xu_control parmeters
	uint8_t xu_unit= XU_RERVISION_USR_ID; 
	uint8_t xu_selector= XU_RERVISION_USR_MULTI_STREAM_CTRL;
	uint16_t xu_size= 11;
	uint8_t *xu_data= ctrldata;

	// Switch command
	xu_data[0] = 0x9A;
	xu_data[1] = 0x04;

	if ((err=XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) 
	{
		TestAp_Printf(TESTAP_DBG_ERR,"XuMultiGetBitRate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n",err);
		if(err==EINVAL) {
			TestAp_Printf(TESTAP_DBG_ERR,"Invalid arguments\n");
        }
		return err;
	}

	// Get BitRate1~3  (unit:bps)
	memset(xu_data, 0, xu_size);
	if ((err=XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
		TestAp_Printf(TESTAP_DBG_ERR,"XuMultiGetBitRate ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n",err);
		if(err==EINVAL) {
			TestAp_Printf(TESTAP_DBG_ERR,"Invalid arguments\n");
        }
		return err;
	}

	TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetBitRate <== Success \n");
	
	for(i=0; i<9; i++) {
		TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

	BitRate1 =  ( xu_data[0]<<16 )| ( xu_data[1]<<8 )| (xu_data[2]) ;
	BitRate2 =  ( xu_data[3]<<16 )| ( xu_data[4]<<8 )| (xu_data[5]) ;
	BitRate3 =  ( xu_data[6]<<16 )| ( xu_data[7]<<8 )| (xu_data[8]) ;
	
	TestAp_Printf(TESTAP_DBG_FLOW, "  HD BitRate (%d)\n", BitRate1);
	TestAp_Printf(TESTAP_DBG_FLOW, "  QVGA BitRate (%d)\n", BitRate2);
	TestAp_Printf(TESTAP_DBG_FLOW, "  QQVGA BitRate (%d)\n", BitRate3);

	return 0;
}
#endif

int32_t H264XuCtrls::XuMultiSetBitRate(uint32_t StreamID, uint32_t BitRate)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetBitRate  StreamID=%d  BiteRate=%d  ==>\n", StreamID, BitRate);

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};
    //	int32_t BitRate_CtrlNum = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x04;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetBitRate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set BitRate  (unit:bps)
    xu_data[0] = StreamID;
    xu_data[1] = (BitRate >> 16) & 0xFF;
    xu_data[2] = (BitRate >> 8) & 0xFF;
    xu_data[3] = BitRate & 0xFF;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetBitRate ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetBitRate <== Success \n");
    return 0;
}

int32_t H264XuCtrls::XuMultiGetBitRate(uint32_t StreamID, uint32_t *BitRate)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetBitRate  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x05;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetBitRate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set Stream ID
    xu_data[0] = StreamID;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetBitRate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get BitRate (unit:bps)
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetBitRate ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetBitRate <== Success \n");

    for (i = 0; i < 4; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *BitRate = (xu_data[1] << 16) | (xu_data[2] << 8) | xu_data[3];
    TestAp_Printf(TESTAP_DBG_FLOW, "  Stream= %d   BitRate= %d\n", xu_data[0], *BitRate);

    return 0;
}

int32_t H264XuCtrls::XuMultiSetQP(uint32_t StreamID, uint32_t QP_Val)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetQP  StreamID=%d  QP_Val=%d  ==>\n", StreamID, QP_Val);

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};
    //	int32_t BitRate_CtrlNum = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x05;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetQP ==> Switch cmd(5) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set Stream ID
    xu_data[0] = StreamID;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetQP ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x06;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetQP ==> Switch cmd(6) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set QP
    xu_data[0] = StreamID;
    xu_data[1] = QP_Val;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetQP ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetQP <== Success \n");
    return 0;
}

int32_t H264XuCtrls::XuMultiGetQP(uint32_t StreamID, uint32_t *QP_val)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetQP  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x05;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetQP ==> Switch cmd(5) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set Stream ID
    xu_data[0] = StreamID;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetQP ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x06;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetQP ==> Switch cmd(6) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get QP
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetQP ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetQP <== Success \n");

    for (i = 0; i < 2; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *QP_val = xu_data[1];
    TestAp_Printf(TESTAP_DBG_FLOW, "  Stream= %d   QP_val = %d\n", xu_data[0], *QP_val);

    return 0;
}

int32_t H264XuCtrls::XuMultiSetH264Mode(uint32_t StreamID, uint32_t Mode)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetH264Mode  StreamID=%d  Mode=%d  ==>\n", StreamID, Mode);

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};
    //	int32_t BitRate_CtrlNum = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x07;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetH264Mode ==> Switch cmd(7) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set Stream ID
    xu_data[0] = StreamID;
    xu_data[1] = Mode;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiSetH264Mode ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiSetH264Mode <== Success \n");
    return 0;
}

int32_t H264XuCtrls::XuMultiGetH264Mode(uint32_t StreamID, uint32_t *Mode)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetH264Mode  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x05;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetH264Mode ==> Switch cmd(5) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set Stream ID
    xu_data[0] = StreamID;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetH264Mode ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x07;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetH264Mode ==> Switch cmd(7) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get H264 Mode
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetH264Mode ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetH264Mode <== Success \n");

    for (i = 0; i < 2; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *Mode = xu_data[1];
    TestAp_Printf(TESTAP_DBG_FLOW, "  Stream= %d   Mode = %d\n", xu_data[0], *Mode);

    return 0;
}

int32_t H264XuCtrls::XuMultiSetSubStreamGOP(uint32_t sub_gop)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XU_Multi_Set_SubStream_GOP sub_gop=%d  ==>\n", sub_gop);

    int32_t err = 0, main_gop = 0;
    uint8_t ctrldata[11] = {0};
    //    int32_t BitRate_CtrlNum = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    XuH264GetGOP((uint32_t *)(&main_gop));

    if (sub_gop > (uint32_t)main_gop) {
        TestAp_Printf(TESTAP_DBG_ERR, "set sub_gop as %d, because sub_gop must less than or equal to main_gop\n", main_gop);
        sub_gop = main_gop;
    }

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x09;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XU_Multi_Set_SubStream_GOP ==> Switch cmd(9) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set sub_gop
    xu_data[0] = sub_gop & 0xff;
    xu_data[1] = (sub_gop & 0xff00) >> 8;
    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XU_Multi_Set_SubStream_GOP ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XU_Multi_Set_SubStream_GOP <== Success \n");
    return 0;
}

int32_t H264XuCtrls::XuMultiGetSubStreamGOP(uint32_t *sub_gop)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetSubStreamGOP  ==>\n");

    //	int32_t i = 0;
    int32_t err          = 0, main_gop, get_gop;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MULTI_STREAM_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    XuH264GetGOP((uint32_t *)(&main_gop));

    // Switch command
    xu_data[0] = 0x9A;
    xu_data[1] = 0x09;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetSubStreamGOP ==> Switch cmd(9) : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get substream gop
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMultiGetSubStreamGOP ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMultiGetSubStreamGOP <== Success \n");
    get_gop  = (xu_data[1] << 8) | xu_data[0];
    *sub_gop = (get_gop < main_gop ? get_gop : main_gop);
    TestAp_Printf(TESTAP_DBG_FLOW, "sub_fps = min(%d, %d)\n", get_gop, main_gop);

    return 0;
}

int32_t H264XuCtrls::XuOsdTimerCtrl(uint8_t enable)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdTimerCtrl  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x00; // OSD Timer control

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdTimerCtrl ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set enable / disable timer count
    xu_data[0] = enable;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdTimerCtrl ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdTimerCtrl <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetRTC(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetRTC  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01; // OSD RTC control

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetRTC ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set RTC
    xu_data[0] = second;
    xu_data[1] = minute;
    xu_data[2] = hour;
    xu_data[3] = day;
    xu_data[4] = month;
    xu_data[5] = (year & 0xFF00) >> 8;
    xu_data[6] = (year & 0x00FF);

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetRTC ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 7; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Set data[%d] : 0x%x\n", i, xu_data[i]);
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetRTC <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetRTC(uint32_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetRTC  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01; // OSD RTC control

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetRTC ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetRTC ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 7; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetRTC <== Success \n");

    *year   = (xu_data[5] << 8) | xu_data[6];
    *month  = xu_data[4];
    *day    = xu_data[3];
    *hour   = xu_data[2];
    *minute = xu_data[1];
    *second = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, " year 	= %d \n", *year);
    TestAp_Printf(TESTAP_DBG_FLOW, " month	= %d \n", *month);
    TestAp_Printf(TESTAP_DBG_FLOW, " day 	= %d \n", *day);
    TestAp_Printf(TESTAP_DBG_FLOW, " hour 	= %d \n", *hour);
    TestAp_Printf(TESTAP_DBG_FLOW, " minute	= %d \n", *minute);
    TestAp_Printf(TESTAP_DBG_FLOW, " second	= %d \n", *second);

    return 0;
}

int32_t H264XuCtrls::XuOsdSetSize(uint8_t LineSize, uint8_t BlockSize)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetSize  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x02; // OSD Size control

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetSize ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return err;
    }

    if (LineSize > 4)
        LineSize = 4;

    if (BlockSize > 4)
        BlockSize = 4;

    // Set data
    xu_data[0] = LineSize;
    xu_data[1] = BlockSize;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetSize ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetSize <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetSize(uint8_t *LineSize, uint8_t *BlockSize)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetSize  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x02; // OSD Size control

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetSize ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetSize ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 2; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *LineSize  = xu_data[0];
    *BlockSize = xu_data[1];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Size (Line) = %d\n", *LineSize);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Size (Block) = %d\n", *BlockSize);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetSize <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetColor(uint8_t FontColor, uint8_t BorderColor)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetColor  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x03; // OSD Color control

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetColor ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    if (FontColor > 4) {
        FontColor = 4;
    }

    if (BorderColor > 4) {
        BorderColor = 4;
    }

    // Set data
    xu_data[0] = FontColor;
    xu_data[1] = BorderColor;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetColor ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetColor <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetColor(uint8_t *FontColor, uint8_t *BorderColor)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetColor  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x03; // OSD Color control

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetColor ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetColor ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 2; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *FontColor   = xu_data[0];
    *BorderColor = xu_data[1];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Font Color = %d\n", *FontColor);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Border Color = %d\n", *BorderColor);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetColor <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetEnable(uint8_t Enable_Line, uint8_t Enable_Block)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetEnable  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x04; // OSD enable

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetEnable ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return err;
    }

    // Set data
    xu_data[0] = Enable_Line;
    xu_data[1] = Enable_Block;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetEnable ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetEnable <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetEnable(uint8_t *Enable_Line, uint8_t *Enable_Block)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetEnable  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x04; // OSD Enable

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetEnable ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetEnable ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 2; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *Enable_Line  = xu_data[0];
    *Enable_Block = xu_data[1];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Enable Line = %d\n", *Enable_Line);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Enable Block = %d\n", *Enable_Block);

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetEnable <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetAutoScale(uint8_t Enable_Line, uint8_t Enable_Block)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetAutoScale  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x05; // OSD Auto Scale enable

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetAutoScale ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Enable_Line;
    xu_data[1] = Enable_Block;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetAutoScale ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetAutoScale <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetAutoScale(uint8_t *Enable_Line, uint8_t *Enable_Block)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetAutoScale  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x05; // OSD Auto Scale enable

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetAutoScale ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetAutoScale ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 2; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *Enable_Line  = xu_data[0];
    *Enable_Block = xu_data[1];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Enable Line  Auto Scale = %d\n", *Enable_Line);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Enable Block Auto Scale = %d\n", *Enable_Block);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetAutoScale <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetMultiSize(uint8_t Stream0, uint8_t Stream1, uint8_t Stream2)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetMultiSize  %d   %d   %d  ==>\n", Stream0, Stream1, Stream2);

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x06;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetMultiSize ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Stream0;
    xu_data[1] = Stream1;
    xu_data[2] = Stream2;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetMultiSize ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetMultiSize <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetMultiSize(uint8_t *Stream0, uint8_t *Stream1, uint8_t *Stream2)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetMultiSize  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x06;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetMultiSize ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetMultiSize ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 3; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *Stream0 = xu_data[0];
    *Stream1 = xu_data[1];
    *Stream2 = xu_data[2];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Multi Stream 0 Size = %d\n", *Stream0);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Multi Stream 1 Size = %d\n", *Stream1);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Multi Stream 2 Size = %d\n", *Stream2);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetMultiSize <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetStartPosition(uint8_t OSD_Type, uint32_t RowStart, uint32_t ColStart)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetStartPosition  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x08; // OSD Start Position

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetStartPosition ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    if (OSD_Type > 3) {
        OSD_Type = 0;
    }

    // Set data
    xu_data[0] = OSD_Type;
    xu_data[1] = (RowStart & 0xFF00) >> 8; // unit 16 lines
    xu_data[2] = RowStart & 0x00FF;
    xu_data[3] = (ColStart & 0xFF00) >> 8; // unit 16 pixels
    xu_data[4] = ColStart & 0x00FF;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetStartPosition ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetStartPosition <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetStartPosition(uint32_t *LineRowStart, uint32_t *LineColStart, uint32_t *BlockRowStart, uint32_t *BlockColStart)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetStartPosition  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x08; // OSD Start Position

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetStartPosition ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetStartPosition ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 8; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *LineRowStart  = (xu_data[0] << 8) | xu_data[1];
    *LineColStart  = (xu_data[2] << 8) | xu_data[3];
    *BlockRowStart = (xu_data[4] << 8) | xu_data[5];
    *BlockColStart = (xu_data[6] << 8) | xu_data[7];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Line Start Row =%d * 16lines\n", *LineRowStart);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Line Start Col =%d * 16pixels\n", *LineColStart);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Block Start Row =%d * 16lines\n", *BlockRowStart);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Block Start Col =%d * 16pixels\n", *BlockColStart);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetStartPosition <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetMSStartPosition(uint8_t StreamID, uint8_t RowStart, uint8_t ColStart)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetMSStartPosition  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x09; // OSD MS Start Position

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetMSStartPosition ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = StreamID;
    xu_data[1] = RowStart & 0x00FF;
    xu_data[2] = ColStart & 0x00FF;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetMSStartPosition ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetMSStartPosition  %d %d %d<== Success \n", StreamID, RowStart, ColStart);

    return 0;
}

int32_t H264XuCtrls::XuOsdGetMSStartPosition(uint8_t *S0_Row, uint8_t *S0_Col, uint8_t *S1_Row, uint8_t *S1_Col, uint8_t *S2_Row, uint8_t *S2_Col)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetMSStartPosition  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x09; // OSD MS Start Position

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetMSStartPosition ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetMSStartPosition ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 6; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
    }

    *S0_Row = xu_data[0];
    *S0_Col = xu_data[1];
    *S1_Row = xu_data[2];
    *S1_Col = xu_data[3];
    *S2_Row = xu_data[4];
    *S2_Col = xu_data[5];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Stream0 Start Row = %d * 16lines\n", *S0_Row);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Stream0 Start Col = %d * 16pixels\n", *S0_Col);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Stream1 Start Row = %d * 16lines\n", *S1_Row);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Stream1 Start Col = %d * 16pixels\n", *S1_Col);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Stream2 Start Row = %d * 16lines\n", *S2_Row);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Stream2 Start Col = %d * 16pixels\n", *S2_Col);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetMSStartPosition <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetString(uint8_t group, char *String)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetString  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x07;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetString ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = 1;
    xu_data[1] = group;

    for (i = 0; i < 8; i++)
        xu_data[i + 2] = String[i];

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetString ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetString <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetString(uint8_t group, char *String)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetString  ==>\n");

    int32_t i            = 0;
    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x07;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetString ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set read mode
    xu_data[0] = 0;
    xu_data[1] = group;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetString ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetString ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }
    group = xu_data[1];

    TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[0] : 0x%x\n", xu_data[0]);
    TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[1] : 0x%x\n", group);

    for (i = 0; i < 8; i++) {
        String[i] = xu_data[i + 2];
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i + 2, String[i]);
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD String = %s \n", String);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetString <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMDSetMode(uint8_t Enable)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDSetMode  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[24] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MOTION_DETECTION;
    uint16_t xu_size    = 24;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01; // Motion detection mode

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDSetMode ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Enable;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDSetMode ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDSetMode <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMDGetMode(uint8_t *Enable)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDGetMode  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[24] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MOTION_DETECTION;
    uint16_t xu_size    = 24;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01; // Motion detection mode

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDGetMode ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDGetMode ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Enable = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "Motion Detect mode = %d\n", *Enable);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDGetMode <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMDSetThreshold(uint32_t MD_Threshold)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDSetThreshold  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[24] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MOTION_DETECTION;
    uint16_t xu_size    = 24;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x02; // Motion detection threshold

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDSetThreshold ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = (MD_Threshold & 0xFF00) >> 8;
    xu_data[1] = MD_Threshold & 0x00FF;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDSetThreshold ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDSetThreshold <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMDGetThreshold(uint32_t *MD_Threshold)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDGetThreshold  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[24] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MOTION_DETECTION;
    uint16_t xu_size    = 24;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x02; // Motion detection threshold

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDGetThreshold ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDGetThreshold ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *MD_Threshold = (xu_data[0] << 8) | xu_data[1];

    TestAp_Printf(TESTAP_DBG_FLOW, "Motion Detect threshold = %d\n", *MD_Threshold);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDGetThreshold <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMDSetMask(uint8_t *Mask)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDSetMask  ==>\n");

    int32_t err = 0;
    uint8_t i;
    uint8_t ctrldata[24] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MOTION_DETECTION;
    uint16_t xu_size    = 24;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x03; // Motion detection mask

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDSetMask ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    for (i = 0; i < 24; i++) {
        xu_data[i] = Mask[i];
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDSetMask ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDSetMask <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMDGetMask(uint8_t *Mask)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDGetMask  ==>\n");

    int32_t err = 0;
    int32_t i, j, k;
    uint8_t ctrldata[24] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MOTION_DETECTION;
    uint16_t xu_size    = 24;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x03; // Motion detection mask

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDGetMask ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)  \n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDGetMask ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i) \n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    for (i = 0; i < 24; i++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
        Mask[i] = xu_data[i];
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "               ======   Motion Detect Mask   ======                \n");
    TestAp_Printf(TESTAP_DBG_FLOW, "     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16 \n");

    for (k = 0; k < 12; k++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "%2d   ", k + 1);
        for (j = 0; j < 2; j++) {
            for (i = 0; i < 8; i++) {
                TestAp_Printf(TESTAP_DBG_FLOW, "%d   ", (Mask[k * 2 + j] >> i) & 0x01);
            }
        }
        TestAp_Printf(TESTAP_DBG_FLOW, "\n");
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMDGetMask <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMDSetResult(uint8_t *Result)
{
    // TestAp_Printf(TESTAP_DBG_FLOW, "XuMDSetResult  ==>\n");

    int32_t err = 0;
    uint8_t i;
    uint8_t ctrldata[24] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MOTION_DETECTION;
    uint16_t xu_size    = 24;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x04; // Motion detection Result

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDSetResult ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    for (i = 0; i < 24; i++) {
        xu_data[i] = Result[i];
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDSetResult ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // TestAp_Printf(TESTAP_DBG_FLOW, "XuMDSetResult <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMDGetResult(uint8_t *Result)
{
    // TestAp_Printf(TESTAP_DBG_FLOW, "XuMDGetResult  ==>\n");

    int32_t err = 0;
    int32_t i, j, k;
    uint8_t ctrldata[24] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_MOTION_DETECTION;
    uint16_t xu_size    = 24;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x04; // Motion detection Result

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDGetResult ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)  \n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMDGetResult ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i) \n", err);
        if (err == EINVAL)
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        return err;
    }

    for (i = 0; i < 24; i++) {
        // TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
        Result[i] = xu_data[i];
    }

    //	system("clear");
    TestAp_Printf(TESTAP_DBG_FLOW, "               ------   Motion Detect Result   ------                \n");
    TestAp_Printf(TESTAP_DBG_FLOW, "     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16 \n");

    for (k = 0; k < 12; k++) {
        TestAp_Printf(TESTAP_DBG_FLOW, "%2d   ", k + 1);
        for (j = 0; j < 2; j++) {
            for (i = 0; i < 8; i++) {
                TestAp_Printf(TESTAP_DBG_FLOW, "%d   ", (Result[k * 2 + j] >> i) & 0x01);
            }
        }
        TestAp_Printf(TESTAP_DBG_FLOW, "\n");
    }

    // TestAp_Printf(TESTAP_DBG_FLOW, "XuMDGetResult <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuMJPGGetBitrate(uint32_t *MJPG_Bitrate)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMJPGGetBitrate ==>\n");
    int32_t i               = 0;
    int32_t ret             = 0;
    int32_t err             = 0;
    uint8_t ctrldata[11]    = {0};
    int32_t BitRate_CtrlNum = 0;
    *MJPG_Bitrate           = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_MJPG_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_MJPG_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMJPGGetBitrate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get Bit rate ctrl number
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMJPGGetBitrate ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "   == XuMJPGGetBitrate Success == \n");

    if (chip_id_ == CHIP_RER9420) {
        for (i = 0; i < 2; i++) {
            TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
        }

        BitRate_CtrlNum = (xu_data[0] << 8) | (xu_data[1]);

        // Bit Rate = BitRate_Ctrl_Num*256*fps*8 /1024(Kbps)
        *MJPG_Bitrate = (BitRate_CtrlNum * 256.0 * current_fps_ * 8) / 1024.0;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        for (i = 0; i < 4; i++) {
            TestAp_Printf(TESTAP_DBG_FLOW, "      Get data[%d] : 0x%x\n", i, xu_data[i]);
        }

        *MJPG_Bitrate = (xu_data[0] << 24) | (xu_data[1] << 16) | (xu_data[2] << 8) | (xu_data[3]);
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMJPGGetBitrate (%x)<==\n", *MJPG_Bitrate);
    return ret;
}

int32_t H264XuCtrls::XuMJPGSetBitrate(uint32_t MJPG_Bitrate)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuMJPGSetBitrate (%x) ==>\n", MJPG_Bitrate);
    int32_t ret             = 0;
    int32_t err             = 0;
    uint8_t ctrldata[11]    = {0};
    int32_t BitRate_CtrlNum = 0;

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_MJPG_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_MJPG_CTRL;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMJPGSetBitrate ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set Bit Rate Ctrl Number
    if (chip_id_ == CHIP_RER9420) {
        // Bit Rate = BitRate_Ctrl_Num*256*fps*8/1024 (Kbps)
        BitRate_CtrlNum = ((MJPG_Bitrate * 1024) / (256 * current_fps_ * 8));

        xu_data[0] = (BitRate_CtrlNum & 0xFF00) >> 8;
        xu_data[1] = (BitRate_CtrlNum & 0x00FF);
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_data[0] = (MJPG_Bitrate & 0xFF000000) >> 24;
        xu_data[1] = (MJPG_Bitrate & 0x00FF0000) >> 16;
        xu_data[2] = (MJPG_Bitrate & 0x0000FF00) >> 8;
        xu_data[3] = (MJPG_Bitrate & 0x000000FF);
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuMJPGSetBitrate ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuMJPGSetBitrate <== Success \n");
    return ret;
}

int32_t H264XuCtrls::XuIMGSetMirror(uint8_t Mirror)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGSetMirror  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGSetMirror ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Mirror;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGSetMirror ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGSetMirror  0x%x <== Success \n", Mirror);

    return 0;
}

int32_t H264XuCtrls::XuIMGGetMirror(uint8_t *Mirror)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGGetMirror  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x01;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGGetMirror ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGGetMirror ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Mirror = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "Mirror = %d\n", *Mirror);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGGetMirror <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuIMGSetFlip(uint8_t Flip)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGSetFlip  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGSetFlip ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Flip;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGSetFlip ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGSetFlip  0x%x <== Success \n", Flip);

    return 0;
}

int32_t H264XuCtrls::XuIMGGetFlip(uint8_t *Flip)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGGetFlip  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x02;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGGetFlip ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGGetFlip ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Flip = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "Flip = %d\n", *Flip);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGGetFlip <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuIMGSetColor(uint8_t Color)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGSetColor  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x03;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x03;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGSetColor ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Color;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGSetColor ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGSetColor  0x%x <== Success \n", Color);

    return 0;
}

int32_t H264XuCtrls::XuIMGGetColor(uint8_t *Color)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGGetColor  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = 0;
    uint8_t xu_selector = 0;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (chip_id_ == CHIP_RER9420) {
        xu_unit     = XU_RERVISION_SYS_ID;
        xu_selector = XU_RERVISION_SYS_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x03;
    } else if ((chip_id_ == CHIP_RER9421) || (chip_id_ == CHIP_RER9422)) {
        xu_unit     = XU_RERVISION_USR_ID;
        xu_selector = XU_RERVISION_USR_IMG_SETTING;

        // Switch command
        xu_data[0] = 0x9A; // Tag
        xu_data[1] = 0x03;
    }

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGGetColor ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuIMGGetColor ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Color = xu_data[0];

    TestAp_Printf(TESTAP_DBG_FLOW, "Image Color = %d\n", *Color);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuIMGGetColor <== Success \n");

    return 0;
}

//--------------------------------------------------------------------------------------
int32_t H264XuCtrls::XuOsdSetCarcamCtrl(uint8_t SpeedEn, uint8_t CoordinateEn, uint8_t CoordinateCtrl)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetCarcamCtrl  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x0A;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetCarcamCtrl ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = SpeedEn;
    xu_data[1] = CoordinateEn;
    xu_data[2] = CoordinateCtrl;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetCarcamCtrl ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetCarcamCtrl  0x%x  0x%x  0x%x <== Success \n", xu_data[0], xu_data[1], xu_data[2]);

    return 0;
}

int32_t H264XuCtrls::XuOsdGetCarcamCtrl(uint8_t *SpeedEn, uint8_t *CoordinateEn, uint8_t *CoordinateCtrl)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetCarcamCtrl  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x0A;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetCarcamCtrl ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetCarcamCtrl ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *SpeedEn        = xu_data[0];
    *CoordinateEn   = xu_data[1];
    *CoordinateCtrl = xu_data[2];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD speed en = %d\n", *SpeedEn);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD coordinate en = %d\n", *CoordinateEn);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD coordinate ctrl = %d\n", *CoordinateCtrl);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetCarcamCtrl <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetSpeed(uint32_t Speed)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetSpeed  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x0B;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetSpeed ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = (Speed >> 8) & 0xFF;
    xu_data[1] = Speed & 0xFF;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetSpeed ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetSpeed  0x%x  0x%x  <== Success \n", xu_data[0], xu_data[1]);

    return 0;
}

int32_t H264XuCtrls::XuOsdGetSpeed(uint32_t *Speed)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetSpeed  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x0B;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetSpeed ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetSpeed ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Speed = (xu_data[0] << 8) | (xu_data[1]);

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD speed = %d \n", *Speed);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetSpeed <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdSetCoordinate1(uint8_t Direction, uint8_t *Vaule)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetCoordinate1  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x0C;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetCoordinate1 ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Direction;
    xu_data[1] = Vaule[0];
    xu_data[2] = Vaule[1];
    xu_data[3] = Vaule[2];
    xu_data[4] = 0;
    xu_data[5] = Vaule[3];
    xu_data[6] = Vaule[4];
    xu_data[7] = Vaule[5];
    xu_data[8] = 0;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetCoordinate1 ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XU_OSD_Set_Coordinate  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x 0x%x <== Success \n", xu_data[0], xu_data[1],
                  xu_data[2], xu_data[3], xu_data[4], xu_data[5], xu_data[6], xu_data[7], xu_data[8]);

    return 0;
}

int32_t H264XuCtrls::XuOsdSetCoordinate2(uint8_t Direction, uint8_t Vaule1, unsigned long Vaule2, uint8_t Vaule3, unsigned long Vaule4)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetCoordinate2  ==>\n");

    int32_t err = 0;
    //	char i;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x0C;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetCoordinate2 ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Direction;
    xu_data[1] = Vaule1;
    xu_data[2] = (Vaule2 >> 16) & 0xFF;
    xu_data[3] = (Vaule2 >> 8) & 0xFF;
    xu_data[4] = Vaule2 & 0xFF;

    xu_data[5] = Vaule3;
    xu_data[6] = (Vaule4 >> 16) & 0xFF;
    xu_data[7] = (Vaule4 >> 8) & 0xFF;
    xu_data[8] = Vaule4 & 0xFF;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdSetCoordinate2 ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdSetCoordinate2  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x <== Success \n", xu_data[0], xu_data[1],
                  xu_data[2], xu_data[3], xu_data[4], xu_data[5], xu_data[6], xu_data[7], xu_data[8]);

    return 0;
}

int32_t H264XuCtrls::XuOsdGetCoordinate1(uint8_t *Direction, uint8_t *Vaule)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetCoordinate1  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x0C;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetCoordinate1 ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetCoordinate1 ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Direction = xu_data[0];
    Vaule[0]   = xu_data[1];
    Vaule[1]   = xu_data[2];
    Vaule[2]   = xu_data[3];
    Vaule[3]   = xu_data[5];
    Vaule[4]   = xu_data[6];
    Vaule[5]   = xu_data[7];

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate direction = %d\n", *Direction);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate degree1 = %d\n", Vaule[0]);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate minute1 = %d\n", Vaule[1]);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate second1 = %d\n", Vaule[2]);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate degree2 = %d\n", Vaule[3]);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate minute2 = %d\n", Vaule[4]);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate second2 = %d\n", Vaule[5]);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetCoordinate1 <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuOsdGetCoordinate2(uint8_t *Direction, uint8_t *Vaule1, unsigned long *Vaule2, uint8_t *Vaule3, unsigned long *Vaule4)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetCoordinate2  ==>\n");

    int32_t err = 0;
    //	char i;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_OSD_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x0C;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetCoordinate2 ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuOsdGetCoordinate2 ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Direction = xu_data[0];
    *Vaule1    = xu_data[1];
    *Vaule2    = (xu_data[2] << 16) | (xu_data[3] << 8) | (xu_data[4]);
    *Vaule3    = xu_data[5];
    *Vaule4    = (xu_data[6] << 16) | (xu_data[7] << 8) | (xu_data[8]);

    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate direction = %d\n", *Direction);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate degree1 = %d.%05ld\n", *Vaule1, *Vaule2);
    TestAp_Printf(TESTAP_DBG_FLOW, "OSD Coordinate degree2 = %d.%05ld\n", *Vaule3, *Vaule4);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetCoordinate2 <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuGPIOCtrlSet(uint8_t GPIO_En, uint8_t GPIO_Value)
{

    TestAp_Printf(TESTAP_DBG_FLOW, "XuGPIOCtrlSet  ==>\n");

    int32_t err = 0;
    //	char i;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_GPIO_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuGPIOCtrlSet ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = GPIO_En;
    xu_data[1] = GPIO_Value;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuGPIOCtrlSet ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuGPIOCtrlSet  0x%x  0x%x <== Success \n", xu_data[0], xu_data[1]);

    return 0;
}

int32_t H264XuCtrls::XuGPIOCtrlGet(uint8_t *GPIO_En, uint8_t *GPIO_OutputValue, uint8_t *GPIO_InputValue)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuGPIOCtrlGet  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_GPIO_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuGPIOCtrlGet ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuGPIOCtrlGet ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *GPIO_En          = xu_data[0];
    *GPIO_OutputValue = xu_data[1];
    *GPIO_InputValue  = xu_data[2];

    TestAp_Printf(TESTAP_DBG_FLOW, "GPIO enable = 0x%x\n", *GPIO_En);
    TestAp_Printf(TESTAP_DBG_FLOW, "GPIO Output value = 0x%x, Input value = 0x%x\n", *GPIO_OutputValue, *GPIO_InputValue);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuGPIOCtrlGet <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuFrameDropEnSet(uint8_t Stream1_En, uint8_t Stream2_En)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuFrameDropEnSet  ==>\n");

    int32_t err = 0;
    //	char i;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_DYNAMIC_FPS_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuFrameDropEnSet ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Stream1_En;
    xu_data[1] = Stream2_En;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuFrameDropEnSet ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuFrameDropEnSet  0x%x  0x%x <== Success \n", xu_data[0], xu_data[1]);

    return 0;
}

int32_t H264XuCtrls::XuFrameDropEnGet(uint8_t *Stream1_En, uint8_t *Stream2_En)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuFrameDropEnGet  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_DYNAMIC_FPS_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x01;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuFrameDropEnGet ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuFrameDropEnGet ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Stream1_En = xu_data[0];
    *Stream2_En = xu_data[1];

    TestAp_Printf(TESTAP_DBG_FLOW, "Stream1 frame drop enable = %d\n", *Stream1_En);
    TestAp_Printf(TESTAP_DBG_FLOW, "Stream2 frame drop enable = %d\n", *Stream2_En);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetCoordinate1 <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuFrameDropCtrlSet(uint8_t Stream1_fps, uint8_t Stream2_fps)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuFrameDropCtrlSet  ==>\n");

    int32_t err = 0;
    //	char i;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_DYNAMIC_FPS_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x02;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuFrameDropCtrlSet ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Set data
    xu_data[0] = Stream1_fps;
    xu_data[1] = Stream2_fps;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuFrameDropCtrlSet ==> ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    TestAp_Printf(TESTAP_DBG_FLOW, "XuFrameDropCtrlSet  0x%x  0x%x <== Success \n", xu_data[0], xu_data[1]);

    return 0;
}

int32_t H264XuCtrls::XuFrameDropCtrlGet(uint8_t *Stream1_fps, uint8_t *Stream2_fps)
{
    TestAp_Printf(TESTAP_DBG_FLOW, "XuFrameDropCtrlGet  ==>\n");

    int32_t err          = 0;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_USR_ID;
    uint8_t xu_selector = XU_RERVISION_USR_DYNAMIC_FPS_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    // Switch command
    xu_data[0] = 0x9A; // Tag
    xu_data[1] = 0x02;

    if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuFrameDropCtrlGet ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    // Get data
    memset(xu_data, 0, xu_size);
    if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
        TestAp_Printf(TESTAP_DBG_ERR, "XuFrameDropCtrlGet ==> ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
        if (err == EINVAL) {
            TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
        }
        return err;
    }

    *Stream1_fps = xu_data[0];
    *Stream2_fps = xu_data[1];

    TestAp_Printf(TESTAP_DBG_FLOW, "Stream1 frame  = %d\n", *Stream1_fps);
    TestAp_Printf(TESTAP_DBG_FLOW, "Stream2 frame  = %d\n", *Stream2_fps);
    TestAp_Printf(TESTAP_DBG_FLOW, "XuOsdGetCoordinate1 <== Success \n");

    return 0;
}

int32_t H264XuCtrls::XuSFRead(uint32_t Addr, uint8_t *pData, uint32_t Length)
{
#define DEF_SF_RW_LENGTH 8
#define min(a, b) a < b ? a : b
    // TestAp_Printf(TESTAP_DBG_FLOW, "XuSFRead  ==>\n");

    int32_t err = 0;
    uint32_t i, ValidLength = 0, loop = 0, remain = 0;
    uint8_t *pCopy       = pData;
    uint8_t ctrldata[11] = {0};

    // uvc_xu_control parmeters
    uint8_t xu_unit     = XU_RERVISION_SYS_ID;
    uint8_t xu_selector = XU_RERVISION_SYS_FLASH_CTRL;
    uint16_t xu_size    = 11;
    uint8_t *xu_data    = ctrldata;

    if (Addr < 0x10000)
        ValidLength = min(0x10000 - Addr, Length);
    else
        ValidLength = min(0x20000 - Addr, Length);
    loop   = ValidLength / DEF_SF_RW_LENGTH;
    remain = ValidLength % DEF_SF_RW_LENGTH;
    // TestAp_Printf(TESTAP_DBG_ERR,"valid = %d, loop = %d, remain = %d\n", ValidLength,loop,remain);

    // get sf data
    for (i = 0; i < loop; i++) {

        xu_data[0] = (Addr + i * DEF_SF_RW_LENGTH) & 0xff;
        xu_data[1] = ((Addr + i * DEF_SF_RW_LENGTH) >> 8) & 0xff;
        if (Addr + i * DEF_SF_RW_LENGTH < 0x10000)
            xu_data[2] = 0x80 | DEF_SF_RW_LENGTH;
        else
            xu_data[2] = 0x90 | DEF_SF_RW_LENGTH;

        // set sf start addr
        if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
            TestAp_Printf(TESTAP_DBG_ERR, "XuSFRead ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
            if (err == EINVAL)
                TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
            return err;
        }
        memset(xu_data, 0, xu_size * sizeof(uint8_t));
        if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
            TestAp_Printf(TESTAP_DBG_ERR, "XuSFRead ==> Switch cmd : ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
            if (err == EINVAL) {
                TestAp_Printf(TESTAP_DBG_ERR, "Read SF error\n");
            }
            return err;
        }
        memcpy(pCopy, xu_data + 3, DEF_SF_RW_LENGTH);
        pCopy += DEF_SF_RW_LENGTH;
    }

    if (remain) {
        xu_data[0] = (Addr + loop * DEF_SF_RW_LENGTH) & 0xff;
        xu_data[1] = ((Addr + loop * DEF_SF_RW_LENGTH) >> 8) & 0xff;
        if (Addr < 0x10000) {
            xu_data[2] = 0x80 | remain;
        } else {
            xu_data[2] = 0x90 | remain;
        }
        // set addr and length of remain
        if ((err = XuSetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
            TestAp_Printf(TESTAP_DBG_ERR, "XuSFRead ==> Switch cmd : ioctl(UVCIOC_CTRL_SET) FAILED (%i)\n", err);
            if (err == EINVAL) {
                TestAp_Printf(TESTAP_DBG_ERR, "Invalid arguments\n");
            }
            return err;
        }

        // get data of remain

        memset(xu_data, 0, xu_size * sizeof(uint8_t));
        if ((err = XuGetCur(xu_unit, xu_selector, xu_size, xu_data)) < 0) {
            TestAp_Printf(TESTAP_DBG_ERR, "XuSFRead ==> Switch cmd : ioctl(UVCIOC_CTRL_GET) FAILED (%i)\n", err);
            if (err == EINVAL) {
                TestAp_Printf(TESTAP_DBG_ERR, "Read SF error\n");
            }
            return err;
        }

        memcpy(pCopy, xu_data + 3, remain);
    }
    // TestAp_Printf(TESTAP_DBG_FLOW, "XuSFRead <== Success \n");
    return 0;
}

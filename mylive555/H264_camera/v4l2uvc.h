
/*******************************************************************************
#             uvccapture: USB UVC Video Class Snapshot Software                #
#This package work with the Logitech UVC based webcams with the mjpeg feature  #
#                                                                              #
#       Orginally Copyright (C) 2005 2006 Laurent Pinchart &&  Michel Xhaard   #
#       Modifications Copyright (C) 2006 Gabriel A. Devenyi                    #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; either version 2 of the License, or            #
# (at your option) any later version.                                          #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/
#pragma once

#include <linux/videodev2.h>
#include <stdint.h>

#define NB_BUFFER 16
#define DHT_SIZE 420

#ifndef V4L2_CID_SHARPNESS
#define V4L2_CID_SHARPNESS (V4L2_CID_BASE + 27)
#endif

struct vdIn {
    int32_t fd;
    char *videodevice;
    int8_t *status;
    int8_t *pictName;
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_buffer buf;
    struct v4l2_requestbuffers rb;
    void *mem[NB_BUFFER];
    uint8_t *tmpbuffer;
    uint8_t *framebuffer;
    int32_t isstreaming;
    int32_t grabmethod;
    uint32_t width;
    uint32_t height;
    uint32_t formatIn;
    uint32_t formatOut;
    uint32_t framesizeIn;
    uint32_t signalquit;
    uint32_t toggleAvi;
    uint32_t getPict;
};

class V4l2Capture
{
public:
  V4l2Capture();
  ~V4l2Capture();

  int32_t InitV4l2(struct vdIn *vd);
  int32_t CloseV4l2(struct vdIn *vd);

private:
  int32_t InitVideoIn(struct vdIn *vd, char *device, int32_t width, int32_t height,
                 int32_t format, int32_t grabmethod);
  int32_t VideoDisable(struct vdIn *vd);
  int32_t VideoEnable(struct vdIn *vd);
  int32_t UvcGrab(struct vdIn *vd);
  int32_t IsV4l2Control(int32_t fd, int32_t control, struct v4l2_queryctrl *queryctrl);

  int32_t v4l2GetControl(int32_t fd, int32_t control);
  int32_t v4l2SetControl(int32_t fd, int32_t control, int32_t value);
  int32_t v4l2UpControl(int32_t fd, int32_t control);
  int32_t v4l2DownControl(int32_t fd, int32_t control);
  int32_t v4l2ToggleControl(int32_t fd, int32_t control);
  int32_t v4l2ResetControl(int32_t fd, int32_t control);

private:
  int32_t debug_;
  struct vdIn *video_;
};
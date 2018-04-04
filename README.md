# rtsp_server
# 
# 基于live555的H264视频直播
# 先进入rtsp_server目录编译出静态库文件
# 可以利用./genMakefiles linux来生成linux环境的Makefile
# 不同的系统参考config.*
#
# 生成Makefile后make生成*.a文件
# 继续进入mylive555目录
# 在mylive555/H264FramedLiveSource.hh的#define SOFT_H264 0 修改是否使用软件压缩H264
# 1为软件压缩 0为UVC H264摄像头获取视频流
# 如果使用H264摄像头 在H264_camera下的H264_UVC_TestAP.cpp 下的Init_264camera()
# 修改摄像头为对应的H264码流通道
# 测试通过了普通YUV摄像头和威尔锐视H264摄像头，基本支持v4l2框架摄像头的流获取
# 下一步是测试树莓派CSI摄像头，并使用respberry硬件编码H264
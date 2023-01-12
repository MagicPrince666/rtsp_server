CXX = /Volumes/unix/openwrt/staging_dir/toolchain-aarch64_cortex-a53_gcc-11.3.0_musl/bin/aarch64-openwrt-linux-musl-g++

TARGET	= RTSPServer

DIR		= . ./camera ./ringbuf ./H264_camera ./raspberry ./epoll


livedir := ../
livemedia := $(livedir)liveMedia
usageenviroment := $(livedir)UsageEnvironment
basicusage := $(livedir)BasicUsageEnvironment
groupsock := $(livedir)groupsock
live := $(livedir)

LIBSDIR += -L$(usageenviroment) -L$(basicusage) -L$(live) -L$(livemedia) -L$(groupsock)
INC		+= -I. -I./include -I./ringbuf -I./H264_camera -I./camera -I./raspberry -I./epoll
INC		+= -I$(usageenviroment)/include -I$(basicusage)/include -I$(livemedia)/include -I$(groupsock)/include

LDFLAGS += -L./lib $(LIBSDIR) -lx264 -lliveMedia -lBasicUsageEnvironment -lgroupsock -lUsageEnvironment
LDFLAGS += -lpthread -ldl -lm
CFLAGS	= -O3 -Wall

OBJPATH	= ./objs

FILES	= $(foreach dir,$(DIR),$(wildcard $(dir)/*.cpp))

OBJS	= $(patsubst %.cpp,%.o,$(FILES))

all:$(OBJS) $(TARGET)

$(OBJS):%.o:%.cpp
	$(CXX) $(CFLAGS) $(INC) -c -o $(OBJPATH)/$(notdir $@) $< 

$(TARGET):$(OBJPATH)
	$(CXX) -o $@ $(OBJPATH)/*.o $(LDFLAGS)

clean:
	-rm -f $(OBJPATH)/*.o
	-rm -f $(TARGET)
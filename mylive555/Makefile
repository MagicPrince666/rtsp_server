CPP = g++

TARGET	= RTSPServer

DIR		= . ./camera ./ringbuf ./H264_camera ./raspberry


livedir := ../
livemedia := $(livedir)liveMedia
usageenviroment := $(livedir)UsageEnvironment
basicusage := $(livedir)BasicUsageEnvironment
groupsock := $(livedir)groupsock
live := $(livedir)

LIBSDIR += -L$(usageenviroment) -L$(basicusage) -L$(live) -L$(livemedia) -L$(groupsock)
INC		+= -I. -I./include -I./ringbuf -I/usr/include -I./H264_camera -I./camera -I./raspberry
INC		+= -I$(usageenviroment)/include -I$(basicusage)/include -I$(livemedia)/include -I$(groupsock)/include

LDFLAGS += -L./lib $(LIBSDIR) -lx264 -lliveMedia -lBasicUsageEnvironment -lgroupsock -lUsageEnvironment
LDFLAGS += -lpthread -ldl -lm
CFLAGS	= -O3 -g -Wall

OBJPATH	= ./objs

FILES	= $(foreach dir,$(DIR),$(wildcard $(dir)/*.cpp))

OBJS	= $(patsubst %.cpp,%.o,$(FILES))

all:$(OBJS) $(TARGET)

$(OBJS):%.o:%.cpp
	$(CPP) $(CFLAGS) $(INC) -c -o $(OBJPATH)/$(notdir $@) $< 

$(TARGET):$(OBJPATH)
	$(CPP) -o $@ $(OBJPATH)/*.o $(LDFLAGS)

#$(OBJPATH):
#	mkdir -p $(OBJPATH)

clean:
	-rm -f $(OBJPATH)/*.o
	-rm -f $(TARGET)
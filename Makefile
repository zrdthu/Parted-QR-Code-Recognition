# 2019.08.15 by zrd: zhangr.d.1996@gmail.com

TARGET = qr
OBJ_PATH = objs
CC = g++
INCLUDES = -Iinclude -I/usr/local/include/opencv4/
LIBS = -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_ximgproc -lopencv_objdetect
CFLAGS = -Wall -Werror -O3 -fopenmp 
LINKFLAGS = -fopenmp

SRCDIR = src

CPP_SRCDIR = $(SRCDIR)
CPP_SOURCES = $(foreach d,$(CPP_SRCDIR),$(wildcard $(d)/*.cpp) )
CPP_OBJS = $(patsubst %.cpp, $(OBJ_PATH)/%.o, $(CPP_SOURCES))

default:init compile
	 
$(C_OBJS):$(OBJ_PATH)/%.o:%.c
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
		 
$(CPP_OBJS):$(OBJ_PATH)/%.o:%.cpp
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@
			 
init:
	$(foreach d,$(SRCDIR), mkdir -p $(OBJ_PATH)/$(d);)
		 
compile:$(C_OBJS) $(CPP_OBJS)
	$(CC)  $^ -o $(TARGET)  $(LINKFLAGS) $(LIBS)
		 
clean:
	rm -rf $(OBJ_PATH)
	rm -f $(TARGET)

########################################
#makefile
########################################
#****************************************************************************
# Cross complie path
#****************************************************************************

#FREETYPE_LIB_ROOT=../third_lib/freetype-2.13.2
FREETYPE_LIB_ROOT=../third_lib/freetype-x86-2.13.2

#CHAIN_ROOT= /opt/armgcc/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin
CHAIN_ROOT= 

#CROSS_COMPILE=$(CHAIN_ROOT)/arm-linux-gnueabihf-
CROSS_COMPILE = 

CC     := $(CROSS_COMPILE)gcc
CXX    := $(CROSS_COMPILE)g++
AS     := $(CROSS_COMPILE)as
AR     := $(CROSS_COMPILE)ar 
LD     := $(CROSS_COMPILE)ld
RANLIB := $(CROSS_COMPILE)ranlib
OBJDUMP:= $(CROSS_COMPILE)objdump
OBJCOPY:= $(CROSS_COMPILE)objcopy
STRIP  := $(CROSS_COMPILE)strip

SUBDIRS := 

define make_subdir
 @ for subdir in $(SUBDIRS) ; do \
 ( cd $$subdir && make $1) \
 done;
endef

#output
BINARY  := txt2bmp
OBJ_DIR := 

CFLAGS=  -I$(FREETYPE_LIB_ROOT)/include/freetype2

LDSCRIPT=  -lfreetype -lm -static

LDFLAGS= -L$(FREETYPE_LIB_ROOT)/lib

SRC  = $(wildcard *.c)
DIR  = $(notdir $(SRC))
OBJS = $(patsubst %.c,$(OBJ_DIR)%.o,$(DIR))

.PHONY: clean lib

all:  prebuild  $(BINARY)

prebuild:
	@echo Building app...

$(BINARY) : $(OBJS)
	@echo Generating ...
	$(CC) -o $(BINARY) $(OBJS) $(LDFLAGS) $(LDSCRIPT) 
	@echo OK!

$(OBJ_DIR)%.o : %.c
	$(CC) -c $(CFLAGS) $< -o  $@

clean:
	@rm -f $(OBJ_DIR)*.o
	@echo Removed!
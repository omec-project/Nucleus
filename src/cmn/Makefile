#
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0
#

include ../../Makefile.common

CC := g++
CFLAGS += -std=c++11 

ifeq ($(DEBUG),true)
        CFLAGS += -g
endif
ifeq ($(DEBUG),false)
        CFLAGS += -O3
endif

IPC_LIBNAME = $(LIBDIR)/libipcfwk.so
DATAGROUPMGR_LIBNAME = $(LIBDIR)/libdatagroupmgr.so
CMNUTILS_LIBNAME = $(LIBDIR)/libcmnUtils.so

IPC_SRCS := ./ipcChannel.cpp  \
            ./tipcSocket.cpp
            
DATAGROUPMGR_SRCS := ./dataGroupManager.cpp

CMNUTILS_SRCS := ./cTime.cpp \
		 ./debug.cpp \
		 ./msgBuffer.cpp \
		 ./timerQueue.cpp \
		 ./timeoutManager.cpp \
		 ./componentDb.cpp
		 

SRCDIR := .
SRCEXT := cpp
IPC_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/cmn/%,$(IPC_SRCS:.$(SRCEXT)=.o))
DATAGROUPMGR_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/cmn/%,$(DATAGROUPMGR_SRCS:.$(SRCEXT)=.o))
CMNUTILS_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/cmn/%,$(CMNUTILS_SRCS:.$(SRCEXT)=.o))


OBJS := $(IPC_OBJS) \
	$(DATAGROUPMGR_OBJS)\
	$(CMNUTILS_OBJS)

all : buildCommonLIBS

buildCommonLIBS : $(OBJS)
	@echo "Linking"
	@mkdir -p $(LIBDIR)
	$(CC) $(CFLAGS) $(IPC_OBJS) -shared -o $(IPC_LIBNAME)
	$(CC) $(CFLAGS) $(DATAGROUPMGR_OBJS) -shared -o $(DATAGROUPMGR_LIBNAME)
	$(CC) $(CFLAGS) $(CMNUTILS_OBJS) -shared -o $(CMNUTILS_LIBNAME)

$(OBJS) : $(OBJDIR)/cmn/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)/cmn
	$(CC) $(CFLAGS) $(INC_DIRS) -fPIC -c -o $@ $<

install:all
	-@mkdir -p $(TARGET_DIR)/lib
	-@cp $(IPC_LIBNAME) $(TARGET_DIR)/lib
	-@cp $(DATAGROUPMGR_LIBNAME) $(TARGET_DIR)/lib
	-@cp $(CMNUTILS_LIBNAME) $(TARGET_DIR)/lib

clean:
	@echo " Cleaning...";
	-@rm -rf $(OBJDIR)/cmn
	-@rm -f $(IPC_LIBNAME) $(DATAGROUPMGR_LIBNAME) $(CMNUTILS_LIBNAME)


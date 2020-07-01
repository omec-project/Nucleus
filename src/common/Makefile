#
# Copyright 2019-present Open Networking Foundation
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0

#
include ../../Makefile.common

ifeq ($(DEBUG),true)
	CFLAGS += -g
endif

ifeq ($(DEBUG),false)
	CFLAGS += -O3
endif

IF_LIBNAME = $(LIBDIR)/libinterface.so
JSON_PARSER_LIBNAME = $(LIBDIR)/libjson.so
LOG_LIBNAME = $(LIBDIR)/liblog.a
UNIXSOCK_LIBNAME = $(LIBDIR)/libusock.a
TPOOL_LIBNAME = $(LIBDIR)/libthreadpool.a
UTIL_LIBNAME = $(LIBDIR)/libsecutil.so
MONITOR_CONFIG_LIBNAME = $(LIBDIR)/libmonitor_config.so

IF_SRC = ./ipc_api.c
LOG_SRC = ./log.c
TPOOL_SRC = ./thread_pool.c ./tpool_queue.c
JSON_PARSER_SRC = ./json_parser.c
UTIL_SRC = ./snow_3g.c ./f8.c ./f9.c
UNIXSOCK_SRC = ./unix_sock.c ./unix_conn.c
MONITOR_CONFIG_SRC = ./monitor_config.c ./backtrace.c

SRCDIR := .
SRCEXT := c
IF_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/common/%,$(IF_SRC:.$(SRCEXT)=.o))
LOG_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/common/%,$(LOG_SRC:.$(SRCEXT)=.o))
TPOOL_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/common/%,$(TPOOL_SRC:.$(SRCEXT)=.o))
JSON_PARSER_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/common/%,$(JSON_PARSER_SRC:.$(SRCEXT)=.o))
UTIL_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/common/%,$(UTIL_SRC:.$(SRCEXT)=.o))
MONITOR_CONFIG_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/common/%,$(MONITOR_CONFIG_SRC:.$(SRCEXT)=.o))
UNIXSOCK_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/common/%,$(UNIXSOCK_SRC:.$(SRCEXT)=.o))

OBJS := $(IF_OBJS) \
        $(LOG_OBJS)\
        $(TPOOL_OBJS) \
	$(JSON_PARSER_OBJS) \
	$(UTIL_OBJS) \
	$(MONITOR_CONFIG_OBJS) \
	$(UNIXSOCK_OBJS)

build_commonLibs: $(OBJS)
	-@echo "Linking..."
	-@mkdir -p $(LIBDIR)
	$(CC) $(CFLAGS) $(IF_OBJS) -shared -o $(IF_LIBNAME)
	$(CC) $(CFLAGS) $(JSON_PARSER_OBJS) -shared -o $(JSON_PARSER_LIBNAME)
	$(CC) $(CFLAGS) $(MONITOR_CONFIG_OBJS) -shared -o $(MONITOR_CONFIG_LIBNAME)
	$(CC) $(CFLAGS) $(UTIL_OBJS) -shared -o $(UTIL_LIBNAME)
	ar rcs $(LOG_LIBNAME) $(LOG_OBJS)
	ar rcs $(TPOOL_LIBNAME) $(TPOOL_OBJS)
	ar rcs $(UNIXSOCK_LIBNAME) $(UNIXSOCK_OBJS)

$(OBJS) : $(OBJDIR)/common/%.o : $(SRCDIR)/%.c
	-@mkdir -p $(OBJDIR)/common
	$(CC) $(CFLAGS) $(INC_DIRS) -fPIC -c -o $@ $<

all: build_commonLibs

install:all
	-@mkdir -p $(TARGET_DIR)/lib
	-@mkdir -p $(TARGET_DIR)/conf
	-@cp $(IF_LIBNAME) $(TARGET_DIR)/lib
	-@cp $(LOG_LIBNAME) $(TARGET_DIR)/lib
	-@cp $(JSON_PARSER_LIBNAME) $(TARGET_DIR)/lib
	-@cp $(UTIL_LIBNAME) $(TARGET_DIR)/lib
	-@cp $(TPOOL_LIBNAME) $(TARGET_DIR)/lib
	-@cp $(MONITOR_CONFIG_LIBNAME) $(TARGET_DIR)/lib
	-@cp $(UNIXSOCK_LIBNAME) $(TARGET_DIR)/lib

clean:
	rm -f $(OBJDIR)/common
	rm -f $(IF_LIBNAME) \
	      $(JSON_PARSER_LIBNAME) \
	      $(UTIL_LIBNAME) \
	      $(LOG_LIBNAME) \
	      $(TPOOL_LIBNAME)


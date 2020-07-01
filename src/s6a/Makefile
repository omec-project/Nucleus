#
# Copyright 2019-present Open Networking Foundation
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0
#

#SRCDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include ../../Makefile.common

LIB_PATH +=-L../common/

SRCDIR := .
TARGET := $(BINDIR)/s6a-app

SRCEXT := c

SOURCES := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')

OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/s6a/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS += -Wall

ifeq ($(DEBUG),true)
	CFLAGS += -g
endif

ifeq ($(DEBUG),false)
#	CFLAGS += -O3
endif


LIBS :=  \
	-linterface \
	-llog \
	-lthreadpool \
	-ljson \
	-lpthread \
	-lfdproto \
	-lfdcore \
	-lmonitor_config

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@mkdir -p $(BINDIR)
	$(CC) $(LFLAGS) $^ -o $(TARGET) $(LIB_PATH) $(LIBS)

$(OBJDIR)/s6a/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)/s6a/handlers
	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

all:$(TARGET)

clean:
	@echo " Cleaning...";
	@rm  -rf $(OBJDIR)/s6a $(TARGET)

install:
	@mkdir -p $(TARGET_DIR)/bin
	-@cp $(TARGET) $(TARGET_DIR)/bin
	-@mkdir -p $(TARGET_DIR)/conf
	-@cp conf/s6a_fd.conf $(TARGET_DIR)/conf/s6a_fd.conf
	-@cp conf/s6a.json $(TARGET_DIR)/conf/s6a.json
	-@cp conf/make_certs.sh $(TARGET_DIR)/conf/make_certs.sh

.PHONY: clean


#
# Copyright 2019-present Open Networking Foundation
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0
#
include ../../Makefile.common

LIB_PATH +=-L../common/

SRCDIR := .
TARGET := $(BINDIR)/s10-app
S10_JSON = s10.json

SRCEXT := c

SOURCES := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')

OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/s10/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS += -Wall -DS10_FEATURE

ifeq ($(DEBUG),true)
	CFLAGS += -g
endif

ifeq ($(DEBUG),false)
	CFLAGS += -O3
endif

LIBS := \
        -lpthread \
	-lthreadpool \
        -lsctp \
	-lgtpV2Codec \
	-lcmnUtils \
	-linterface \
	-llog \
	-ljson \
	-lmonitor_config

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@mkdir -p $(BINDIR)
	g++ $(LFLAGS) $^ -o $(TARGET) $(LIB_PATH) $(LIBS) -Lcpp_utils/ -ls10cpputil
	g++ $(LFLAGS) $^ -o $(TARGET) $(LIB_PATH) $(LIBS)

$(OBJDIR)/s10/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)/s10/handlers
	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

all:$(TARGET)

clean:
	@echo " Cleaning...";
	@rm -rf $(OBJDIR)/s10 $(TARGET)

install:
	@mkdir -p $(TARGET_DIR)/bin/
	-@cp $(TARGET) $(TARGET_DIR)/bin/
	-@cp conf/$(S10_JSON) $(TARGET_DIR)/conf/

.PHONY: clean


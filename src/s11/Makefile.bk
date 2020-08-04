#
# Copyright 2019-present Open Networking Foundation
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0
#
include ../../Makefile.common

CC := g++

LIB_PATH +=-L../common/

SRCDIR := .
TARGET := $(BINDIR)/s11-app
S11_JSON = s11.json

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/s11/%,$(SOURCES:.$(SRCEXT)=.o))

#CEXT := c
#CSOURCES := $(shell find $(SRCDIR) -type f -name '*.$(CEXT)')
#COBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/s11/%,$(CSOURCES:.$(CEXT)=.o))


CFLAGS += -Wall -std=c++11
CFLAGS += -I$(TOPDIR)/src/s11

ifeq ($(DEBUG),true)
	CFLAGS += -g
endif

ifeq ($(DEBUG),false)
	CFLAGS += -O3
endif

LIBS :=	-lpthread \
	    -lgtpV2Codec \
        -lcrypto \
        -lcmnUtils \
        -ldatagroupmgr \
		-ljson \
        -llog \
        -linterface \
        -lipcfwk \
		-lmonitor_config \
		-lthreadpool \
        -lstatemachinefwk \
		-lmmeGrpcProtoBuf \
		-lgrpc++ \
        -lgrpc \
        -lgpr \
		-lsecutil



$(TARGET): $(OBJECTS) 
	@echo " Linking..."
	@mkdir -p $(BINDIR)
	$(CC) $(LFLAGS) $^ -o $(TARGET) $(LIB_PATH) $(LIBS)

$(OBJDIR)/s11/%.o: $(SRCDIR)/%.$(SRCEXT)
	echo "Compiling..."
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/s11/interfaces
	@mkdir -p $(OBJDIR)/s11/msgHandlers
	@mkdir -p $(OBJDIR)/s11/utils
	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

#$(OBJDIR)/s11/%.o: $(SRCDIR)/%.$(CEXT)
#	@mkdir -p $(OBJDIR)/s11/handlers
#	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

all:$(TARGET)

clean:
	@echo " Cleaning...";
	@rm -rf $(OBJDIR)/s11 $(TARGET)

install:
	@echo "Installing s11-app"
	@mkdir -p $(TARGET_DIR)/bin/
	-@cp $(TARGET) $(TARGET_DIR)/bin/
	-@cp conf/$(S11_JSON) $(TARGET_DIR)/conf/

.PHONY: clean


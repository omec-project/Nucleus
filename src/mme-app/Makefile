#
# Copyright 2019-present Open Networking Foundation
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0
#

#

include ../../Makefile.common

CC := g++
CFLAGS += -Wall -DSTATS -I/usr/include/openssl -I$(GRPC_ROOT)/include -I$(GRPC_ROOT)/third_party/protobuf/src -std=c++11
LFLAGS += -L$(GRPC_ROOT)/libs/opt -L$(GRPC_ROOT)/libs/opt/protobuf

ifeq ($(DEBUG),true)
        CFLAGS += -g
endif
ifeq ($(DEBUG),false)
        CFLAGS += -O3
endif

LIB_PATH +=-L../common/ 

LIBS :=	-lpthread \
        -lcrypto \
        -lcmnUtils \
        -ldatagroupmgr \
	-ljson \
        -llog \
        -linterface \
        -lipcfwk \
	-lmonitor_config \
	-lusock \
	-lthreadpool \
        -lstatemachinefwk \
	-lmmeGrpcProtoBuf \
	-lgrpc++ \
        -lgrpc \
        -lgpr \
        -lprotobuf \
        -lprometheus-cpp-pull -lprometheus-cpp-core -lz \
	-lsecutil

SRCDIR := .
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/mme-app/%,$(SOURCES:.$(SRCEXT)=.o))

CEXT := c
CSOURCES := $(shell find $(SRCDIR) -type f -name '*.$(CEXT)')
COBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/mme-app/%,$(CSOURCES:.$(CEXT)=.o))

TARGET := $(BINDIR)/mme-app

$(TARGET): $(COBJECTS) $(OBJECTS) 
	@echo "Linking..."
	@mkdir -p $(BINDIR)
	$(CC) $(LFLAGS) $^ -o $(TARGET) $(LIB_PATH) $(LIBS)

$(OBJDIR)/mme-app/%.o: $(SRCDIR)/%.$(SRCEXT)
	echo "Compiling..."
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/mme-app/contextManager
	@mkdir -p $(OBJDIR)/mme-app/interfaces
	@mkdir -p $(OBJDIR)/mme-app/msgHandlers
	@mkdir -p $(OBJDIR)/mme-app/actionHandlers
	@mkdir -p $(OBJDIR)/mme-app/sec
	@mkdir -p $(OBJDIR)/mme-app/mmeGrpcServer
	@mkdir -p $(OBJDIR)/mme-app/mmeStates
	@mkdir -p $(OBJDIR)/mme-app/utils
	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

$(OBJDIR)/mme-app/%.o : $(SRCDIR)/%.$(CEXT)
	echo "Compiling C files..."
	@mkdir -p $(OBJDIR)/mme-app
	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

all:$(TARGET)

clean:
	@echo "Cleaning...";
	-@rm -rf $(OBJDIR)/mme-app $(TARGET)

install:
	@echo "Installing mme-app"
	-@mkdir -p $(TARGET_DIR)/bin
	-@cp $(TARGET) $(TARGET_DIR)/bin
	-@cp run.sh $(TARGET_DIR)
	-@cp stop.sh $(TARGET_DIR)
	-@mkdir -p $(TARGET_DIR)/conf
	-@cp conf/*.json $(TARGET_DIR)/conf/

.PHONY: clean


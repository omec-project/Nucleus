#
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# SPDX-License-Identifier: Apache-2.0
#

#SRCDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include ../../Makefile.common

LIB_PATH +=-L../common/ -L./asn1c/asnGenFiles

SRCDIR := .
TARGET := $(BINDIR)/s1ap-app
S1AP_CONF = s1ap.json

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/s1ap/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS += -Wall

ifeq ($(DEBUG),true)
	CFLAGS += -g
endif

ifeq ($(DEBUG),false)
	CFLAGS += -O3
endif

INCS := $(INC_DIRS)

LIBS := -lpthread \
	-lsctp \
	-linterface \
	-llog \
	-lthreadpool \
	-ljson \
	-lasncodec \
	-lsecutil \
	-lmonitor_config

LIBS += -L/usr/local/bin/ -L/usr/local/lib -lcrypto
$(TARGET): $(OBJECTS)
	@echo " Linking..."
	-@mkdir -p $(BINDIR)
	$(CC) $(LFLAGS) $^ -o $(TARGET) $(LIB_PATH)  $(LIBS)

$(OBJDIR)/s1ap/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)/s1ap/handlers
	$(CC) $(CFLAGS) $(INCS) -c -o $@ $<

all:$(TARGET)

clean:
	@echo " Cleaning...";
	@rm -rf $(OBJDIR)/s1ap $(TARGET)

install:
	mkdir -p $(TARGET_DIR)/bin/
	cp $(TARGET) $(TARGET_DIR)/bin/
	cp conf/$(S1AP_CONF) $(TARGET_DIR)/conf/


.PHONY: clean


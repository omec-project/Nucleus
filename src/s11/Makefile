#
# Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
# Copyright (c) 2017 Intel Corporation
# Copyright (c) 2019, Infosys Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
include ../../Makefile.common

LIB_PATH +=-L../common/

SRCDIR := .
TARGET := $(BINDIR)/s11-app
S11_JSON = s11.json

SRCEXT := c

SOURCES := $(shell find $(SRCDIR) -type f -name '*.$(SRCEXT)')

OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/s11/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS += -Wall

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
	g++ $(LFLAGS) $^ -o $(TARGET) $(LIB_PATH) $(LIBS)

$(OBJDIR)/s11/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)/s11/handlers
	$(CC) $(CFLAGS) $(INC_DIRS) -c -o $@ $<

all:$(TARGET)

clean:
	@echo " Cleaning...";
	@rm -rf $(OBJDIR)/s11 $(TARGET)

install:
	@mkdir -p $(TARGET_DIR)/bin/
	-@cp $(TARGET) $(TARGET_DIR)/bin/
	-@cp conf/$(S11_JSON) $(TARGET_DIR)/conf/

.PHONY: clean


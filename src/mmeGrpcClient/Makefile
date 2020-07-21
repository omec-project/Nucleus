# 
# Copyright 2019-present Infosys Limited
# 
# SPDX-License-Identifier: Apache-2.0
# 

include ../../Makefile.common

CFLAGS := -Wall -I$(GRPC_ROOT)/include -I$(GRPC_ROOT)/third_party/protobuf/src -std=c++11
LDFLAGS := -L$(GRPC_ROOT)/libs/opt -lgrpc++ -lgrpc -lgpr -L$(GRPC_ROOT)/libs/opt/protobuf -lprotobuf

SRCDIR := .
SRCS := ./mmeGrpcClient.cpp
OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/mmeGrpcClient/%,$(SRCS:.cpp=.o))

TARGET := $(BINDIR)/mme-grpc-client

all: $(TARGET)

$(TARGET) : $(OBJS) 
	@mkdir -p $(BINDIR)
	g++ -o $(TARGET) $(OBJS) $(LIB_PATH) -lmmeGrpcProtoBuf $(LDFLAGS)
	
$(OBJS) : $(OBJDIR)/mmeGrpcClient/%.o : $(SRCDIR)/%.cpp
	-@mkdir -p $(OBJDIR)/mmeGrpcClient
	g++ $(CFLAGS) $(INC_DIRS) -c -o $@ $<

install :
	-@mkdir -p $(TARGET_DIR)/bin
	-@cp $(TARGET) $(TARGET_DIR)/bin

clean:
	-@rm -rf $(OBJDIR)/mmeGrpcClient $(TARGET)

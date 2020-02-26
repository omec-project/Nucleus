#
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

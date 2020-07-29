# Copyright 2020-present Open Networking Foundation
#
# SPDX-License-Identifier: LicenseRef-ONF-Member-Only-1.0

include ../../../Makefile.common

CFLAGS += -D_FORTIFY_SOURCE=2 -fasynchronous-unwind-tables -fexceptions
CFLAGS += -fstack-protector-all -fstack-protector-strong -Wall
CFLAGS += -Werror=format-security 


CC := g++
CFLAGS += -std=c++11 

DEBUG = true
ifeq ($(DEBUG),true)
        CFLAGS += -g
endif
ifeq ($(DEBUG),false)
        CFLAGS += -O3
endif

S11_LIBNAME = libs11cpputil.so 

GTPLIB_SRCS := ./gtp_tables.cpp \
			   ./gtp_cpp_wrapper.cpp 

SRCDIR := .
SRCEXT := cpp
GTP_CPP_OBJS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(GTPLIB_SRCS:.$(SRCEXT)=.o))

OBJS := $(GTP_CPP_OBJS)

all : buildCommonLIBS install

buildCommonLIBS : $(OBJS)
	@echo "Linking"
	@mkdir -p $(LIBDIR)
	$(CC) $(CFLAGS) $(GTP_CPP_OBJS) -shared -o $(S11_LIBNAME)

$(OBJS) : $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)/
	$(CC) $(CFLAGS) $(INC_DIRS) -fPIC -c -o $@ $<

install: 
	@echo "Installing**************************"
	-@mkdir -p $(TOPDIR)/target/lib
	-@cp $(S11_LIBNAME) $(TOPDIR)/target/lib

clean:
	@echo " Cleaning...";
	-@rm -rf $(OBJDIR)/
	-@rm -f $(S11_LIBNAME)
	-@rm -f $(TOPDIR)/target/lib/$(S11_LIBNAME)


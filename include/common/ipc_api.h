/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright 2019-present, Infosys Limited.
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IPC_API_H_
#define IPC_API_H_

#include <tipcTypes.h>

#define		IPC_MAX_BUFFER_SIZE		1024

#define IPC_MODE 0664

enum ipc_access_mode {
	IPC_READ = 01,
	IPC_WRITE = 02,
};

typedef int ipc_handle;

int
create_ipc_channel(char *name);

int
open_ipc_channel(char *name, enum ipc_access_mode access_mode);

int
create_open_ipc_channel(char *name,
		enum ipc_access_mode access_mode);

int
read_ipc_channel(ipc_handle fd, char *buffer, size_t size);

int
write_ipc_channel(ipc_handle fd, char *buffer, size_t size);

int
close_ipc_channel(ipc_handle fd);

int
create_tipc_socket();

int
bind_tipc_socket(int sockFd, uint32_t instanceNum);

int
send_tipc_message(int sd, uint32_t destAddr, void * buf, int len);

int
read_tipc_msg(int sockFd, void * buf, int len);

void
close_tipc_socket(int sockFd);


#endif /* IPC_API_H_ */

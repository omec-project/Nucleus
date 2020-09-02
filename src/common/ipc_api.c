/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2019, Infosys Ltd.
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/tipc.h>
#include <stdint.h>
#include "log.h"
#include "ipc_api.h"
#include "err_codes.h"

#include <sys/socket.h>
#include <string.h>

#define TIPC_SERVICE_ADDR       2

int
create_ipc_channel(char *name)
{
	if (mkfifo (name, IPC_MODE) == -1) {
		log_msg(LOG_ERROR, "Error in create_ipc_channel %s\n", name);
		perror("Error:");
		return -1;
	}

	return 0;
}

int
open_ipc_channel(char *name, enum ipc_access_mode access_mode)
{
	int mode = O_RDONLY;
	int fd;

	if (access_mode == IPC_WRITE)
		mode = O_WRONLY;

	if ((fd = open(name, mode)) == -1) {
		log_msg(LOG_ERROR, "Error in create_ipc_channel %s\n",name);
		perror("Error:");
		return -E_FAIL;
	}

	return fd;
}

int
create_open_ipc_channel(char *name, enum ipc_access_mode access_mode)
{
	if (create_ipc_channel(name) != 0)
		return -1;

	return open_ipc_channel(name, access_mode);
}

int
read_ipc_channel(ipc_handle fd, char *buffer, size_t size)
{
	int len = read(fd, buffer, size);
	switch (len) {
	case -1:
            // case -1 means pipe is empty and errono
            // set EAGAIN
		if (errno == EAGAIN) {
		log_msg(LOG_ERROR, "pipe empty \n");
                usleep(5);
                return -1;
            }
            else { perror("read");
                exit(4);
            }

        // case 0 means all bytes are read and EOF(end of conv.)
        case 0:
            log_msg(LOG_ERROR, "End of conversation\n");

            // read link
            //close(p[0]);

            exit(0);
        default:
            // text read
            // by default return no. of bytes
            // which read call read at that time
            return len;
        }
}

int
write_ipc_channel(ipc_handle fd, char *buffer, size_t size)
{
	return write(fd, buffer, size);
}

int
close_ipc_channel(ipc_handle fd)
{
	if (close(fd) == -1)
		return -1;

	return 0;
}

int
create_tipc_socket()
{
    int sockFd = socket(AF_TIPC, SOCK_RDM, 0);

    if (sockFd <= 0)
    {
        log_msg(LOG_INFO, "Failed to create tipc socket error: %s", strerror(errno));
    }

    return sockFd;
}

int
bind_tipc_socket(int sockFd, uint32_t instanceNum)
{
    struct sockaddr_tipc server;

    server.family = AF_TIPC;
    server.addrtype = TIPC_SERVICE_ADDR;
    server.scope = TIPC_CLUSTER_SCOPE;
    server.addr.name.name.type = tipcServiceAddressType_c;
    server.addr.name.name.instance = instanceNum;

    int rc = 1;
    if (0 != bind(sockFd, (void *)&server, sizeof(server)))
    {
        log_msg(LOG_ERROR, "Server: failed to bind port name %s\n", strerror(errno));
        rc = -1;
    }
    else
    {
	log_msg(LOG_INFO, "Server: Success %s %d\n", strerror(errno), rc);
    }
    return rc;
}

int
send_tipc_message(int sd, uint32_t destAddr, void * buf, int len)
{
    struct sockaddr_tipc server;
    server.family = AF_TIPC;
    server.addrtype = TIPC_SERVICE_ADDR;
    server.scope = TIPC_CLUSTER_SCOPE;
    server.addr.name.domain = 0;
    server.addr.name.name.type = tipcServiceAddressType_c;
    server.addr.name.name.instance = destAddr;

    int rc = 0;
    if (0 > sendto(sd, buf, len, 0, (void*)&server, sizeof(server)))
    {
    	log_msg(LOG_ERROR, "FAILED TO SENT TIPC MESSAGE %s\n", strerror(errno));
    } else {
    	rc = 1;
    }

    return rc;
}

int
read_tipc_msg(int sockFd, void * buf, int len)
{
    int bytesRead = 0;

    if ((bytesRead = recv(sockFd, buf, len, 0)) <= 0)
    {
    	log_msg(LOG_ERROR, "FAILED TO READ TIPC MESSAGE %s\n", strerror(errno));
    }
    return bytesRead;
}

void
close_tipc_socket(int sockFd)
{
	close(sockFd);
}




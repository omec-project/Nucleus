/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <pthread.h>
#include <errno.h>

#include "log.h"
#include "sctp_conn.h"

#include "s1apContextManager/s1apContextWrapper_c.h"
#define ENB_PORT 62324
#define S1AP_PPID 18
int create_sctp_socket(unsigned int remote_ip, unsigned short port)
{
	int connSock;
	struct sockaddr_in servaddr;
	int ret;

	connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

	if (connSock == -1)
	{
		log_msg(LOG_ERROR, "Socket creation failed");
		return -1;
	}

	//TODO: use remote_ip
	bzero ((void *) &servaddr, sizeof (servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(remote_ip);
	//servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons(port);

	ret = bind(connSock, (struct sockaddr *) &servaddr, sizeof (servaddr));

	if(ret == -1 )
	{
		perror("Bind:");
		log_msg(LOG_ERROR, "Bind failed ");
		close(connSock);
		return -1;
	}

	/* Specify maximum streams available per socket */
	struct sctp_initmsg initmsg;
	memset (&initmsg, 0, sizeof (initmsg));
	initmsg.sinit_num_ostreams = MAX_STREAM;
	initmsg.sinit_max_instreams = MAX_STREAM;
	initmsg.sinit_max_attempts = 4;
	ret = setsockopt(connSock, IPPROTO_SCTP, SCTP_INITMSG,
			&initmsg, sizeof (initmsg));

	if(ret == -1 )
	{
		log_msg(LOG_ERROR, "setsockopt() failed ");
		close(connSock);
		return -1;
	}
	int mtu = 1000;
	ret = setsockopt(connSock, SOL_SCTP, SCTP_MAXSEG,&mtu, sizeof(mtu));
	if(ret == -1 )
	{
		log_msg(LOG_ERROR, "1 setsockopt() failed %s ",strerror(errno));
	}

	ret = listen(connSock, MAX_PENDING_CONN);
	if(ret == -1 )
	{
		log_msg(LOG_ERROR, "listen() failed ");
		close(connSock);
		return -1;
	}
	return connSock;
}

int accept_sctp_socket(int connSock)
{
	return accept(connSock, (struct sockaddr *) NULL, (socklen_t *) NULL);
}

int recv_sctp_msg(int connSock, unsigned char *buffer, size_t len)
{
    struct sctp_sndrcvinfo sndrcvinfo = {0};
	int flags=0;
	return sctp_recvmsg(connSock, buffer, len,
			(struct sockaddr *) NULL, 0, &sndrcvinfo, &flags);
}

pthread_mutex_t sctp_sock_mutex;	/* protection to sctp_sock */
int send_sctp_msg(int cbIndex, unsigned char *buffer, size_t len, uint16_t stream_no)
{
	uint32_t ppid = S1AP_PPID;
    uint32_t enb_fd = getEnbFdWithCbIndex(cbIndex);
	pthread_mutex_lock(&sctp_sock_mutex);
	int ret = sctp_sendmsg(enb_fd, (void *)buffer, len,
			NULL, 0, htonl(ppid), 0, stream_no, 0, 0);
	pthread_mutex_unlock(&sctp_sock_mutex);
	return ret;
}

int send_sctp_msg_with_fd(int connSock, unsigned char *buffer, size_t len, uint16_t stream_no)
{
	uint32_t ppid = S1AP_PPID;
	int ret = sctp_sendmsg(connSock, (void *)buffer, len,
			NULL, 0, htonl(ppid), 0, stream_no, 0, 0);
	return ret;
}

int close_sctp_socket(int connSock)
{
	return close(connSock);
}

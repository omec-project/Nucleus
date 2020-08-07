/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2019, Infosys Ltd.
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "thread_pool.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s11.h"
#include "s11_config.h"
#include <sys/types.h>
#include "msgType.h"
#include "s11_options.h"
#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"

/**Global and externs **/
extern s11_config_t g_s11_cfg;
/*S11 CP communication parameters*/
int g_s11_fd;
struct sockaddr_in g_s11_cp_addr;
socklen_t g_s11_serv_size;
struct sockaddr_in g_client_addr;
socklen_t g_client_addr_size;
int ipc_reader_tipc_s11;

/*Connections to send response(CS/MB) to mme-app*/
int g_resp_fd;

pthread_t tipcReaderS11_t;

pthread_mutex_t s11_net_lock = PTHREAD_MUTEX_INITIALIZER;

struct thread_pool *g_tpool;
struct thread_pool *g_tpool_tipc_reader_s11;
/**End: global and externs**/

extern char processName[255];
extern int pid;

#define S11_IPC_MSG_BUF_LEN 1024

void
handle_mmeapp_message_s11(void * data)
{
	char *msg = ((char *) data) + (sizeof(uint32_t)*2);

	msg_type_t* msg_type = (msg_type_t*)(msg);

	switch(*msg_type)
	{
	case create_session_request:
		create_session_handler(msg);
		break;
	case modify_bearer_request:
		modify_bearer_handler(msg);
		break;
	case delete_session_request:
		delete_session_handler(msg);
		break;
	case release_bearer_request:
		release_bearer_handler(msg);
		break;
	case ddn_acknowledgement:
		ddn_ack_handler(msg);
		break;
	default:
		break;
	}
	free(data);
}

void * tipc_msg_handler_s11()
{
	int bytesRead = 0;
	unsigned char buffer[S11_IPC_MSG_BUF_LEN] = {0};
	while (1)
	{
		if ((bytesRead = read_tipc_msg(ipc_reader_tipc_s11, buffer, S11_IPC_MSG_BUF_LEN)) > 0)
		{
			unsigned char *tmpBuf = (unsigned char *) malloc(sizeof(char) * bytesRead);
			memcpy(tmpBuf, buffer, bytesRead);
			log_msg(LOG_INFO, "S11 message received from mme-app, bytesRead %d", bytesRead);
			insert_job(g_tpool_tipc_reader_s11, handle_mmeapp_message_s11, tmpBuf);
		}
		bytesRead = 0;
	}
}
struct GtpV2Stack* gtpStack_gp = NULL;

int
init_s11_workers()
{
	if ((ipc_reader_tipc_s11 = create_tipc_socket()) <= 0)
	{
		log_msg(LOG_ERROR, "Failed to create IPC Reader tipc socket \n");
		return -E_FAIL;
	}
	if ( bind_tipc_socket(ipc_reader_tipc_s11, s11AppInstanceNum_c) != 1)
	{
		log_msg(LOG_ERROR, "Failed to bind IPC Reader tipc socket \n");
		return -E_FAIL;
	}

	/* Initialize thread pool for mme-app messages */
	g_tpool_tipc_reader_s11 = thread_pool_new(3);

	if (g_tpool_tipc_reader_s11 == NULL) {
		log_msg(LOG_ERROR, "Error in creating thread pool. \n");
		return -E_FAIL_INIT;
	}

	log_msg(LOG_INFO, "S11 Listener thead pool initalized.\n");

	// thread to read incoming ipc messages from tipc socket
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tipcReaderS11_t, &attr, &tipc_msg_handler_s11, NULL);
	pthread_attr_destroy(&attr);

	return 0;
}

/*Initialize sctp socket connection for eNB*/
int
init_gtpv2()
{
	/*Create UDP socket*/
	g_s11_fd = socket(PF_INET, SOCK_DGRAM, 0);

	g_client_addr.sin_family = AF_INET;
	//g_client_addr.sin_addr.s_addr = htonl(g_s11_cfg.local_egtp_ip);
	struct in_addr mme_local_addr = {g_s11_cfg.local_egtp_ip};
	fprintf(stderr, "....................local egtp %s\n", inet_ntoa(mme_local_addr));
	g_client_addr.sin_addr.s_addr = htonl(g_s11_cfg.local_egtp_ip);
	g_client_addr.sin_port = htons(g_s11_cfg.egtp_def_port);

	bind(g_s11_fd, (struct sockaddr *)&g_client_addr, sizeof(g_client_addr));
	g_client_addr_size = sizeof(g_client_addr);

	/*Configure settings in address struct*/
	g_s11_cp_addr.sin_family = AF_INET;
	//g_s11_cp_addr.sin_port = htons(g_s11_cfg.egtp_def_port);
	fprintf(stderr, ".................... egtp def port %d\n", g_s11_cfg.egtp_def_port);
	g_s11_cp_addr.sin_port = htons(g_s11_cfg.egtp_def_port);
	//g_s11_cp_addr.sin_addr.s_addr = htonl(g_s11_cfg.sgw_ip);
	struct in_addr sgw_addr = {g_s11_cfg.sgw_ip};
	fprintf(stderr, "....................sgw ip %s\n", inet_ntoa(sgw_addr));
	g_s11_cp_addr.sin_addr.s_addr = g_s11_cfg.sgw_ip;
	memset(g_s11_cp_addr.sin_zero, '\0', sizeof(g_s11_cp_addr.sin_zero));

	g_s11_serv_size = sizeof(g_s11_cp_addr);

	return SUCCESS;
}

int
init_s11_ipc()
{
	log_msg(LOG_INFO, "Connecting to mme-app S11 CS response queue\n");
	if ((g_resp_fd  = create_tipc_socket()) <= 0)
		return -E_FAIL;

	log_msg(LOG_INFO, "S11 - mme-app IPC: Connected.\n");

	return 0;
}

/**
  Read incoming S11 messages and pass to threadpool
  for processing.
*/
void
s11_reader()
{
	unsigned char buffer[S11_GTPV2C_BUF_LEN];
	int len;

	while(1) {
		//len = recvfrom(g_s11_fd, buffer, S11_GTPV2C_BUF_LEN, 0,
		//	&g_client_addr, &g_client_addr_size);
		len = recvfrom(g_s11_fd, buffer, S11_GTPV2C_BUF_LEN, 0,
			(struct sockaddr*)&g_s11_cp_addr, &g_s11_serv_size);

		if(len > 0) {
			MsgBuffer* tmp_buf_p = createMsgBuffer(len);
			uint32_t ip = ntohl(g_s11_cp_addr.sin_addr.s_addr);
			MsgBuffer_writeUint32(tmp_buf_p, ip, true);
			MsgBuffer_writeBytes(tmp_buf_p, buffer, len, true);
			MsgBuffer_rewind(tmp_buf_p);

			log_msg(LOG_INFO, "S11 Received msg len : %d \n",len);

			insert_job(g_tpool, handle_s11_message, tmp_buf_p);
		}

	}
}


int
main(int argc, char **argv)
{
	memcpy (processName, argv[0], strlen(argv[0]));
	pid = getpid();
	
	init_backtrace(argv[0]);

	char *hp = getenv("MMERUNENV");
	if (hp && (strcmp(hp, "container") == 0)) {
		init_logging("container", NULL);
	}
	else { 
		init_logging("hostbased","/tmp/s11logs.txt" );
	}

    init_cpp_gtp_tables();

	init_parser("conf/s11.json");
	parse_s11_conf();

	// init stack
	gtpStack_gp = createGtpV2Stack();
	if (gtpStack_gp == NULL)
	{
		log_msg(LOG_ERROR, "Error in initializing ipc.\n");
		return -1;
	}

	/*Init writer sockets*/
	if (init_s11_ipc() != 0) {
		log_msg(LOG_ERROR, "Error in initializing ipc.\n");
		return -1;
	}

	init_s11_workers();

	/* Initialize thread pool for S11 messages from CP*/
	g_tpool = thread_pool_new(S11_THREADPOOL_SIZE);

	if (g_tpool == NULL) {
		log_msg(LOG_ERROR, "Error in creating thread pool. \n");
		return -1;
	}
	log_msg(LOG_INFO, "S11 listener threadpool initialized.\n");

	if (init_gtpv2() != 0)
		return -1;

	s11_reader();

	return 0;
}

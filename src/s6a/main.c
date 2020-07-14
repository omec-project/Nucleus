/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2019, Infosys Ltd.
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>
#include <freeDiameter/libfdproto.h>
#include "s6a_config.h"
#include "s6a.h"
#include "s6a_fd.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "hss_message.h"
#include "thread_pool.h"
#include <sys/types.h>
#include "msgType.h"
/**Globals and externs**/
struct fd_dict_objects g_fd_dict_objs;
struct fd_dict_data g_fd_dict_data;
int g_Q_mme_S6a_fd;

int g_our_hss_fd;
struct thread_pool *g_tpool;
struct thread_pool *g_tpool_tipc_reader_s6a;
extern s6a_config g_s6a_cfg;

pthread_t g_AIR_handler_tid, g_ULR_handler_tid;
pthread_t g_detach_handler_tid;
pthread_t g_our_hss_tid;

extern char processName[255];
extern int pid;

int ipc_reader_tipc_s6;

extern void*
S6Req_handler(void *data);

extern void*
detach_handler(void *data);
/**Globals and externs**/

/**
 * @brief Initialize communication channel IPC or IPC to non freediameter HSS
 * ready
 * @param  None
 * @return void
 */
void
init_hss_rpc()
{
	struct sockaddr_un hss_serv;

	g_our_hss_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (g_our_hss_fd < 0) {
		log_msg(LOG_ERROR, "HSS socket creation failed.\n");
		perror("Error opening HSS socket");
		exit(-1);
	}

	hss_serv.sun_family = AF_UNIX;
	strcpy(hss_serv.sun_path, g_s6a_cfg.hss_ipc_endpt);

	if (connect(g_our_hss_fd, (struct sockaddr *)&hss_serv,
	sizeof(struct sockaddr_un)) < 0) {
		log_msg(LOG_ERROR, "HSS connect failed.\n");
		perror("connecting HSS socket");
		close(g_our_hss_fd);
		exit(-1);
	}
	log_msg(LOG_INFO, "Connected to HSS\n");
}

/**
 * @brief Initialize freediameter library, dictionary and data elements
 * ready
 * @param  None
 * @return int SUCCESS or S6A_FD_ERROR
 */
static int
init_fd()
{
	log_msg(LOG_INFO, "INIT FD .. .\n");

	/* Initialize the core freeDiameter library */
	CHECK_FCT_DO(fd_core_initialize(), return S6A_FD_ERROR);

	/* Parse the configuration file */
	CHECK_FCT_DO(fd_core_parseconf(S6A_FD_CONF), return S6A_FD_ERROR);

	if(SUCCESS != s6a_fd_init()) exit(S6A_FD_ERROR);

	if(SUCCESS != s6a_fd_objs_init()) exit(S6A_FD_ERROR);

	if(SUCCESS != s6a_fd_data_init()) exit(S6A_FD_ERROR);

	if(SUCCESS != s6a_fd_cb_reg()) exit(S6A_FD_ERROR);

	CHECK_FCT_DO( fd_core_start(), return S6A_FD_ERROR);

	return SUCCESS;
}

/**
 * @brief Unused
 * ready
 * @param
 * @return i
 */
static void
check_args(int argc, char **argv)
{
	/*Parsse arguments to extract file path*/
	/*If no file path mentioned then use default*/
	/*For wrong arguments print help*/
	return;
}
void
handle_mmeapp_message_s6a(void * data)
{
	char *msg = ((char *) data) + (sizeof(uint32_t)*2);

	msg_type_t* msg_type = (msg_type_t*)(msg);

	switch(*msg_type)
	{
	case auth_info_request:
	case update_loc_request:
		S6Req_handler(msg);
		break;
	case purge_request:
		detach_handler(msg);
		break;
	default:
		break;
	}
	free(data);
}
void * AIR_handler(void * data)
{
	int bytesRead = 0;
	while (1)
	{
		unsigned char buffer[1024] = {0};
		if ((bytesRead = read_tipc_msg(ipc_reader_tipc_s6, buffer, 1024)) > 0)
		{
			unsigned char *tmpBuf = (unsigned char *) malloc(sizeof(char) * bytesRead);
			memcpy(tmpBuf, buffer, bytesRead);
			log_msg(LOG_INFO, "S6 message received from mme-app");
			insert_job(g_tpool_tipc_reader_s6a, handle_mmeapp_message_s6a, tmpBuf);
		}
		bytesRead = 0;
	}
}

/**
 * @brief Initialize s6a application message handlers
 * ready
 * @param None
 * @return int SUCCESS or FAIL
 */
static int
init_handlers()
{
	if ((ipc_reader_tipc_s6 = create_tipc_socket()) <= 0)
	{
		log_msg(LOG_ERROR, "Failed to create IPC Reader tipc socket \n");
		return -E_FAIL;
	}
	if ( bind_tipc_socket(ipc_reader_tipc_s6, s6AppInstanceNum_c) != 1)
	{
		log_msg(LOG_ERROR, "failed to bind port name %s\n", strerror(errno));
		return -E_FAIL;
	}

	g_tpool_tipc_reader_s6a = thread_pool_new(3);

	if (g_tpool_tipc_reader_s6a == NULL) {
		log_msg(LOG_ERROR, "Error in creating thread pool. \n");
		return -E_FAIL_INIT;
	}
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	/* set the thread detach state */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&g_AIR_handler_tid, &attr, &AIR_handler, NULL);

	pthread_attr_destroy(&attr);
	return 0;
}

/**
 * @brief initialize s6a application IPC mechanism, queues
 * ready
 * @param None
 * @return int as SUCCESS or FAIL. exit() in case of error.
 */
static int
init_s6a_ipc()
{
	g_Q_mme_S6a_fd = create_tipc_socket();
	if (g_Q_mme_S6a_fd == -1) {
		log_msg(LOG_ERROR, "Error in opening writer IPC channel\n");
		pthread_exit(NULL);
	}
	log_msg(LOG_INFO, "S6a response - mme-app TIPC: Connected.\n");
	return 0;
}

/**
 * @brief HSS message listener. Listen hss response and delegate to thread pool
 * case of dummy hss
 * ready
 * @param None
 * @return void
 */
void
s6a_run()
{
	unsigned char buf[HSS_RCV_BUF_SIZE];
	int len;

	/*If using in build perf-hss then start thread to handle it's responses*/
	if(HSS_FD == g_s6a_cfg.hss_type) {
		/*Use main thread for ULR or stats etc.*/
		while(1) {
			sleep(10);
		}
	} else {

	while(1) {
		bzero(buf, sizeof(buf));

		if ((len = read(g_our_hss_fd, buf,
						sizeof(struct hss_resp_msg))) < 0) {
			log_msg(LOG_ERROR, "Error reading hss buff\n");
	                perror("reading stream message");
			exit(-1);
		} else if (len == 0) {
			log_msg(LOG_ERROR, "Error reading hss buff\n");
	                perror("reading stream message");
			exit(-1);
		}else {
			unsigned char *tmp_buf = (unsigned char *)
					calloc(sizeof(char), len);
			memcpy(tmp_buf, buf, len);
			log_msg(LOG_INFO, "HSS Received msg len : %d \n",len);
			insert_job(g_tpool, hss_resp_handler, tmp_buf);
		}
	}
	} /*else - HSS_PERF*/
}


/**
 * brief main for s6a application
 * @param argc and argv
 * @return int - program's exit code
 */
int
main(int argc, char **argv)
{
	srand(time(0));
	memcpy (processName, argv[0], strlen(argv[0]));
	pid = getpid();
	
	init_backtrace(argv[0]); 

	char *hp = getenv("MMERUNENV");
	if (hp && (strcmp(hp, "container") == 0)) {
			init_logging("container", NULL);
	}
	else { 
		init_logging("hostbased", "/tmp/s6alogs.txt");
	}
	/*Check cmd line arguments for config file path*/
	check_args(argc, argv);
	
	init_parser("conf/s6a.json");
	parse_s6a_conf();

	if(HSS_FD == g_s6a_cfg.hss_type){
		/*Initialize free diameter*/
		init_fd();
	} else {
		init_hss_rpc();
		/* Initialize thread pool for handling HSS resp*/
		g_tpool = thread_pool_new(HSS_RESP_THREADPOOL_SIZE);
		if (g_tpool == NULL) {
			log_msg(LOG_ERROR, "Error in creating thread pool. \n");
			return -1;
		}
	}

	init_s6a_ipc();

	/*Initialize listner for AIR and ULR from mme-app*/
	init_handlers();

	s6a_run();

	return 0;
}

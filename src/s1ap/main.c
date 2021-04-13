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
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>

#include "options.h"
#include "ipc_api.h"
#include "main.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "sctp_conn.h"
#include "s1ap_structs.h"
#include "thread_pool.h"
#include "tpool_queue.h"
#include "snow_3g.h"
#include "f9.h"
#include "err_codes.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/cmac.h>
#include "s1apContextManager/s1apContextWrapper_c.h"
#include "s1_common_types.h"

s1ap_instance_t *s1ap_inst;

/*Global and externs **/
pthread_t s1ap_iam_t;

int g_sctp_fd = 0;
struct thread_pool *g_tpool;
struct thread_pool *g_tpool_tipc_reader;

ipc_handle ipc_S1ap_Hndl;

ipc_handle ipc_tipc_reader;

ipc_handle ipcHndl_sctpsend_reader;
ipc_handle ipcHndl_sctpsend_writer;

pthread_t tipcReader_t;
pthread_t acceptSctp_t;

struct time_stat g_attach_stats[65535];
/**End: global and externs**/

extern char processName[255];

extern void
handle_mmeapp_message(void * data);

#define MAX_ENB     32
#define BUFFER_LEN  4096
#define AES_128_KEY_SIZE 16

char *msg_to_hex_str(const char *msg, int len, char **buffer) {

  char chars[]= "0123456789abcdef";
  char *local;

  if (!len)
      return NULL;

  if (!((*buffer) = (char *)malloc(2 * len + 1)))
      return NULL;

  local = *buffer;
  for (int i = 0; i < len; i++) {
      local[2 * i] = chars[(msg[i] >> 4) & 0x0F];
      local[2 * i + 1] = chars[(msg[i]) & 0x0F];
  }
  local[2 * len] = '\0';

  return local;
}

unsigned short get_length(char **msg) {
    /* get length and consume msg bytes accordingly */

    unsigned short ie_len = 0;

    unsigned char val = ((*msg)[0] & 0xc0) >> 6;
    if(val == 2) {
        //log_msg(LOG_INFO, "length more than 128\n");
        unsigned short higher = (unsigned char)(*msg)[0] & 0x3f;
        (*msg)++;
        unsigned short lower = (unsigned char)(*msg)[0];
        ie_len = (higher << 8) | lower;
    } else {
        //log_msg(LOG_INFO, "length less than 128\n");
        ie_len = (unsigned short)(*msg)[0];
    }
    (*msg)++;
    return ie_len;
}

/**
 * @brief Decode int value from the byte array received in the s1ap incoming
 * packet.
 * @param[in] bytes - Array of bytes in packet
 * @param[in] len - Length of the bytes array from which to extract the int
 * @return Integer value extracted out of bytes array. 0 if failed.
 */
int
decode_int_val(unsigned char *bytes, short len)
{
	switch(len) {
		case 1:
		case 2:
			return (bytes[1] & 0xff);

		case 3:
			return (bytes[2] & 0xff) |
					(0xff00 & ((unsigned short)(bytes[1] << 8)));

		case 4:
			return (((((unsigned int)(bytes[1]) << 16) & 0xffff00) |
					((unsigned int)(bytes[2]) << 8)) & 0xffff00) |
					((unsigned int)(bytes[3]) & 0xff);
	};
	return 0;
}

/**
 * @brief Pack short number value in to the buffer
 * @param[out] buffer to fill the value
 * @param[in] value to fill
 * @return number of bytes filled in to the buffer
 */
int
copyU16(unsigned char *buffer, uint32_t val)
{
	if (val < 255) {
		buffer[0] = (val >> 8) & 0xFF;
		buffer[1] = val & 0xFF;
		return 2;
	} else if (val < 65535) {
		buffer[0] = 0x40;
		buffer[1] = (val >> 8) & 0xFF;
		buffer[2] = val & 0xFF;
		return 3;
	} else {
		buffer[0] = 0x80;
		buffer[1] = (val >> 16) & 0xFF;
		buffer[2] = (val >> 8) & 0xFF;
		buffer[3] = val & 0xFF;
		return 4;
	}
}

int
init_s1ap_workers()
{
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	/* set the thread detach state */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_attr_destroy(&attr);
	return 0;
}

/**
 * @brief Thread to read and distribute sctp request received. Messages will
 * be passed to s1ap handler function for further processing.
 * @param[in] thread data
 * @return thread return data
 */
void *
accept_sctp(void *data)
{
	log_msg(LOG_INFO, "accept connection on sctp sock");
	int new_socket = 0;
	int activity = 0;
	int i = 0;
	int valread = 0;
	int sd = 0;
	int max_sd = 0;
	int enb_socket[MAX_ENB] = {0};
	unsigned char buffer[BUFFER_LEN] = {0};

	while(1) {
		fd_set readfds;
		memset (buffer, 0, BUFFER_LEN);
		valread  = 0;

		//TODO performance issue here
		FD_ZERO(&readfds);
		FD_SET(g_sctp_fd, &readfds);
		max_sd = g_sctp_fd;

		for (i = 0; i < MAX_ENB; i++) {
			sd = enb_socket[i];

			if (sd > 0) {
				FD_SET(sd, &readfds);
			}

			if (sd > max_sd) {
				max_sd = sd;
			}
		}

		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR)) {
			log_msg(LOG_ERROR, "select error.");
		 }

		if (FD_ISSET(g_sctp_fd, &readfds)) {

			if ((new_socket = accept_sctp_socket(g_sctp_fd)) == -1) {
				log_msg(LOG_ERROR, "Error in accept on sctp socket.");
			}

			int mtu = 1000;
			int ret = setsockopt(new_socket, SOL_SCTP, SCTP_MAXSEG,&mtu, sizeof(mtu));
			if(ret == -1 ) {
				log_msg(LOG_ERROR, "1 setsockopt() failed %s ",strerror(errno));
			}

			log_msg(LOG_INFO, "New Connection Established.");

			for (i = 0; i < MAX_ENB; i++) {

				if( enb_socket[i] == 0 ) {

					enb_socket[i] = new_socket;
					break;
				}
			}
		}

		for (i = 0; i < MAX_ENB; i++) {

			sd = enb_socket[i];

			if (FD_ISSET(sd, &readfds)) {
				if ((valread = recv_sctp_msg(sd, buffer, SCTP_BUF_SIZE)) <= 0) {
                    struct EnbStruct temp = {0};
                    int temp_cbIndex = findControlBlockWithEnbFd(sd);
                    s1apEnbStatus_Msg_t s1Msg={0};
                    s1Msg.header.msg_type = enb_status_msg; 
                    s1Msg.header.srcInstAddr = htonl(s1apAppInstanceNum_c);
                    s1Msg.header.destInstAddr = htonl(mmeAppInstanceNum_c);
                    s1Msg.ver = 1;
                    s1Msg.status = 0;
                    s1Msg.context_id = temp_cbIndex;
                    send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&s1Msg, sizeof(s1apEnbStatus_Msg_t));
                    
                    clearControlBlockDetailsEnbFd(sd, &temp);
                    log_msg(LOG_ERROR, "enb fd = %d eNB %s - Tac %d - disconnected ", sd, temp.eNbName, temp.tai_m.tac);
                    close(sd);
                    enb_socket[i] = 0;
                    /* MME-app should get notificaiton that peer is down ? 
                     * what MME will do with existing subscribers with the
                     * same eNB ? */

				} else {
					unsigned char *tmpBuf = (unsigned char *) malloc(sizeof(char) * valread + (2*sizeof(int)) );
					memcpy(tmpBuf, &sd, sizeof(sd));
					memcpy(tmpBuf + sizeof(int), &valread, sizeof(int));
					memcpy(tmpBuf + (2*sizeof(int)), buffer, valread);
					insert_job(g_tpool, handle_s1ap_message, tmpBuf);

				}
			}
		}

	}/* while */

	return NULL;
}

void * tipc_msg_handler()
{
	int bytesRead = 0;
	while (1)
	{
		unsigned char buffer[BUFFER_LEN] = {0};
		if ((bytesRead = read_tipc_msg(ipc_tipc_reader, buffer,BUFFER_LEN)) > 0)
		{
			unsigned char *tmpBuf = (unsigned char *) malloc(sizeof(char) * bytesRead);
			memcpy(tmpBuf, buffer, bytesRead);
			insert_job(g_tpool_tipc_reader, handle_mmeapp_message, tmpBuf);
		}
	}
}

/**
 * @brief Initialize sctp socket connection for eNB
 * @return Error code SUCCESS or FAIL
*/
int
init_sctp()
{
	s1ap_config_t *s1ap_cfg = get_s1ap_config();
	
	log_msg(LOG_INFO, "Create sctp sock, ip:%d, port:%d",
			s1ap_cfg->s1ap_local_ip, s1ap_cfg->sctp_port);
	/*Create MME sctp listned socket*/
	g_sctp_fd = create_sctp_socket(s1ap_cfg->s1ap_local_ip,
					s1ap_cfg->sctp_port);

	if (g_sctp_fd == -1) {
		log_msg(LOG_ERROR, "Error in creating sctp socket. ");
		return -E_FAIL;
	}

	pthread_attr_t attr;

	pthread_attr_init(&attr);
	/* set the thread detach state */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	int ret = pthread_create(&acceptSctp_t, &attr,&accept_sctp, NULL);
	if(ret < 0) {
		log_msg(LOG_ERROR,"SCTP ACCEPTS THREAD FAILED");
		return -E_FAIL;
	}

	pthread_attr_destroy(&attr);
	return SUCCESS;
}


/**
 * @brief Init all writer IPC channels for s1ap program
 * @return Success/fail error code
 */
int
init_writer_ipc()
{
	if ((ipc_S1ap_Hndl  = create_tipc_socket()) <= 0)
		return -E_FAIL;

	log_msg(LOG_INFO, "Writer IPCs initialized");

	return SUCCESS;
}

/**
 * @brief Init handlers to process messages coming from mme-app
 * @return error code.
 */
int
start_mme_resp_handlers()
{
	if ((ipc_tipc_reader = create_tipc_socket()) <= 0)
	{
		log_msg(LOG_ERROR, "Failed to create IPC Reader tipc socket ");
		return -E_FAIL;
	}
	if ( bind_tipc_socket(ipc_tipc_reader, s1apAppInstanceNum_c) != 1)
	{
		log_msg(LOG_ERROR, "Failed to bind IPC Reader tipc socket ");
		return -E_FAIL;
	}

	/* Initialize thread pool for mme-app messages */
	g_tpool_tipc_reader = thread_pool_new(5);

	if (g_tpool_tipc_reader == NULL) {
		log_msg(LOG_ERROR, "Error in creating thread pool. ");
		return -E_FAIL_INIT;
	}

	log_msg(LOG_INFO, "S1AP Listener theadpool initalized.");

	// thread to read incoming ipc messages from tipc socket
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tipcReader_t, &attr, &tipc_msg_handler, NULL);
	pthread_attr_destroy(&attr);


	return SUCCESS;
}

/**
 *@Thread to handle send messages to sctp thread
 * NOT user currently.
 */
void *
sctp_write(void *args)
{
	while(1) {

		sleep(10);

	}
}

int
start_sctp_threads()
{
	pthread_t sctp_writer;
	pthread_attr_t attr;
	int res = SUCCESS;

	res = pthread_attr_init(&attr);
	if (res != 0)
		return -E_FAIL;

	res = pthread_attr_setdetachstate(&attr,
			PTHREAD_CREATE_DETACHED);
	if (res != 0)
		return -E_FAIL;

	res = pthread_create(&sctp_writer, &attr,
			sctp_write, NULL);
	if (res != 0) {
		log_msg(LOG_ERROR, "Error in creating sctp writer thread.");
		pthread_attr_destroy(&attr);
		return -E_FAIL;
	}

	pthread_attr_destroy(&attr);

	return SUCCESS;
}

/**
 * @brief - main entry point for s1ap application. Read json config,
 * start all the handlers. Connect with configured enb
 */
int
main(int argc, char **argv)
{
	memcpy (processName, argv[0], strlen(argv[0]));
	
	s1ap_inst = (s1ap_instance_t *) calloc(1, sizeof(s1ap_instance_t));
	s1ap_inst->s1ap_config = (s1ap_config_t *) calloc(1, sizeof(s1ap_config_t));
	
	char *hp = getenv("MMERUNENV");
	if (hp && (strcmp(hp, "container") == 0)) {
		init_logging("container", NULL);
	}
	else { 
		init_logging("hostbased", "/tmp/s1aplogs.txt");
	}
	init_backtrace(argv[0]); 

	parse_args(argc, argv);

	s1ap_parse_config(s1ap_inst->s1ap_config);

	if (init_writer_ipc() != SUCCESS) {
		log_msg(LOG_ERROR, "Error in initializing writer ipc.");
		return -E_FAIL_INIT;
	}

	if (start_mme_resp_handlers() != SUCCESS) {
			log_msg(LOG_ERROR, "Error in starting mme response handlers.");
			return -E_FAIL_INIT;
	}

	/* Initialize thread pool for sctp request parsers */
	g_tpool = thread_pool_new(THREADPOOL_SIZE);

	if (g_tpool == NULL) {
		log_msg(LOG_ERROR, "Error in creating thread pool. ");
		return -E_FAIL_INIT;
	}
	log_msg(LOG_INFO, "S1AP Listener theadpool initalized.");


	if (init_sctp() != SUCCESS) {
		log_msg(LOG_ERROR, "Error in initializing sctp server.");
		return -E_FAIL_INIT;
	}
	log_msg(LOG_INFO, "SCTP socket open - success ");

	if (start_sctp_threads() != SUCCESS) {
		log_msg(LOG_ERROR, "Error in creating sctp reader/writer thread.");
		return -E_FAIL_INIT;
	}
	log_msg(LOG_INFO, "sctp reader/writer thread started.");

	register_config_updates();

	while (1) {
		sleep(10);
	}

	return SUCCESS;
}

void s1ap_parse_config(s1ap_config_t *config)
{
	/*Read MME configurations*/
	init_parser("conf/s1ap.json");
	parse_s1ap_conf(config);
}

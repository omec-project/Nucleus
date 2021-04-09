/*
* Copyright 2019-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*
*
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "thread_pool.h"
#include "tpool_queue.h"
#include "err_codes.h"
#include "unix_conn.h"
#include "unix_sock.h"
#include "log.h"

extern int g_unix_fd; 
extern pthread_t acceptUnix_t;
extern struct thread_pool *g_tpool;

JobFunction monitorConfigFunc_fpg = dummy_monitor_fn;

/**
 * @brief Initialize listen socket connection 
 * @return Error code SUCCESS or FAIL
*/
int
init_sock()
{
	log_msg(LOG_INFO, "Create listen sock");
	/*Create MME unix domain listen socket*/
	g_unix_fd = create_unix_socket();

	if (g_unix_fd == -1) {
		log_msg(LOG_ERROR, "Error in creating unix socket. ");
		return -E_FAIL;
	}

	pthread_attr_t attr;

	pthread_attr_init(&attr);
	/* set the thread detach state */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	int ret = pthread_create(&acceptUnix_t, &attr,&accept_unix, NULL);
	if(ret < 0) {
		log_msg(LOG_ERROR,"UNIX ACCEPTS THREAD FAILED");
		return -E_FAIL;
	}

	pthread_attr_destroy(&attr);
	return SUCCESS;
}

/**
 * @brief Thread to read and distribute unix sock request received. Messages will
 * be passed to monitor handler for further processing.
 * @param[in] thread data
 * @return thread return data
 */
void *
accept_unix(void *data)
{
	log_msg(LOG_DEBUG, "accept connection on unix sock");
	int new_socket = 0;
	int activity = 0;
	int i = 0;
	int valread = 0;
	int sd = 0;
	int max_sd = 0;
	int client_socket[MAX_CLIENT] = {0};

	fd_set readfds;
	unsigned char buffer[BUFFER_LEN] = {0};


	while(1) {

		FD_ZERO(&readfds);
		FD_SET(g_unix_fd, &readfds);
		max_sd = g_unix_fd;

		for (i = 0; i < MAX_CLIENT; i++) {
			sd = client_socket[i];

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

		if (FD_ISSET(g_unix_fd, &readfds)) {
			log_msg(LOG_DEBUG, "stuck in accept.");

			if ((new_socket = accept_unix_socket(g_unix_fd)) == -1) {
				log_msg(LOG_ERROR, "Error in accept on unix socket.");
			}

			log_msg(LOG_DEBUG, "New Connection Established.");
			FD_CLR(g_unix_fd, &readfds);

			for (i = 0; i < MAX_CLIENT; i++) {

				if( client_socket[i] == 0 ) {

					client_socket[i] = new_socket;
					log_msg(LOG_DEBUG, "Adding to list of sockets at %d value %d" , i, new_socket);
					break;
				}
			}
		}

		for (i = 0; i < MAX_CLIENT; i++) {

			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds)) 
			{
			    if ((valread = recv_unix_msg(sd, 
			             buffer, BUF_SIZE)) == 0) 
				{
                                    log_msg(LOG_INFO, "Host Disconnected");
				    close(sd);
				    client_socket[i] = 0;

				}
				else if(valread == -1)
				{
                                    log_msg(LOG_INFO, "Host Disconnected");
				    //close(sd);
				    //client_socket[i] = 0;
				}
				else 
				{
                                    unsigned char *tmpBuf = (unsigned char *)
				    malloc((sizeof(char) * valread)+sizeof(int));
                                    if(tmpBuf == NULL)
				    {
                                        log_msg(LOG_ERROR, "malloc failed");
					continue;
				    }
				    
				    memcpy(tmpBuf, &sd, sizeof(int));
			            memcpy(tmpBuf + sizeof(int), buffer, valread);
				    log_msg(LOG_DEBUG, "Received msg len : %d ",valread);
				    insert_job(g_tpool, monitorConfigFunc_fpg, tmpBuf);

				}
			}
		}

	}/* while */

	return NULL;
}

void dummy_monitor_fn(void* msg)
{
    log_msg(LOG_ERROR, "Unix IPC Socket callback "
                      "not registered by the application!");
    pthread_exit(NULL);
}

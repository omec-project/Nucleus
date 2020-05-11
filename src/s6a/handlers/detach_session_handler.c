/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>
#include <freeDiameter/libfdproto.h>

#include "err_codes.h"
#include "ipc_api.h"
#include "sec.h"
#include "s6a_fd.h"
#include "s6a.h"
#include "s6a_config.h"
#include "msgType.h"


/************************************************************************
Current file : Stage  - AIR handler of S6A
ATTACH stages :
@@@	Stage 1 : IAM-->[stage1 handler]-->AIR, ULR
	Stage 2 : AIA, ULA -->[stage2 handler]--> Auth req
	Stage 3 : Auth resp-->[stage1 handler]-->Sec mode cmd
	Stage 4 : sec mode resp-->[stage1 handler]-->esm infor req
	Stage 5 : esm infor resp-->[stage1 handler]-->create session
	Stage 6 : create session resp-->[stage1 handler]-->init ctx setup
	Stage 7 : attach complete-->[stage1 handler]-->modify bearer
**************************************************************************/

/****Globals and externs ***/

extern int g_Q_mme_S6a_fd;
extern int g_our_hss_fd;
/*Making global just to avoid stack passing*/

extern s6a_config g_s6a_cfg;
extern struct fd_dict_objects g_fd_dict_objs;
extern struct fd_dict_data g_fd_dict_data;
/****Global and externs end***/


/**
 * @brief Prepare PUR freediameter message, dump and post to HSS
 * @param[in] ue_idx UE indx to append to session id
 * @param[in] imsi - IMSI
 * @return int Sucess or failure code
 */
static int
send_purge(struct s6a_Q_msg *pur_msg, char imsi[])
{
	struct msg *fd_msg = NULL;
	union avp_value val;
	struct s6a_sess_info s6a_sess = {.sess_id="", .sess_id_len = 0};

	if(SUCCESS != create_fd_sess_id(&s6a_sess, pur_msg->ue_idx)) return S6A_FD_ERROR;

	CHECK_FCT_DO(fd_msg_new(g_fd_dict_objs.PUR, MSGFL_ALLOC_ETEID, &fd_msg),
			return S6A_FD_ERROR);

	/*AVP: Session-Id*/
	val.os.data = (unsigned char*)s6a_sess.sess_id;
	val.os.len = strlen(s6a_sess.sess_id);
	add_fd_msg(&val, g_fd_dict_objs.sess_id, &fd_msg);

	/*AVP: Auth-Session-State*/
	val.i32 = 1; /*NO_STATE_MAINTAINED*/
	val.os.len = 0;
	add_fd_msg(&val, g_fd_dict_objs.auth_sess_state, &fd_msg);

	/*AVP: Origin-Host/Realm*/
	CHECK_FCT_DO(fd_msg_add_origin(fd_msg, 0), return S6A_FD_ERROR);

	/*AVP: Destination-Host*/
	val.os.data = (unsigned char *)g_s6a_cfg.hss_host_name;
	val.os.len = strlen(g_s6a_cfg.hss_host_name);
	add_fd_msg(&val, g_fd_dict_objs.dest_host, &fd_msg);

	/*AVP: Destination-Realm*/
	val.os.data = (unsigned char*)g_s6a_cfg.realm;
	val.os.len = strlen(g_s6a_cfg.realm);
	add_fd_msg(&val, g_fd_dict_objs.dest_realm, &fd_msg);

	/*AVP: User-Name*/
	val.os.data = (unsigned char*)imsi;
	val.os.len = strlen(imsi);
	add_fd_msg(&val, g_fd_dict_objs.user_name, &fd_msg);

	/*AVP: PUR-Flags*/
	val.u32 = true;
	val.os.len = 0;
	add_fd_msg(&val, g_fd_dict_objs.PUR_flags, &fd_msg);

	dump_fd_msg(fd_msg);

	/*Post message to hss*/
	CHECK_FCT_DO(fd_msg_send(&fd_msg, NULL, NULL), return S6A_FD_ERROR);

	return SUCCESS;
}

static void
send_rpc_purge(struct s6a_Q_msg *pur_msg, char imsi[])
{
	struct hss_req_msg msg;

	msg.hdr = HSS_PURGE_MSG;
	msg.ue_idx = pur_msg->ue_idx;

	strncpy(msg.data.pur.imsi, imsi, IMSI_STR_LEN);

	if (write(g_our_hss_fd, &msg, HSS_REQ_MSG_SIZE) < 0) {
		log_msg(LOG_ERROR, "HSS PUR msg send failed.\n");
		perror("writing on stream socket");
	}
	log_msg(LOG_INFO, "PUR msg send to hss\n");
}

/**
* Stage specific message processing.
*/
static int
detach_processing(struct s6a_Q_msg * purge_msg)
{
	/*Parse and validate  the buffer*/
	log_msg(LOG_INFO, "IMSI recvd - %s\n",purge_msg-> imsi);
	if (HSS_FD == g_s6a_cfg.hss_type)
		send_purge(purge_msg, (char *)purge_msg-> imsi);
	else {
		log_msg(LOG_INFO, "Sending over IPC \n");
		send_rpc_purge(purge_msg, (char *)purge_msg-> imsi);
	}

	return SUCCESS;
}

/**
* Thread function for stage.
*/
void*
detach_handler(void *data)
{

	log_msg(LOG_INFO, "Detach Q handler ready.\n");

	detach_processing((struct s6a_Q_msg *) data);

	return NULL;
}


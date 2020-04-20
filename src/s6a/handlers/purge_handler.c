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
#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>
#include <freeDiameter/libfdproto.h>

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s6a_fd.h"
#include "s6a.h"
#include "msgType.h"

#include "hss_message.h"

/** Global and externs**/
extern struct fd_dict_objects g_fd_dict_objs;
extern struct fd_dict_data g_fd_dict_data;
extern int g_Q_mme_S6a_fd;
/**global and externs end**/

static
void send_to_stage2(struct s6_incoming_msg_data_t *incoming_msg_p)
{
	TRACE_ENTRY("\n****************WRITE TO g_Q_mme_S6a_fd");

	incoming_msg_p->destInstAddr = htonl(mmeAppInstanceNum_c);
	incoming_msg_p->srcInstAddr = htonl(s6AppInstanceNum_c);

	/*Send to stage2 queue*/
	send_tipc_message(g_Q_mme_S6a_fd, mmeAppInstanceNum_c, (char*)incoming_msg_p, S6_READ_MSG_BUF_SIZE);
}

/**
 * @brief callback handler for purge answer recvd from hss
 * Parse purge answer, state and do cleanup for freediameter
 * @params callback std
 * @return error/success
 */
int
purge_resp_callback(struct msg **buf, struct avp *avps, struct session *sess,
			void *data, enum disp_action *action)
{
	struct msg *resp = NULL;
	struct avp *avp_ptr = NULL;
	struct s6_incoming_msg_data_t s6_incoming_msgs;
	struct avp_hdr *avp_header = NULL;
	unsigned int sess_id_len;
	unsigned char *sess_id= NULL;

	resp = *buf;

	dump_fd_msg(resp);

	/*read session id and extract ue index*/
	CHECK_FCT_DO(fd_sess_getsid(sess, &sess_id, (size_t*)&sess_id_len),
			return S6A_FD_ERROR);
	log_msg(LOG_INFO, "\nPurge callback ----- >session id=%s \n",sess_id);
    
	s6_incoming_msgs.msg_type = purge_answser;
	s6_incoming_msgs.ue_idx = get_ue_idx_from_fd_resp(sess_id, sess_id_len);

	/*AVP: Result-Code*/
	avp_ptr = NULL;
	fd_msg_search_avp(resp, g_fd_dict_objs.res_code, &avp_ptr);

	if(NULL != avp_ptr) {
		fd_msg_avp_hdr(avp_ptr, &avp_header);
		s6_incoming_msgs.msg_data.purge_resp_Q_msg_m.status = avp_header->avp_value->u32;

		if (SUCCESS != s6_incoming_msgs.msg_data.purge_resp_Q_msg_m.status) {
			s6_incoming_msgs.msg_data.purge_resp_Q_msg_m.status = S6A_FD_ERROR;
		}
	} else {
		struct fd_result res;
		avp_ptr = NULL;

		fd_msg_search_avp(resp, g_fd_dict_objs.exp_res,
			&avp_ptr);

		if (NULL != avp_ptr) {
			s6_incoming_msgs.msg_data.purge_resp_Q_msg_m.status = S6A_FD_ERROR;
		}

		if (parse_fd_result(avp_ptr, &res) != SUCCESS) {
			s6_incoming_msgs.msg_data.purge_resp_Q_msg_m.status = S6A_FD_ERROR;
		}
		s6_incoming_msgs.msg_data.purge_resp_Q_msg_m.status =res.result_code;
	}

	
	/*Inform response to mme-app*/
	send_to_stage2(&s6_incoming_msgs);

	/*Do cleanup for freediameter*/
	fd_msg_free(*buf);

	*buf = NULL;

	return SUCCESS;
}

/*Handler for AIA coming from built in perf HS*/
void
handle_perf_hss_purge_resp(int ue_idx)
{
	struct s6_incoming_msg_data_t resp;
    
	resp.msg_type = purge_answser;
	resp.ue_idx = ue_idx;
	resp.msg_data.purge_resp_Q_msg_m.status = 0;

	send_to_stage2(&resp);
}

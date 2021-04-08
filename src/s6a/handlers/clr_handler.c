/*
 * Copyright (c) 2019, Infosys Ltd.
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
//#include "detach_stage2_info.h"
#include "hss_message.h"

#define DIAMETER_SUCCESS 2001

/** Global and externs**/
extern struct fd_dict_objects g_fd_dict_objs;
extern struct fd_dict_data g_fd_dict_data;
extern int g_Q_mme_S6a_fd;
/**global and externs end**/



/**
 * @brief callback handler for clr recvd from hss
 * Parse clr, state and do cleanup for freediameter
 * @params callback std
 * @return error/success
 */
int
clr_resp_callback(struct msg **buf, struct avp *avps, struct session *sess,
			void *data, enum disp_action *action)
{
	struct msg *resp = NULL;
	struct avp *avp_ptr = NULL;
	clr_Q_msg_t clr_msg = {0};
	struct avp_hdr *avp_header = NULL;
	unsigned int sess_id_len;
	unsigned char *sess_id= NULL;

	resp = *buf;

	dump_fd_msg(resp);

	/*read session id and extract ue index*/
	CHECK_FCT_DO(fd_sess_getsid(sess, &sess_id, (size_t*)&sess_id_len),
			return S6A_FD_ERROR);
	log_msg(LOG_INFO, "CLR callback ----- >session id=%s ",sess_id);
    

	/*AVP: Cancellation-Type*/
	avp_ptr = NULL;
	fd_msg_search_avp(resp, g_fd_dict_objs.cancellation_type, &avp_ptr);
	if(NULL != avp_ptr) {
		fd_msg_avp_hdr(avp_ptr, &avp_header);
		clr_msg.c_type = avp_header->avp_value->i32;
	}
	
	fd_msg_search_avp(resp,g_fd_dict_objs.org_host, &avp_ptr);
	if(NULL != avp_ptr) {
		fd_msg_avp_hdr(avp_ptr, &avp_header);
		memcpy(clr_msg.origin_host,avp_header->avp_value->os.data,sizeof(clr_msg.origin_host));
    }

	fd_msg_search_avp(resp, g_fd_dict_objs.org_realm, &avp_ptr);
	if(NULL != avp_ptr) {
		fd_msg_avp_hdr(avp_ptr, &avp_header);
		memcpy(clr_msg.origin_realm,avp_header->avp_value->os.data,sizeof(clr_msg.origin_realm));
    }
         
	fd_msg_search_avp(resp, g_fd_dict_objs.user_name,&avp_ptr);
	if(NULL != avp_ptr) {
		fd_msg_avp_hdr(avp_ptr, &avp_header);
		memcpy(clr_msg.imsi,avp_header->avp_value->os.data,sizeof(clr_msg.imsi));
    }
       
    /*CLA Processing*/

    struct msg *ans;

    if (buf == NULL)
        return EINVAL;

    /* Create answer header */
    CHECK_FCT( fd_msg_new_answer_from_req ( fd_g_config->cnf_dict, buf, 0 ) );
    ans = *buf;

    /* Set the Origin-Host, Origin-Realm, Result-Code AVPs */
    CHECK_FCT( fd_msg_rescode_set( ans, "DIAMETER_SUCCESS", NULL, NULL, 1 ) );

    /* Send the answer */
    CHECK_FCT( fd_msg_send( buf, NULL, NULL ) );

    /*Do cleanup for freediameter*/
    fd_msg_free(*buf);

	*buf = NULL;
	
	clr_msg.header.msg_type = cancel_location_request;

	clr_msg.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	clr_msg.header.srcInstAddr = htonl(s6AppInstanceNum_c);

	/*Send to stage2 queue*/
    send_tipc_message(g_Q_mme_S6a_fd, mmeAppInstanceNum_c, (char*)&clr_msg, sizeof(clr_Q_msg_t));
	
	return SUCCESS;
}

/*Handler for CLR coming from built in perf HS*/
void
handle_perf_hss_clr(int ue_idx, struct hss_clr_msg *clr)
{
	clr_Q_msg_t clr_msg = {0};
    
	clr_msg.header.msg_type = cancel_location_request;
	clr_msg.header.ue_idx = ue_idx;
	memcpy(&(clr_msg.c_type), &(clr->cancellation_type), sizeof(clr->cancellation_type));
	/*Send to stage2 queue*/
	write_ipc_channel(g_Q_mme_S6a_fd, (char*)&clr_msg, sizeof(clr_Q_msg_t));
}

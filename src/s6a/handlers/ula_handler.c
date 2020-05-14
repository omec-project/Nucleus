/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdproto.h>
#include <freeDiameter/libfdcore.h>

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s6a_fd.h"
#include "s6a.h"
#include "msgType.h"

#include "hss_message.h"

extern int g_Q_mme_S6a_fd;
extern struct fd_dict_objects g_fd_dict_objs;
extern struct fd_dict_data g_fd_dict_data;

/**
 * @brief Parse subscription information for UE
 * @param[in] avp_ptr - POinter to subscription data avp
 * @param[out] ula information filled with parsed data
 * @return void
 */
static void
parse_ula_subscription_data(struct avp *avp_ptr, struct ula_Q_msg *ula)
{
	struct avp *next = NULL;
	struct avp_hdr *element = NULL;

	CHECK_FCT_DO(fd_msg_avp_hdr(avp_ptr, &element),
                        return);

	if ((NULL == element) ||
           (element->avp_code != g_fd_dict_data.subscription_data.avp_code))
        	return;

	CHECK_FCT_DO(fd_msg_browse(avp_ptr, MSG_BRW_FIRST_CHILD, &next, NULL),
			return);

	for(;
		NULL != next;
		fd_msg_browse(next, MSG_BRW_NEXT, &next, NULL)) {

		fd_msg_avp_hdr (next, &element);

		if(NULL == element) return;

		/*AVP: Access-Restriction-Data(1426)*/
		if(g_fd_dict_data.access_restriction_data.avp_code ==
				element->avp_code) {
			ula->access_restriction_data = element->avp_value->u32;
			continue;
		}

		/*AVP: Subscriber-Status(1424)*/
		if(g_fd_dict_data.subscriber_status.avp_code == element->avp_code) {
			ula->subscription_status = element->avp_value->i32;
			continue;
		}

		/*AVP: Network-Access-Mode(1417)*/
		if(g_fd_dict_data.net_access_mode.avp_code == element->avp_code) {
			ula->net_access_mode = element->avp_value->i32;
			continue;
		}

		/*AVP: Regional-Subscription-Zone-Code(1446)*/
		if(g_fd_dict_data.reg_subs_zone_code.avp_code ==
				element->avp_code) {
			//element->avp_value : 10 string values of len 4
			continue;
		}

		/*AVP: MSISDN(701)*/
		if(g_fd_dict_data.MSISDN.avp_code == element->avp_code) {
			memcpy(ula->MSISDN, element->avp_value->os.data, element->avp_value->os.len);
			continue;
		}

		/*AVP: AMBR(1435)*/
			/*AVP: Max-Requested-Bandwidth-UL(516)
			  AVP: Max-Requested-Bandwidth-DL(515)
			  AVP: Extended-Max-Requested-BW-UL(555)
                          AVP: Extended-Max-Requested-BW-DL(554)*/

		if(g_fd_dict_data.AMBR.avp_code == element->avp_code) {
			/*AMBR has its own child elements, iterate through those*/
			struct avp *ambr_itr = NULL;
			struct avp_hdr *ambr_element = NULL;

			CHECK_FCT_DO(fd_msg_browse(next, MSG_BRW_FIRST_CHILD,
						&ambr_itr, NULL), return);

			/*Iterate through subscription data child avps*/
			while(NULL != ambr_itr) {
				fd_msg_avp_hdr(ambr_itr, &ambr_element);

				if(g_fd_dict_data.max_req_bandwidth_UL.avp_code ==
						ambr_element->avp_code) {
					ula->max_requested_bw_ul = ambr_element->avp_value->u32;
				}

				if(g_fd_dict_data.max_req_bandwidth_DL.avp_code ==
						ambr_element->avp_code) {
					ula->max_requested_bw_dl = ambr_element->avp_value->u32;
				}

				if(EXT_MAX_REQ_BW_UL_AVP_CODE == ambr_element->avp_code) {
                                        ula->extended_max_requested_bw_ul = ambr_element->avp_value->u32;
                                }

				if(EXT_MAX_REQ_BW_DL_AVP_CODE == ambr_element->avp_code) {
                                        ula->extended_max_requested_bw_dl = ambr_element->avp_value->u32;
                                }

				CHECK_FCT_DO(fd_msg_browse(ambr_itr, MSG_BRW_NEXT,
						&ambr_itr, NULL), return);
			}
			continue;
		}

		/*AVP: APN-Configuration-Profile(1429)*/
			/*AVP: Context-Identifier(1423)
			AVP: All-APN-Configurations-Included-Indicator(1428)
			AVP: APN-Configuration(1430)*/
		if(g_fd_dict_data.APN_config_profile.avp_code == element->avp_code) {
			/*APN profile has its own child elements, iterate through
			 * those*/
			struct avp *apn_cfg_prof_itr = NULL;
			struct avp_hdr *apn_cfg_prof_element = NULL;

			CHECK_FCT_DO(fd_msg_browse(next, MSG_BRW_FIRST_CHILD,
						&apn_cfg_prof_itr, NULL), return);


			/*Iterate through subscription data child avps*/
			while(NULL != apn_cfg_prof_itr) {
				fd_msg_avp_hdr(apn_cfg_prof_itr, &apn_cfg_prof_element);

				if(g_fd_dict_data.ctx_id.avp_code ==
						apn_cfg_prof_element->avp_code) {
					ula->apn_config_profile_ctx_id =
						apn_cfg_prof_element->avp_value->u32;
				} else
				if(g_fd_dict_data.all_APN_configs_included_ind.avp_code ==
						apn_cfg_prof_element->avp_code) {
					ula->all_APN_cfg_included_ind =
						apn_cfg_prof_element->avp_value->i32;
				} else
				if(g_fd_dict_data.APN_config.avp_code ==
						apn_cfg_prof_element->avp_code){

					//APN configuration list : There is list of elements to read
					struct avp *apn_cfg_itr = NULL;
					struct avp_hdr *apn_cfg_element = NULL;

					CHECK_FCT_DO(fd_msg_browse(apn_cfg_prof_itr,
							MSG_BRW_FIRST_CHILD, &apn_cfg_itr, NULL), return);

					while(NULL != apn_cfg_itr){

						fd_msg_avp_hdr(apn_cfg_itr, &apn_cfg_element);

						// TODO g_fd_dict_data does not have service_slection
						// will finish this part in the following patch
						// service_slection code is 493
						// if(g_fd_dict_data.service_slection ==
						if (493 == apn_cfg_element->avp_code){

							log_msg(LOG_INFO, "APN name recvd from hss - %s\n",
									apn_cfg_element->avp_value->os.data);
							log_msg(LOG_INFO, "APN length recvd from hss - %lu\n",
									apn_cfg_element->avp_value->os.len);

							ula->selected_apn.val[0] = apn_cfg_element->avp_value->os.len;
							memcpy(&(ula->selected_apn.val[1]),
									apn_cfg_element->avp_value->os.data,
									apn_cfg_element->avp_value->os.len);
							ula->selected_apn.len =
									apn_cfg_element->avp_value->os.len+1;
						}
						if (848 == apn_cfg_element->avp_code && ula->static_addr == 0){
							struct sockaddr_in  temp;
							int result = fd_dictfct_Address_interpret(apn_cfg_element->avp_value, &temp);
							log_msg(LOG_INFO, "Served IP address found %d %s \n", result, inet_ntoa(temp.sin_addr)); 
							ula->static_addr = temp.sin_addr.s_addr; // network order
						} 
						apn_cfg_prof_itr = apn_cfg_itr;

						CHECK_FCT_DO(fd_msg_browse(apn_cfg_itr, MSG_BRW_NEXT,
								&apn_cfg_itr, NULL), return);

					}
					continue;

				}

				CHECK_FCT_DO(fd_msg_browse(apn_cfg_prof_itr, MSG_BRW_NEXT,
						&apn_cfg_prof_itr, NULL), return);
			}
			continue;
		}

		/*AVP: Subscribed-Periodic-RAU-TAU-Timer(1619)*/
		if(g_fd_dict_data.subsc_periodic_RAU_TAU_tmr.avp_code
				== element->avp_code) {
			ula->RAU_TAU_timer = element->avp_value->u32;
			continue;
		}

	}
}

/**
 * @brief Call back registered to handle ULA from hss
 * @param callback requiremd arguments
 * @return int error code as success or failure
 */
int
ula_resp_callback(struct msg **buf, struct avp *avp_ptr, struct session *sess,
			void *data, enum disp_action *action)
{
	int sess_id_len, ue_idx;
	unsigned char *sess_id= NULL;
	struct s6_incoming_msg_data_t s6_incoming_msgs = {0};
	struct avp *avp = NULL;

	CHECK_FCT_DO(fd_sess_getsid(sess, &sess_id, (size_t*)&sess_id_len),
		return S6A_FD_ERROR);

	log_msg(LOG_INFO, "\nCallback ----- >session id=%s \n", sess_id);

	s6_incoming_msgs.msg_data.ula_Q_msg_m.res = SUCCESS;
	ue_idx = get_ue_idx_from_fd_resp(sess_id, sess_id_len);

	CHECK_FCT_DO(fd_msg_browse(*buf, MSG_BRW_FIRST_CHILD, &avp, NULL), return S6A_FD_ERROR);
        while (avp) {
                struct avp_hdr *hdr = NULL;
                fd_msg_avp_hdr (avp, &hdr);

                switch(hdr->avp_code)
                {
                    case SUB_DATA_AVP_CODE:
                    {
                        parse_ula_subscription_data(avp, &s6_incoming_msgs.msg_data.ula_Q_msg_m);
                    } break;
                    case SUPP_FEAT_AVP_CODE:
                    {
			supported_features_list *supp_features_list = &s6_incoming_msgs.msg_data.ula_Q_msg_m.supp_features_list;
                        if(supp_features_list->count < 2) {
                            int ret = parse_supported_features_avp(avp, &supp_features_list->supp_features[supp_features_list->count]);
                            if(ret == SUCCESS)
                                supp_features_list->count ++;
                        }
                    } break;
                    default:
                        goto next;
                }
                next:
                    /* Go to next AVP */
                    CHECK_FCT_DO(fd_msg_browse(avp, MSG_BRW_NEXT, &avp, NULL), return S6A_FD_ERROR);
	}
	fd_msg_free(*buf);
	*buf = NULL;
    
	s6_incoming_msgs.msg_type = update_loc_answer;
	s6_incoming_msgs.ue_idx = ue_idx;

	s6_incoming_msgs.destInstAddr = htonl(mmeAppInstanceNum_c);
	s6_incoming_msgs.srcInstAddr = htonl(s6AppInstanceNum_c);

	/*Send to stage2 queue*/
	send_tipc_message(g_Q_mme_S6a_fd, mmeAppInstanceNum_c, (char*)&s6_incoming_msgs, S6_READ_MSG_BUF_SIZE);

	return SUCCESS;
}

/*Handler for ULA coming from built in perf HS*/
void
handle_perf_hss_ula(int ue_idx, struct hss_ula_msg *ula)
{
	struct s6_incoming_msg_data_t msg;
    
	msg.msg_type = update_loc_answer;
	msg.ue_idx = ue_idx;
	msg.msg_data.ula_Q_msg_m.res = ula->subscription_state;
	/*Send to stage2 queue*/
	write_ipc_channel(g_Q_mme_S6a_fd, (char*)&msg, S6_READ_MSG_BUF_SIZE);
}

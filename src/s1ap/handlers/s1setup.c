/*
 * Copyright 2019-present Open Networking Foundation
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

#include "s1apContextManager/s1apContextWrapper_c.h"
#include "s1_common_types.h"
#include "err_codes.h"
#include "options.h"
#include "ipc_api.h"
#include "main.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "sctp_conn.h"
#include "s1ap_structs.h"
#include "s1ap_msg_codes.h"
#include "ProtocolIE-ID.h"
#include "ProtocolIE-Field.h"
#include "SupportedTAs-Item.h"
#include "TAC.h"

extern ipc_handle ipc_S1ap_Hndl;
/******************************************************
  S1_SETUP_RESPONSE handling
******************************************************/
int
s1_setup_response(int enb_fd, struct PLMN *plmn)
{
    log_msg(LOG_DEBUG,"Process S1 Setup response.");
	uint32_t length = 0;
    uint8_t *buffer = NULL;
	s1ap_config_t *s1ap_cfg = get_s1ap_config();
    struct s1ap_common_req_Q_msg rsp_msg = {0};
    rsp_msg.IE_type = S1AP_SETUP_RESPONSE;

    memcpy(rsp_msg.mme_name, s1ap_cfg->mme_name, strlen(s1ap_cfg->mme_name));
    rsp_msg.mme_code = s1ap_cfg->mme_code;
	rsp_msg.mme_group_id = (s1ap_cfg->mme_group_id);
    
    struct PLMN local_plmn_id = {0};

	local_plmn_id.idx[0] = plmn->idx[0];
	local_plmn_id.idx[1] = plmn->idx[1];
	local_plmn_id.idx[2] = plmn->idx[2];
	log_msg(LOG_DEBUG,"Number of mnc digits %d ", plmn->mnc_digits);
    memcpy(&rsp_msg.mme_plmn_id, &local_plmn_id, 3);
    rsp_msg.rel_cap = s1ap_cfg->rel_cap;

    int ret = s1ap_mme_encode_outcome(&rsp_msg, &buffer, &length);
    if(ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding S1 setup response failed.");
        return E_FAIL;
    }


    send_sctp_msg_with_fd(enb_fd, buffer, length, 0);
   	log_msg(LOG_INFO, "buffer size is %d", length);
    if(buffer)
    {
        free(buffer);
    }

	return SUCCESS;
}

/******************************************************
  S1_SETUP_FAILURE handling
******************************************************/
int
s1_setup_failure(struct s1ap_common_req_Q_msg* s1ap_setup_failure)
{
    log_msg(LOG_DEBUG,"Process S1 Setup failure.");
	uint32_t length = 0;
    uint8_t *buffer = NULL;
    int enb_fd = s1ap_setup_failure->enb_fd;

    int ret = s1ap_mme_encode_outcome(s1ap_setup_failure, &buffer, &length);
    if(ret == -1)
    {
        log_msg(LOG_ERROR, "Encoding S1 setup failure failed.");
        return E_FAIL;
    }

    send_sctp_msg_with_fd(enb_fd, buffer, length, 0);
  	log_msg(LOG_ERROR, "S1ap setup failure ");
    if(buffer)
    {
        free(buffer);
    }

	return SUCCESS;
}

int
s1_setup_handler(InitiatingMessage_t *msg, int enb_fd)
{
	//unsigned char *resp_msg = NULL;
	//int resp_len = 0;
	s1ap_config_t *s1ap_cfg = get_s1ap_config();
    struct EnbStruct enbStruct = {0};
    enbStruct.enbFd_m = enb_fd;
    bool match_found = false;
    uint32_t cbIndex = 0;
    struct PLMN matched_plmn = {0};

	/*Validate all eNB info*/
	if(msg->value.present == InitiatingMessage__value_PR_S1SetupRequest)
	{
		ProtocolIE_Container_129P40_t *protocolIes = &msg->value.choice.S1SetupRequest.protocolIEs;
		for(int i=0; i < protocolIes->list.count; i++ )
		{
			S1SetupRequestIEs_t *ie_p;
			ie_p = protocolIes->list.array[i];
			switch(ie_p->id)
			{
				case ProtocolIE_ID_id_Global_ENB_ID:
				{
					Global_ENB_ID_t *geNb = &ie_p->value.choice.Global_ENB_ID;
					if(geNb->eNB_ID.present == ENB_ID_PR_macroENB_ID) 
					{
						log_msg(LOG_DEBUG, "macro eNB id size %lu", geNb->eNB_ID.choice.macroENB_ID.size);
                        uint8_t *enb_id_buf = geNb->eNB_ID.choice.macroENB_ID.buf;
                        uint32_t val = enb_id_buf[0];
                        enbStruct.enbId_m |= val << 12;
                        val = enb_id_buf[1];
                        enbStruct.enbId_m |= val << 4;
                        val = enb_id_buf[2] & 0xf0;
                        enbStruct.enbId_m |= val >> 4;
					}
                    else if (geNb->eNB_ID.present == ENB_ID_PR_homeENB_ID)
                    {
						log_msg(LOG_DEBUG, "home eNB id size %lu", 
                                  geNb->eNB_ID.choice.homeENB_ID.size);
                        uint8_t *enb_id_buf = geNb->eNB_ID.choice.homeENB_ID.buf;
                        uint32_t val = enb_id_buf[0];
                        enbStruct.enbId_m |= val << 20;
                        val = enb_id_buf[1];
                        enbStruct.enbId_m |= val << 12;
                        val = enb_id_buf[2];
                        enbStruct.enbId_m |= val << 4;
                        val = enb_id_buf[3] & 0xf0;
                        enbStruct.enbId_m |= val >> 4;
                    }
 					break;
				}
				case ProtocolIE_ID_id_eNBname:
				{
					ENBname_t *eNbName = &ie_p->value.choice.ENBname;
					log_msg(LOG_DEBUG, "S1 Setup Message with eNB name %s ", eNbName->buf);
                    strcpy(enbStruct.eNbName, (const char *)eNbName->buf);
					break;
				}
				
				case ProtocolIE_ID_id_SupportedTAs:
				{
					SupportedTAs_t *val = &ie_p->value.choice.SupportedTAs;
					for(int supTAs=0; supTAs<val->list.count; supTAs++)
					{
						SupportedTAs_Item_t *tac = val->list.array[supTAs];
						uint16_t tac_i=0;
						for(int t=0; t< tac->tAC.size; t++) 
						{
							tac_i = (tac_i<<8) | tac->tAC.buf[t]; 
						}

						log_msg(LOG_INFO, "S1setup Supported Tac %d %d size %lu ..Final tac %d ", tac->tAC.buf[0], tac->tAC.buf[1] , tac->tAC.size, tac_i);
						BPLMNs_t *plmns = &tac->broadcastPLMNs; 
						log_msg(LOG_INFO, "S1setup Supported PLMNS %d ", plmns->list.count);
						for(int p=0; p<plmns->list.count; p++)
						{
							PLMNidentity_t *plmn = plmns->list.array[p]; 
							char plmn_s[10] = {'\0'};
							memcpy(plmn_s, plmn->buf, plmn->size);
							log_msg(LOG_INFO, "S1setup Supported PLMN %s Plmn buffer size %lu ", plmn_s, plmn->size);
							struct PLMN plmn_struct = {0}; 
							for(int b=0; b< plmn->size; b++) 
							{
								plmn_struct.idx[b]  = plmn->buf[b]; 
							}
							int config_plmn;
							for(config_plmn = 0; config_plmn < s1ap_cfg->num_plmns; config_plmn++)
							{
								if((s1ap_cfg->plmns[config_plmn].idx[0] == plmn_struct.idx[0]) &&
								  (s1ap_cfg->plmns[config_plmn].idx[1] == plmn_struct.idx[1]) &&
								  (s1ap_cfg->plmns[config_plmn].idx[2] == plmn_struct.idx[2])) 
								{
									log_msg(LOG_INFO, "PLMN match found  Configured %x %x %x ", s1ap_cfg->plmns[config_plmn].idx[0], s1ap_cfg->plmns[config_plmn].idx[1], s1ap_cfg->plmns[config_plmn].idx[2]);
									log_msg(LOG_INFO, "PLMN match found  Received %x %x %x ", plmn_struct.idx[0], plmn_struct.idx[1], plmn_struct.idx[2]);
                                    memcpy(&enbStruct.tai_m.plmn_id,
                                           &plmn_struct, 
                                           3); //sizeof(struct PLMN)); plmn struct has some more fields
                                    enbStruct.tai_m.tac = tac_i;
                                    match_found = true;
                                    matched_plmn = s1ap_cfg->plmns[config_plmn];
									break;
								}
								else 
								{
									log_msg(LOG_INFO, "PLMN match not found  - Configured - %x %x %x ", s1ap_cfg->plmns[config_plmn].idx[0], s1ap_cfg->plmns[config_plmn].idx[1], s1ap_cfg->plmns[config_plmn].idx[2]);
									log_msg(LOG_INFO, "PLMN match not found Received - %x %x %x ", plmn_struct.idx[0], plmn_struct.idx[1], plmn_struct.idx[2]);
								}
							}
							if(config_plmn >= s1ap_cfg->num_plmns)
							{
								//TODO : reject connection no match found 
							}
						}
					}
					break;
				}
				case ProtocolIE_ID_id_DefaultPagingDRX:
				{
					//PagingDRX_t *val = &ie_p->value.choice.PagingDRX;
					break;
				}
				default:
					break;
			}
		}
	} 

    if(match_found)
    {
        log_msg(LOG_DEBUG, "PLMN Match found. Create CB and add Enb Info.");
        cbIndex = findControlBlockWithEnbId(enbStruct.enbId_m);
        if(INVALID_CB_INDEX == cbIndex)
        {
            log_msg(LOG_DEBUG, "No ENb ctx found for enb id %d.", enbStruct.enbId_m);
            cbIndex = createControlBlock();
            if(INVALID_CB_INDEX == cbIndex)
            {
                log_msg(LOG_ERROR,"CB creation failed.");
                return E_FAIL;
            }
            log_msg(LOG_DEBUG, "New ctx block %d allocated for found for enb id %d.", cbIndex, enbStruct.enbId_m);
            setValuesForEnbCtx(cbIndex, &enbStruct, false);
        }
        else
        {
            log_msg(LOG_DEBUG, "ENB Ctx found for enb id %d. Update values.",enbStruct.enbId_m);
            enbStruct.restart_counter++;
            cbIndex = setValuesForEnbCtx(cbIndex, &enbStruct, true);
            if(INVALID_CB_INDEX == cbIndex)
            {
                log_msg(LOG_ERROR,"Set values in Enb Ctx failed.");
                struct s1ap_common_req_Q_msg s1ap_setup_failure = {0};
                s1ap_setup_failure.IE_type = S1AP_SETUP_FAILURE;
                s1ap_setup_failure.enb_fd = enb_fd;
                s1ap_setup_failure.cause.present = s1apCause_PR_misc;
                s1ap_setup_failure.cause.choice.misc = s1apCauseMisc_unknown_PLMN;
                return s1_setup_failure(&s1ap_setup_failure);
            }
        }
    }
    else
    {
        log_msg(LOG_ERROR, "No PLMN Match found for enb id %d.",enbStruct.enbId_m);
        /* Send S1Setup Failure.*/
   	    struct s1ap_common_req_Q_msg s1ap_setup_failure = {0};
        s1ap_setup_failure.IE_type = S1AP_SETUP_FAILURE;
        s1ap_setup_failure.enb_fd = enb_fd;
        s1ap_setup_failure.cause.present = s1apCause_PR_misc;
        s1ap_setup_failure.cause.choice.misc = s1apCauseMisc_unknown_PLMN;
        return s1_setup_failure(&s1ap_setup_failure);
    }

	/*Create S1Setup response*/
    s1_setup_response(enb_fd, &matched_plmn);
	s1apEnbStatus_Msg_t s1Msg = {0};
	s1Msg.header.msg_type = enb_status_msg; 
	s1Msg.header.srcInstAddr = htonl(s1apAppInstanceNum_c);
	s1Msg.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    uint32_t temp_cbIndex = findControlBlockWithEnbFd(enb_fd);
    s1Msg.ver = 1;
    s1Msg.status = 1;
    s1Msg.context_id = temp_cbIndex;
    s1Msg.enbId_m = enbStruct.enbId_m;
    s1Msg.restart_counter = enbStruct.restart_counter;
    strncpy(s1Msg.eNbName, enbStruct.eNbName, 128);
    log_msg(LOG_INFO, "Send eNB setup message to mme-app %s", s1Msg.eNbName);
	send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&s1Msg, sizeof(s1apEnbStatus_Msg_t));

	return SUCCESS;
}

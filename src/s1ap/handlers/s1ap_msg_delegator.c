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
#include "options.h"
#include "ipc_api.h"
#include "main.h"
#include "s1ap.h"
#include "s1ap_config.h"
#include "sctp_conn.h"
#include "s1ap_structs.h"
#include "s1ap_msg_codes.h"
#include "s1ap_ie.h"
#include "ProtocolIE-ID.h"
#include "ProtocolIE-Field.h"

extern ipc_handle ipc_S1ap_Hndl;

int convertToInitUeProtoIe(InitiatingMessage_t *msg, struct proto_IE* proto_ies, initial_ue_msg_t *s1Msg)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;

	s1Msg->criticality = msg->criticality; 

    if(msg->value.present == InitiatingMessage__value_PR_InitialUEMessage)
    {
        ProtocolIE_Container_129P32_t* protocolIes = &msg->value.choice.InitialUEMessage.protocolIEs;
        proto_ies->no_of_IEs = protocolIes->list.count;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), proto_ies->no_of_IEs);
        if(proto_ies->data == NULL) {
            log_msg(LOG_ERROR,"Calloc failed for protocol IE.");
            return -1;
        }
		for (int i = 0; i < protocolIes->list.count; i++) {
			InitialUEMessage_IEs_t *ie_p;
			ie_p = protocolIes->list.array[i];
			switch(ie_p->id) {
				case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
					{
                        ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                        if(InitialUEMessage_IEs__value_PR_ENB_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                        }
                        else 
						{
							log_msg (LOG_ERROR, "Decoding of IE eNB_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
						memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id, s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                        log_msg(LOG_DEBUG, "ENB UE S1ap ID decode Success = %lu ",proto_ies->data[i].val.enb_ue_s1ap_id);
						s1Msg->header.s1ap_enb_ue_id = proto_ies->data[i].val.enb_ue_s1ap_id;
					} break;
				case ProtocolIE_ID_id_NAS_PDU:
					{
                        NAS_PDU_t *s1apNASPDU_p = NULL;
                        if(InitialUEMessage_IEs__value_PR_NAS_PDU == ie_p->value.present)
                        {
						    s1apNASPDU_p = &ie_p->value.choice.NAS_PDU;
						}
						else 
						{
							log_msg (LOG_ERROR, "Decoding of IE NAS PDU failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_NAS_PDU;
						memcpy(s1Msg->nasMsg.nasMsgBuf, (char*)s1apNASPDU_p->buf, s1apNASPDU_p->size);
						s1Msg->nasMsg.nasMsgSize = s1apNASPDU_p->size;
					} break;
				case ProtocolIE_ID_id_TAI:
					{
                        TAI_t *s1apTAI_p = NULL;
                        if(InitialUEMessage_IEs__value_PR_TAI == ie_p->value.present)
                        {
						    s1apTAI_p = &ie_p->value.choice.TAI;
                        }
                        else 
						{
							log_msg (LOG_ERROR, "Decoding of IE TAI failed");
							return -1;
						}

                        log_msg(LOG_DEBUG, "TAI decode Success");
                        proto_ies->data[i].IE_type = S1AP_IE_TAI;
						memcpy(&proto_ies->data[i].val.tai.tac, s1apTAI_p->tAC.buf, s1apTAI_p->tAC.size);
						memcpy(proto_ies->data[i].val.tai.plmn_id.idx,
                                s1apTAI_p->pLMNidentity.buf, s1apTAI_p->pLMNidentity.size);
						s1apTAI_p = NULL;
                    	memcpy(&(s1Msg->tai), &(proto_ies->data[i].val.tai), sizeof(struct TAI));
					} break;
				case ProtocolIE_ID_id_EUTRAN_CGI:
					{
			            EUTRAN_CGI_t*	 s1apCGI_p = NULL;;
                        if(InitialUEMessage_IEs__value_PR_EUTRAN_CGI == ie_p->value.present)
                        {
						    s1apCGI_p = &ie_p->value.choice.EUTRAN_CGI;
                        }

                        if (s1apCGI_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE CGI failed");
							return -1;
						}

                        log_msg(LOG_DEBUG, "CGI decode Success");
                        proto_ies->data[i].IE_type = S1AP_IE_UTRAN_CGI;
						memcpy(&proto_ies->data[i].val.utran_cgi.cell_id,
                               s1apCGI_p->cell_ID.buf, s1apCGI_p->cell_ID.size);
						memcpy(proto_ies->data[i].val.utran_cgi.plmn_id.idx,
                                s1apCGI_p->pLMNidentity.buf, s1apCGI_p->pLMNidentity.size);
						s1apCGI_p = NULL;
                        memcpy(&(s1Msg->utran_cgi), &(proto_ies->data[i].val.utran_cgi),
                           sizeof(struct CGI));

					} break;
				case ProtocolIE_ID_id_RRC_Establishment_Cause:
					{
			            RRC_Establishment_Cause_t	 *s1apRRCEstCause_p = NULL;
                        if(InitialUEMessage_IEs__value_PR_RRC_Establishment_Cause == ie_p->value.present)
                        {
						    s1apRRCEstCause_p = &ie_p->value.choice.RRC_Establishment_Cause;
                        }
                        else {
							log_msg (LOG_ERROR, "Decoding of IE RRC Cause failed");
							return -1;
						}

                        log_msg(LOG_DEBUG, "RRC Cause decode Success");
                        proto_ies->data[i].IE_type = S1AP_IE_RRC_EST_CAUSE;
						proto_ies->data[i].val.rrc_est_cause = (enum ie_RRC_est_cause) *s1apRRCEstCause_p;
					} break;
				case ProtocolIE_ID_id_S_TMSI:
					{
			            S_TMSI_t*	 s1apStmsi_p = NULL;;
                        if(InitialUEMessage_IEs__value_PR_S_TMSI == ie_p->value.present)
                        {
						    s1apStmsi_p = &ie_p->value.choice.S_TMSI;
                        }
                        else 
						{
							log_msg (LOG_ERROR, "Decoding of IE STMSI failed");
							return -1;
						}

                        //struct STMSI     s_tmsi
                        proto_ies->data[i].IE_type = S1AP_IE_S_TMSI;
						memcpy(&proto_ies->data[i].val.s_tmsi.mme_code,
                               s1apStmsi_p->mMEC.buf, sizeof(uint8_t));
						memcpy(&proto_ies->data[i].val.s_tmsi.m_TMSI,
                                s1apStmsi_p->m_TMSI.buf, sizeof(uint32_t));
                        memcpy(&s1Msg->s_tmsi.mme_code, s1apStmsi_p->mMEC.buf, sizeof(uint8_t)); 
                        memcpy(&s1Msg->s_tmsi.m_TMSI, s1apStmsi_p->m_TMSI.buf, sizeof(uint32_t));
					} break;
                default:
                    {
                        proto_ies->data[i].IE_type = ie_p->id;
                        log_msg(LOG_WARNING, "Unhandled IE %lu in initial UE message ", ie_p->id);
                    }
			}
		}
     }

    return 0;
}

static int
init_ue_msg_handler(InitiatingMessage_t *msg, int enb_fd)
{
	struct proto_IE proto_ies={0};
	initial_ue_msg_t s1Msg={0};

    /* TODO : Error handling. Bad message will lead crash. 
     * Preferably reject the message, increment stats.
     */
    uint32_t cbIndex = findControlBlockWithEnbFd(enb_fd);
    if(INVALID_CB_INDEX == cbIndex)
    {
        log_msg(LOG_ERROR,"No CB found for enb fd %d.", enb_fd);
        return E_FAIL;
    }

	int decode_result = convertToInitUeProtoIe(msg, &proto_ies, &s1Msg);
    if(decode_result < 0 )
    {
		log_msg(LOG_ERROR, "S1ap message decode failed. Dropping message");
		free(proto_ies.data);
		return E_FAIL;
    }
	s1Msg.enb_fd = cbIndex;
	s1Msg.header.msg_type = S1AP_INITIAL_UE_MSG_CODE; 
	s1Msg.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	s1Msg.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

	log_msg(LOG_INFO, "sending S1AP_INITIAL_UE_MSG msg: context_id = %u tmsi = %u ", cbIndex, s1Msg.s_tmsi.m_TMSI);
	send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&s1Msg, sizeof(s1Msg));

	/*Send S1Setup response*/
	free(proto_ies.data);
	return SUCCESS;
}

static int
UL_NAS_msg_handler(InitiatingMessage_t *msg, int enb_fd)
{
	//TODO: use static instead of synamic for perf.
	struct proto_IE proto_ies={0};

	log_msg(LOG_INFO, "S1AP_UL_NAS_TX_MSG msg ");
    
    uint32_t cbIndex = findControlBlockWithEnbFd(enb_fd);
    if(INVALID_CB_INDEX == cbIndex)
    {
        log_msg(LOG_ERROR,"No CB found for enb fd %d.", enb_fd);
        return E_FAIL;
    }

	uplink_nas_t s1Msg={0};
	s1Msg.enb_fd = cbIndex;
    int decode_result = convertUplinkNasToProtoIe(msg, &proto_ies, &s1Msg);

    if(decode_result < 0 )
    {
	  	log_msg(LOG_ERROR, "S1ap message decode failed. Dropping message");
		free(proto_ies.data);
		return E_FAIL;
    }
	s1Msg.header.msg_type = S1AP_UL_NAS_TX_MSG_CODE;
	s1Msg.header.destInstAddr = htonl(mmeAppInstanceNum_c);
	s1Msg.header.srcInstAddr = htonl(s1apAppInstanceNum_c);

	send_tipc_message(ipc_S1ap_Hndl, mmeAppInstanceNum_c, (char *)&s1Msg, sizeof(s1Msg));


	free(proto_ies.data);
	return SUCCESS;
}

void
handle_s1ap_message(void *msg)
{
    /*convert message from network to host*/

    /*Call handler for the procedure code. TBD: Tasks pool for handlers*/

    int enb_fd = 0;
    int msg_size = 0;
    memcpy(&enb_fd, msg, sizeof(int));

    memcpy(&msg_size, msg + sizeof(int), sizeof(int));

    char *message = ((char *) msg) + 2*sizeof(int);
    S1AP_PDU_t                              pdu = {(S1AP_PDU_PR_NOTHING)};
    S1AP_PDU_t                             *pdu_p = &pdu;
    asn_dec_rval_t                          dec_ret = {(RC_OK)};
    memset ((void *)pdu_p, 0, sizeof (S1AP_PDU_t));
    dec_ret = aper_decode (NULL, &asn_DEF_S1AP_PDU, (void **)&pdu_p, message, msg_size, 0, 0);

    if (dec_ret.code != RC_OK) {
        log_msg(LOG_ERROR, "handle s1ap message ASN Decode PDU Failed");
        free(msg);
        return;
    }

    log_msg(LOG_INFO, "handle s1ap message enb_fd = %d msg size = %d .",enb_fd, msg_size);
    switch (pdu_p->present) {
        case S1AP_PDU_PR_initiatingMessage:
            s1ap_mme_decode_initiating (pdu_p->choice.initiatingMessage, enb_fd);
            break;
        case S1AP_PDU_PR_successfulOutcome:
            s1ap_mme_decode_successfull_outcome (pdu_p->choice.successfulOutcome);
            break;
        case S1AP_PDU_PR_unsuccessfulOutcome:
            s1ap_mme_decode_unsuccessfull_outcome (pdu_p->choice.unsuccessfulOutcome);
            break;
        default:
            log_msg(LOG_WARNING, "Unknown message outcome (%d) or not implemented", (int)pdu_p->present);
            break;
    }
    free(msg);
    return;
}

int
s1ap_mme_decode_successfull_outcome (SuccessfulOutcome_t* msg)
{
  log_msg(LOG_DEBUG,"successful outcome decode :proc code %lu", msg->procedureCode);
  switch (msg->procedureCode) {

	case S1AP_INITIAL_CTX_RESP_CODE:
		s1_init_ctx_resp_handler(msg);
		break;
	
	case S1AP_UE_CONTEXT_RELEASE_CODE:
		s1_ctx_release_complete_handler(msg);
		break;
		
	case S1AP_HANDOVER_RESOURCE_ALLOCATION_CODE:
		s1_handover_ack_handler(msg);
		break;

	case S1AP_ERAB_SETUP_CODE:
		erab_setup_response_handler(msg);
		break;

    case S1AP_ERAB_RELEASE_CODE:
        erab_release_response_handler(msg);
        break;
		
	default:
		log_msg(LOG_ERROR, "Unknown procedure code - %lu", msg->procedureCode & 0x00FF);
		break;
	}
	
	return 0;
}

int
s1ap_mme_decode_unsuccessfull_outcome (UnsuccessfulOutcome_t *msg)
{
    log_msg(LOG_DEBUG,"unsuccessful outcome decode : proc code %lu", msg->procedureCode);
    switch (msg->procedureCode) {

      case S1AP_HANDOVER_RESOURCE_ALLOCATION_CODE:
    	  s1_handover_faliure_handler(msg);
    	  break;
      default:
    	  log_msg(LOG_ERROR, "Unknown procedure code - %lu",msg->procedureCode & 0x00FF);
    	  break;
    }

    return 0;
}

int
s1ap_mme_decode_initiating (InitiatingMessage_t *initiating_p, int enb_fd) 
{
  log_msg(LOG_INFO, "s1ap_mme_decode_initiating proc code %lu", initiating_p->procedureCode);
  switch (initiating_p->procedureCode) {
	case S1AP_SETUP_REQUEST_CODE:
		s1_setup_handler(initiating_p, enb_fd);
		break;

	case S1AP_INITIAL_UE_MSG_CODE:
		init_ue_msg_handler(initiating_p, enb_fd);
		break;

	case S1AP_UL_NAS_TX_MSG_CODE:
		UL_NAS_msg_handler(initiating_p, enb_fd);
		break;
		
	case S1AP_UE_CONTEXT_RELEASE_REQUEST_CODE:
		s1_ctx_release_request_handler(initiating_p);
		break;
		
	case S1AP_HANDOVER_REQUIRED_CODE:
		s1_handover_required_handler(initiating_p, enb_fd);
		break;

	case S1AP_HANDOVER_NOTIFY_CODE:
		s1_handover_notify_handler(initiating_p);
		break;

	case S1AP_ENB_STATUS_TRANSFER_CODE:
		s1_enb_status_transfer_handler(initiating_p);
		break;

	case S1AP_HANDOVER_CANCEL_CODE:
		s1_handover_cancel_handler(initiating_p);
		break;
        
	case S1AP_ERAB_MODIFICATION_INDICATION_CODE:
		erab_mod_indication_handler(initiating_p);
		break;

	default:
		log_msg(LOG_ERROR, "Unknown procedure code - %lu", initiating_p->procedureCode & 0x00FF);
		break;
	}
	
	return 0;
}

int convertUplinkNasToProtoIe(InitiatingMessage_t *msg, struct proto_IE* proto_ies, uplink_nas_t *s1Msg)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
	int no_of_IEs = 0;

    if(msg->value.present == InitiatingMessage__value_PR_UplinkNASTransport)
    {
        ProtocolIE_Container_129P33_t* protocolIes = &msg->value.choice.UplinkNASTransport.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if(proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR,"Malloc failed for protocol IE.");
            return -1;
        }
		for (int i = 0; i < protocolIes->list.count; i++) {
			UplinkNASTransport_IEs_t *ie_p;
			ie_p = protocolIes->list.array[i];
			switch(ie_p->id) {
				case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
					{
                        ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                        if(UplinkNASTransport_IEs__value_PR_ENB_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                        }
                        else 
						{
							log_msg (LOG_ERROR, "Decoding of IE eNB_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID; 
						memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id, s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
						s1Msg->header.s1ap_enb_ue_id = proto_ies->data[i].val.enb_ue_s1ap_id;
					} break;
				case ProtocolIE_ID_id_MME_UE_S1AP_ID:
					{
                        MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                        if(UplinkNASTransport_IEs__value_PR_MME_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                        }
						
                        if (s1apMMEUES1APID_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE MME_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID; 
						memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
						s1Msg->header.ue_idx = proto_ies->data[i].val.mme_ue_s1ap_id;
					} break;
				case ProtocolIE_ID_id_NAS_PDU:
					{
                        NAS_PDU_t *s1apNASPDU_p = NULL;
                        if(UplinkNASTransport_IEs__value_PR_NAS_PDU == ie_p->value.present)
                        {
						    s1apNASPDU_p = &ie_p->value.choice.NAS_PDU;
                        } 
						else
                        {
							log_msg (LOG_ERROR, "Decoding of IE NAS PDU failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_NAS_PDU; 
						memcpy(s1Msg->nasMsg.nasMsgBuf, (char*)s1apNASPDU_p->buf, s1apNASPDU_p->size);
						s1Msg->nasMsg.nasMsgSize = s1apNASPDU_p->size;

					} break;
				case ProtocolIE_ID_id_TAI:
					{
                        TAI_t *s1apTAI_p = NULL;
                        if(UplinkNASTransport_IEs__value_PR_TAI == ie_p->value.present)
                        {
						    s1apTAI_p = &ie_p->value.choice.TAI;
                        }
						
                        if (s1apTAI_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE TAI failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_TAI; 
						memcpy(&proto_ies->data[i].val.tai.tac, s1apTAI_p->tAC.buf, s1apTAI_p->tAC.size);
						memcpy(proto_ies->data[i].val.tai.plmn_id.idx, 
                                s1apTAI_p->pLMNidentity.buf, s1apTAI_p->pLMNidentity.size);
                        memcpy(&(s1Msg->tai), &(proto_ies->data[i].val.tai), sizeof(struct TAI));
					} break;
				case ProtocolIE_ID_id_EUTRAN_CGI:
					{
			            EUTRAN_CGI_t*	 s1apCGI_p = NULL;;
                        if(UplinkNASTransport_IEs__value_PR_EUTRAN_CGI == ie_p->value.present)
                        {
						    s1apCGI_p = &ie_p->value.choice.EUTRAN_CGI;
                        }
						
                        if (s1apCGI_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE CGI failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_UTRAN_CGI; 
						memcpy(&proto_ies->data[i].val.utran_cgi.cell_id, 
                               s1apCGI_p->cell_ID.buf, s1apCGI_p->cell_ID.size);
						memcpy(proto_ies->data[i].val.utran_cgi.plmn_id.idx, 
                                s1apCGI_p->pLMNidentity.buf, s1apCGI_p->pLMNidentity.size);
                        memcpy(&(s1Msg->utran_cgi), &(proto_ies->data[i].val.utran_cgi), sizeof(struct CGI));
					} break;
                default:
                    {
                        proto_ies->data[i].IE_type = ie_p->id;
                        log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
                    }
			}
		}
     }

    return 0;
}

int convertInitCtxRspToProtoIe(SuccessfulOutcome_t *msg, struct proto_IE* proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
	int no_of_IEs = 0;

    if(msg->value.present == SuccessfulOutcome__value_PR_InitialContextSetupResponse)
    {
        ProtocolIE_Container_129P20_t* protocolIes 
            = &msg->value.choice.InitialContextSetupResponse.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if(proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR,"Malloc failed for protocol IE.");
            return -1;
        }
		for (int i = 0; i < protocolIes->list.count; i++) {
			InitialContextSetupResponseIEs_t *ie_p;
			ie_p = protocolIes->list.array[i];
			switch(ie_p->id) {
				case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
					{
                        ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                        if(InitialContextSetupResponseIEs__value_PR_ENB_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                        }
						
                        if (s1apENBUES1APID_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE eNB_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID; 
						memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id, s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
						s1apENBUES1APID_p = NULL;

					} break;
				case ProtocolIE_ID_id_MME_UE_S1AP_ID:
					{
                        MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                        if(InitialContextSetupResponseIEs__value_PR_MME_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                        }
                        else 
						{
							log_msg (LOG_ERROR, "Decoding of IE MME_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID; 
						memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
						s1apMMEUES1APID_p = NULL;

					} break;
				case ProtocolIE_ID_id_E_RABSetupListCtxtSURes:
					{
                        E_RABSetupListCtxtSURes_t *s1apErabSetupList_p = NULL;
                        if(InitialContextSetupResponseIEs__value_PR_E_RABSetupListCtxtSURes == ie_p->value.present)
                        {
						    s1apErabSetupList_p = &ie_p->value.choice.E_RABSetupListCtxtSURes;
                        }
						
                        if (s1apErabSetupList_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE s1apErabSetupList failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_ERAB_SETUP_CTX_SUR;
                        proto_ies->data[i].val.erab.no_of_elements = s1apErabSetupList_p->list.count;
                        proto_ies->data[i].val.erab.elements = calloc(sizeof(union eRAB_IE), 
                                                                    s1apErabSetupList_p->list.count);
                        if(proto_ies->data[i].val.erab.elements == NULL)
                        {
                            log_msg(LOG_ERROR,"Malloc failed for protocol IE: Erab elements.");
                            break;
                        }
                        for (int j = 0; 
                             j < s1apErabSetupList_p->list.count; j++) 
                        {
                            E_RABSetupItemCtxtSUResIEs_t *ie_p;
                            ie_p = (E_RABSetupItemCtxtSUResIEs_t*)s1apErabSetupList_p->list.array[j];
                            switch(ie_p->id) {
                                case ProtocolIE_ID_id_E_RABSetupItemCtxtSURes:
                                    {
                                        E_RABSetupItemCtxtSURes_t* s1apErabSetupItem_p = NULL;
                                        if(E_RABSetupItemCtxtSUResIEs__value_PR_E_RABSetupItemCtxtSURes == ie_p->value.present)
                                        {
                                            s1apErabSetupItem_p = &ie_p->value.choice.E_RABSetupItemCtxtSURes;
                                        }

                                        if (s1apErabSetupItem_p == NULL) {
                                            log_msg (LOG_ERROR, "Decoding of IE s1apErabSetupItem failed");
                                            return -1;
                                        }

                                        proto_ies->data[i].val.erab.elements[j].su_res.eRAB_id 
                                                 = (unsigned short)s1apErabSetupItem_p->e_RAB_ID;

					if(s1apErabSetupItem_p->gTP_TEID.buf != NULL)
					{
                                            memcpy(
                                                &(proto_ies->data[i].val.erab.elements[j].su_res.gtp_teid),
                                                s1apErabSetupItem_p->gTP_TEID.buf,
                                                s1apErabSetupItem_p->gTP_TEID.size);
                                            proto_ies->data[i].val.erab.elements[j].su_res.gtp_teid
                                                = ntohl(proto_ies->data[i].val.erab.elements[j].su_res.gtp_teid);

					}
					else
					{
                                            log_msg(LOG_ERROR,
                                            "Decoding of IE E_RABSetupItemCtxtSURes->gTP_TEID failed");
                                            return -1;
                                        }

					if(s1apErabSetupItem_p->transportLayerAddress.buf != NULL)
					{
                                            memcpy(
                                                &(proto_ies->data[i].val.erab.elements[j].su_res.transp_layer_addr),
                                                s1apErabSetupItem_p->transportLayerAddress.buf,
                                                s1apErabSetupItem_p->transportLayerAddress.size);
                                            proto_ies->data[i].val.erab.elements[j].su_res.transp_layer_addr
                                                = ntohl(proto_ies->data[i].val.erab.elements[j].su_res.transp_layer_addr);
					}
					else
					{
					    log_msg(LOG_ERROR,
                                            "Decoding of IE E_RABSetupItemCtxtSURes->transp_layer_addr failed");
                                            return -1;
                    }
                                    }break;
                                default:
                                    {
                                        log_msg(LOG_WARNING, "Unhandled List item %lu", ie_p->id);
                                    }
                            }
                        }

		        } break;
                default:
                    {
                        proto_ies->data[i].IE_type = ie_p->id;
                        log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
                    }
            }
        }
    }

    return 0;
}

int convertUeCtxRelComplToProtoIe(SuccessfulOutcome_t *msg, struct proto_IE* proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
	int no_of_IEs = 0;

    if(msg->value.present == SuccessfulOutcome__value_PR_UEContextReleaseComplete)
    {
        ProtocolIE_Container_129P25_t* protocolIes 
            = &msg->value.choice.UEContextReleaseComplete.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if(proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR,"Malloc failed for protocol IE.");
            return -1;
        }
		for (int i = 0; i < protocolIes->list.count; i++) {
			UEContextReleaseComplete_IEs_t *ie_p;
			ie_p = protocolIes->list.array[i];
			switch(ie_p->id) {
				case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
					{
                        ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                        if(UEContextReleaseComplete_IEs__value_PR_ENB_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                        }
						
                        if (s1apENBUES1APID_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE eNB_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID; 
						memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id, s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
						s1apENBUES1APID_p = NULL;
					} break;
				case ProtocolIE_ID_id_MME_UE_S1AP_ID:
					{
                        MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                        if(UEContextReleaseComplete_IEs__value_PR_MME_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                        }
						
                        if (s1apMMEUES1APID_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE MME_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID; 
						memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
						s1apMMEUES1APID_p = NULL;
					} break;
                default:
                    {
                        proto_ies->data[i].IE_type = ie_p->id;
                        log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
                    }
			}
		}
     }

    return 0;
}

int convertUeCtxRelReqToProtoIe(InitiatingMessage_t *msg, struct proto_IE* proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
	int no_of_IEs = 0;

    if(msg->value.present == InitiatingMessage__value_PR_UEContextReleaseRequest)
    {
        ProtocolIE_Container_129P23_t* protocolIes
            = &msg->value.choice.UEContextReleaseRequest.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if(proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR,"Malloc failed for protocol IE.");
            return -1;
        }
		for (int i = 0; i < protocolIes->list.count; i++) {
			UEContextReleaseRequest_IEs_t *ie_p;
			ie_p = protocolIes->list.array[i];
			switch(ie_p->id) {
				case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
					{
                        ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                        if(UEContextReleaseRequest_IEs__value_PR_ENB_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                        }

                        if (s1apENBUES1APID_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE eNB_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
						memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id, s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
						s1apENBUES1APID_p = NULL;
					} break;
				case ProtocolIE_ID_id_MME_UE_S1AP_ID:
					{
                        MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                        if(UEContextReleaseRequest_IEs__value_PR_MME_UE_S1AP_ID == ie_p->value.present)
                        {
						    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                        }

                        if (s1apMMEUES1APID_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE MME_UE_S1AP_ID failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
						memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id, s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
						s1apMMEUES1APID_p = NULL;
					} break;
				case ProtocolIE_ID_id_Cause:
					{
                        Cause_t *s1apCause_p = NULL;
                        if(UEContextReleaseRequest_IEs__value_PR_Cause == ie_p->value.present)
                        {
						    s1apCause_p = &ie_p->value.choice.Cause;
                        }

                        if (s1apCause_p == NULL) {
							log_msg (LOG_ERROR, "Decoding of IE Cause failed");
							return -1;
						}

                        proto_ies->data[i].IE_type = S1AP_IE_CAUSE;
                        proto_ies->data[i].val.cause.present = s1apCause_p->present;
                        switch(s1apCause_p->present)
                        {
                            case Cause_PR_radioNetwork:
							    log_msg (LOG_DEBUG, "RadioNetwork case : %lu",
                                          s1apCause_p->choice.radioNetwork);
                                proto_ies->data[i].val.cause.choice.radioNetwork
                                    = s1apCause_p->choice.radioNetwork;
                                break;
                            case Cause_PR_transport:
							    log_msg (LOG_DEBUG, "Transport case : %lu",
                                          s1apCause_p->choice.transport);
                                proto_ies->data[i].val.cause.choice.transport
                                    = s1apCause_p->choice.transport;
                                break;
                            case Cause_PR_nas:
							    log_msg (LOG_DEBUG, "Nas case : %lu",
                                          s1apCause_p->choice.nas);
                                proto_ies->data[i].val.cause.choice.nas
                                    = s1apCause_p->choice.nas;
                                break;
                            case Cause_PR_protocol:
							    log_msg (LOG_DEBUG, "Protocol case : %lu",
                                          s1apCause_p->choice.protocol);
                                proto_ies->data[i].val.cause.choice.protocol
                                    = s1apCause_p->choice.protocol;
                                break;
                            case Cause_PR_misc:
							    log_msg (LOG_DEBUG, "Misc case : %lu",
                                          s1apCause_p->choice.misc);
                                proto_ies->data[i].val.cause.choice.misc
                                    = s1apCause_p->choice.misc;
                                break;
                            case Cause_PR_NOTHING:
                            default:
                                log_msg(LOG_WARNING, "Unknown cause %d", s1apCause_p->present);

                        }
						s1apCause_p = NULL;
					} break;
                default:
                    {
                        proto_ies->data[i].IE_type = ie_p->id;
                        log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
                    }
			}
		}
     }

    return 0;
}

int convertUehoReqToProtoIe(InitiatingMessage_t *msg,
        struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if (msg->value.present == InitiatingMessage__value_PR_HandoverRequired)
    {
        ProtocolIE_Container_129P0_t *protocolIes =
                &msg->value.choice.HandoverRequired.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if (proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR, "calloc failed for protocol IE.");
            return -1;
        }
        for (int i = 0; i < protocolIes->list.count; i++)
        {
            HandoverRequiredIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                if (HandoverRequiredIEs__value_PR_ENB_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                }

                if (s1apENBUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
                memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id,
                        s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                s1apENBUES1APID_p = NULL;
            } break;
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                if (HandoverRequiredIEs__value_PR_MME_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                }

                if (s1apMMEUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE MME_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                        s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                s1apMMEUES1APID_p = NULL;
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t *s1apCause_p = NULL;
                if (HandoverRequiredIEs__value_PR_Cause == ie_p->value.present)
                {
                    s1apCause_p = &ie_p->value.choice.Cause;
                }

                if (s1apCause_p == NULL)
                {
                    log_msg(LOG_ERROR, "Decoding of IE Cause failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_CAUSE;
                proto_ies->data[i].val.cause.present = s1apCause_p->present;
                switch (s1apCause_p->present)
                {
                case Cause_PR_radioNetwork:
                    log_msg(LOG_DEBUG, "RadioNetwork case : %lu",
                            s1apCause_p->choice.radioNetwork);
                    proto_ies->data[i].val.cause.choice.radioNetwork =
                            s1apCause_p->choice.radioNetwork;
                    break;
                default:
                    log_msg(LOG_WARNING, "Unknown cause %d",
                            s1apCause_p->present);

                }
                s1apCause_p = NULL;
            } break;
            case ProtocolIE_ID_id_HandoverType:
            {
                HandoverType_t *handoverType_p = NULL;
                if (HandoverRequiredIEs__value_PR_HandoverType
                        == ie_p->value.present)
                {
                    handoverType_p = &ie_p->value.choice.HandoverType;
                }

                if (handoverType_p == NULL)
                {
                    log_msg(LOG_ERROR, "Decoding of IE HandoverType failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_HANDOVER_TYPE;
                memcpy(&proto_ies->data[i].val.handoverType, handoverType_p,
                        sizeof(HandoverType_t));
                handoverType_p = NULL;
            } break;
            case ProtocolIE_ID_id_TargetID:
            {
                TargetID_t *targetID_p = NULL;
                if (HandoverRequiredIEs__value_PR_TargetID
                        == ie_p->value.present)
                {
                    targetID_p = &ie_p->value.choice.TargetID;
                }

                if (targetID_p == NULL)
                {
                    log_msg(LOG_ERROR, "Decoding of IE targetID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_TARGET_ID;
                if (targetID_p->present == TargetID_PR_targeteNB_ID)
                {
                    struct TargeteNB_ID *targeteNB_ID = targetID_p->choice.targeteNB_ID;
                    if (targeteNB_ID == NULL)
                    {
                        log_msg(LOG_ERROR, "Decoding of IE targeteNB_ID failed");
                        return -1;
                    }

                    TAI_t *s1apTAI_p = &(targeteNB_ID->selected_TAI);
                    if (s1apTAI_p == NULL)
                    {
                        log_msg (LOG_ERROR, "Decoding of IE TAI failed");
                        return -1;
                    }

                    memcpy(&proto_ies->data[i].val.target_id.selected_tai.tac,
                            s1apTAI_p->tAC.buf, s1apTAI_p->tAC.size);
                    memcpy(proto_ies->data[i].val.target_id.selected_tai.plmn_id.idx,
                            s1apTAI_p->pLMNidentity.buf, s1apTAI_p->pLMNidentity.size);

                    if (targeteNB_ID->global_ENB_ID.eNB_ID.present == ENB_ID_PR_macroENB_ID)
                    {
                        memcpy(proto_ies->data[i].val.target_id.global_enb_id.macro_enb_id,
                                targeteNB_ID->global_ENB_ID.eNB_ID.choice.macroENB_ID.buf,
                                targeteNB_ID->global_ENB_ID.eNB_ID.choice.macroENB_ID.size);
                    }

                }

                targetID_p = NULL;
            } break;
            case ProtocolIE_ID_id_Source_ToTarget_TransparentContainer:
            {
                Source_ToTarget_TransparentContainer_t *source_ToTarget_TransparentContainer_p = NULL;
                if (HandoverRequiredIEs__value_PR_Source_ToTarget_TransparentContainer
                        == ie_p->value.present)
                {
                    source_ToTarget_TransparentContainer_p =
                            &ie_p->value.choice.Source_ToTarget_TransparentContainer;
                }

                if (source_ToTarget_TransparentContainer_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE Source_ToTarget_TransparentContainer failed");
                    return -1;
                }

                proto_ies->data[i].IE_type =
                S1AP_IE_SOURCE_TOTARGET_TRANSPARENTCONTAINER;

                proto_ies->data[i].val.srcToTargetTranspContainer.size =
                        source_ToTarget_TransparentContainer_p->size;
               proto_ies->data[i].val.srcToTargetTranspContainer.buffer_p =
                        source_ToTarget_TransparentContainer_p->buf;
                source_ToTarget_TransparentContainer_p = NULL;
            } break;
            default:
            {
                proto_ies->data[i].IE_type = ie_p->id;
                log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
            }
            }
        }
    }

    return 0;
}

int convertHoAcklToProtoIe(SuccessfulOutcome_t *msg, struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if (msg->value.present
            == SuccessfulOutcome__value_PR_HandoverRequestAcknowledge)
    {
        ProtocolIE_Container_129P4_t *protocolIes =
                &msg->value.choice.HandoverRequestAcknowledge.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if (proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR, "Calloc failed for protocol IE.");
            return -1;
        }
        for (int i = 0; i < protocolIes->list.count; i++)
        {
            HandoverRequestAcknowledgeIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                if (HandoverRequestAcknowledgeIEs__value_PR_ENB_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                }

                if (s1apENBUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
                memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id,
                        s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                s1apENBUES1APID_p = NULL;
            } break;
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                if (HandoverRequestAcknowledgeIEs__value_PR_MME_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                }

                if (s1apMMEUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE MME_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                        s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                s1apMMEUES1APID_p = NULL;
            } break;
            case ProtocolIE_ID_id_E_RABAdmittedList:
            {
                E_RABAdmittedList_t *e_RABAdmittedList_p = NULL;
                if (HandoverRequestAcknowledgeIEs__value_PR_E_RABAdmittedList
                        == ie_p->value.present)
                {
                    e_RABAdmittedList_p = &ie_p->value.choice.E_RABAdmittedList;
                }

                if (e_RABAdmittedList_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE E_RABAdmittedList failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_E_RAB_ADMITTED;
                proto_ies->data[i].val.erab_admittedlist.count =
                        e_RABAdmittedList_p->list.count;

                for (int j = 0; j < e_RABAdmittedList_p->list.count; j++)
                {
                    E_RABAdmittedItemIEs_t *ie_p;
                    ie_p =
                            (E_RABAdmittedItemIEs_t*) e_RABAdmittedList_p->list.array[j];
                    switch (ie_p->id)
                    {
                    case ProtocolIE_ID_id_E_RABAdmittedItem:
                    {
                        E_RABAdmittedItem_t *eRabAdmittedItem_p = NULL;
                        if (E_RABAdmittedItemIEs__value_PR_E_RABAdmittedItem
                                == ie_p->value.present)
                        {
                            eRabAdmittedItem_p =
                                    &ie_p->value.choice.E_RABAdmittedItem;
                        }

                        if (eRabAdmittedItem_p == NULL)
                        {
                            log_msg(LOG_ERROR,
                                    "Decoding of IE eRABAdmittedItem failed");
                            return -1;
                        }

                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].e_RAB_ID =
                                (unsigned short) eRabAdmittedItem_p->e_RAB_ID;
                        memcpy(
                                &(proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].gtp_teid),
                                eRabAdmittedItem_p->gTP_TEID.buf,
                                eRabAdmittedItem_p->gTP_TEID.size);

                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].gtp_teid =
                                ntohl(
                                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].gtp_teid);

                        memcpy(
                                &(proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].transportLayerAddress),
                                eRabAdmittedItem_p->transportLayerAddress.buf,
                                eRabAdmittedItem_p->transportLayerAddress.size);

                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].transportLayerAddress =
                                ntohl(
                                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].transportLayerAddress);

                        memcpy(
                                &(proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].dL_gtp_teid),
                                eRabAdmittedItem_p->dL_gTP_TEID->buf,
                                eRabAdmittedItem_p->dL_gTP_TEID->size);

                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].dL_gtp_teid =
                                ntohl(
                                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].dL_gtp_teid);

                        memcpy(
                                &(proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].dL_transportLayerAddress),
                                eRabAdmittedItem_p->dL_transportLayerAddress->buf,
                                eRabAdmittedItem_p->dL_transportLayerAddress->size);

                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].dL_transportLayerAddress =
                                ntohl(
                                        proto_ies->data[i].val.erab_admittedlist.erab_admitted[0].dL_transportLayerAddress);

                        eRabAdmittedItem_p = NULL;
                    }
                        break;
                    default:
                    {
                        log_msg(LOG_WARNING, "Unhandled List item %lu", ie_p->id);
                    }
                    }
                }
                e_RABAdmittedList_p = NULL;
            } break;
            case ProtocolIE_ID_id_Target_ToSource_TransparentContainer:
            {
                Target_ToSource_TransparentContainer_t *target_ToSource_TransparentContainer_p = NULL;
                if (HandoverRequestAcknowledgeIEs__value_PR_Target_ToSource_TransparentContainer
                        == ie_p->value.present)
                {
                    target_ToSource_TransparentContainer_p =
                            &ie_p->value.choice.Target_ToSource_TransparentContainer;
                }

                if (target_ToSource_TransparentContainer_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE Target_ToSource_TransparentContainer failed");
                    return -1;
                }

                proto_ies->data[i].IE_type =
                S1AP_IE_TARGET_TOSOURCE_TRANSPARENTCONTAINER;

                proto_ies->data[i].val.targetToSrcTranspContainer.size =
                        target_ToSource_TransparentContainer_p->size;
                proto_ies->data[i].val.targetToSrcTranspContainer.buffer_p =
                        target_ToSource_TransparentContainer_p->buf;

                target_ToSource_TransparentContainer_p = NULL;
            }break;
            default:
            {
                proto_ies->data[i].IE_type = ie_p->id;
                log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
            }

            }
        }
    }
    return 0;
}

int convertHoNotifyToProtoIe(InitiatingMessage_t *msg,
        struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if (msg->value.present == InitiatingMessage__value_PR_HandoverNotify)
    {
        ProtocolIE_Container_129P6_t *protocolIes =
                &msg->value.choice.HandoverNotify.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if (proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR, "Calloc failed for protocol IE.");
            return -1;
        }
        for (int i = 0; i < protocolIes->list.count; i++)
        {
            HandoverNotifyIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                if (HandoverNotifyIEs__value_PR_ENB_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                }

                if (s1apENBUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
                memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id,
                        s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                s1apENBUES1APID_p = NULL;
            }
                break;
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                if (HandoverNotifyIEs__value_PR_MME_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                }

                if (s1apMMEUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE MME_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                        s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                s1apMMEUES1APID_p = NULL;
            }
                break;
            case ProtocolIE_ID_id_EUTRAN_CGI:
            {
                EUTRAN_CGI_t *s1apCGI_p = NULL;
                ;
                if (HandoverNotifyIEs__value_PR_EUTRAN_CGI
                        == ie_p->value.present)
                {
                    s1apCGI_p = &ie_p->value.choice.EUTRAN_CGI;
                }

                if (s1apCGI_p == NULL)
                {
                    log_msg(LOG_ERROR, "Decoding of IE CGI failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_UTRAN_CGI;
                memcpy(&proto_ies->data[i].val.utran_cgi.cell_id,
                        s1apCGI_p->cell_ID.buf, s1apCGI_p->cell_ID.size);
                memcpy(&proto_ies->data[i].val.utran_cgi.plmn_id.idx,
                        s1apCGI_p->pLMNidentity.buf,
                        s1apCGI_p->pLMNidentity.size);
            }
                break;
            case ProtocolIE_ID_id_TAI:
            {
                TAI_t *s1apTAI_p = NULL;
                if (HandoverNotifyIEs__value_PR_TAI == ie_p->value.present)
                {
                    s1apTAI_p = &ie_p->value.choice.TAI;
                }

                if (s1apTAI_p == NULL)
                {
                    log_msg(LOG_ERROR, "Decoding of IE TAI failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_TAI;
                memcpy(&proto_ies->data[i].val.tai.tac, s1apTAI_p->tAC.buf,
                        s1apTAI_p->tAC.size);
                memcpy(&proto_ies->data[i].val.tai.plmn_id,
                        s1apTAI_p->pLMNidentity.buf,
                        s1apTAI_p->pLMNidentity.size);
            }
                break;
            default:
            {
                proto_ies->data[i].IE_type = ie_p->id;
                log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
            }

            }
        }
    }

    return 0;
}

int convertEnbStatusTransferToProtoIe(InitiatingMessage_t *msg,
        struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if (msg->value.present == InitiatingMessage__value_PR_ENBStatusTransfer)
    {
        ProtocolIE_Container_129P52_t *protocolIes =
                &msg->value.choice.ENBStatusTransfer.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if (proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR, "Calloc failed for protocol IE.");
            return -1;
        }
        for (int i = 0; i < protocolIes->list.count; i++)
        {
            ENBStatusTransferIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                if (ENBStatusTransferIEs__value_PR_ENB_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                }

                if (s1apENBUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
                memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id,
                        s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                s1apENBUES1APID_p = NULL;
            }
                break;
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                if (ENBStatusTransferIEs__value_PR_MME_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                }

                if (s1apMMEUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE MME_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                        s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                s1apMMEUES1APID_p = NULL;
            }
                break;
            case ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer:
            {
                ENB_StatusTransfer_TransparentContainer_t *s1apContainer_p =
                NULL;
                ;
                if (ENBStatusTransferIEs__value_PR_ENB_StatusTransfer_TransparentContainer
                        == ie_p->value.present)
                {
                    s1apContainer_p =
                            &ie_p->value.choice.ENB_StatusTransfer_TransparentContainer;
                }

                if (s1apContainer_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE ENB_STATUS_TRANSFER_TRANSPARENTCONTAINER failed");
                    return -1;
                }

                proto_ies->data[i].IE_type =
                S1AP_IE_ENB_STATUS_TRANSFER_TRANSPARENTCONTAINER;
                proto_ies->data[i].val.enB_status_transfer_transparent_containerlist.count =
                        s1apContainer_p->bearers_SubjectToStatusTransferList.list.count;

                for (int j = 0;
                        j
                                < s1apContainer_p->bearers_SubjectToStatusTransferList.list.count;
                        j++)
                {
                    Bearers_SubjectToStatusTransfer_ItemIEs_t *ie;
                    ie =
                            (Bearers_SubjectToStatusTransfer_ItemIEs_t*) s1apContainer_p->bearers_SubjectToStatusTransferList.list.array[j];
                    switch (ie->id)
                    {
                    case ProtocolIE_ID_id_Bearers_SubjectToStatusTransfer_Item:
                    {
                        Bearers_SubjectToStatusTransfer_Item_t *bearersSubjectToStatusTransferItem =
                                NULL;
                        if (Bearers_SubjectToStatusTransfer_ItemIEs__value_PR_Bearers_SubjectToStatusTransfer_Item
                                == ie->value.present)
                        {
                            bearersSubjectToStatusTransferItem =
                                    &ie->value.choice.Bearers_SubjectToStatusTransfer_Item;
                        }

                        if (bearersSubjectToStatusTransferItem == NULL)
                        {
                            log_msg(LOG_ERROR,
                                    "Decoding of IE bearersSubjectToStatusTransferItem failed");
                            return -1;
                        }

                        proto_ies->data[i].val.enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container[j].eRAB_id =
                                bearersSubjectToStatusTransferItem->e_RAB_ID;

                        proto_ies->data[i].val.enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container[j].dl_count_value.pdcp_sn =
                                bearersSubjectToStatusTransferItem->dL_COUNTvalue.pDCP_SN;

                        proto_ies->data[i].val.enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container[j].dl_count_value.hfn =
                                bearersSubjectToStatusTransferItem->dL_COUNTvalue.hFN;

                        proto_ies->data[i].val.enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container[j].ul_count_value.pdcp_sn =
                                bearersSubjectToStatusTransferItem->uL_COUNTvalue.pDCP_SN;

                        proto_ies->data[i].val.enB_status_transfer_transparent_containerlist.enB_status_transfer_transparent_container[j].ul_count_value.hfn =
                                bearersSubjectToStatusTransferItem->uL_COUNTvalue.hFN;

                    }
                        break;
                    default:
                    {
                        log_msg(LOG_WARNING, "Unhandled List item %lu", ie->id);
                    }

                    }
                }
            }
                break;
            default:
            {
                proto_ies->data[i].IE_type = ie_p->id;
                log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
            }

            }
        }
    }

    return 0;
}

int convertHoFailureToProtoIe(UnsuccessfulOutcome_t *msg,
        struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if (msg->value.present == UnsuccessfulOutcome__value_PR_HandoverFailure)
    {
        ProtocolIE_Container_129P5_t *protocolIes =
                &msg->value.choice.HandoverFailure.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if (proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR, "calloc failed for protocol IE.");
            return -1;
        }
        for (int i = 0; i < protocolIes->list.count; i++)
        {
            HandoverFailureIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                if (HandoverFailureIEs__value_PR_MME_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                }

                if (s1apMMEUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE MME_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                        s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                s1apMMEUES1APID_p = NULL;
            }
                break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t *s1apCause_p = NULL;
                if (HandoverFailureIEs__value_PR_Cause == ie_p->value.present)
                {
                    s1apCause_p = &ie_p->value.choice.Cause;
                }

                if (s1apCause_p == NULL)
                {
                    log_msg(LOG_ERROR, "Decoding of IE Cause failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_CAUSE;
                proto_ies->data[i].val.cause.present = s1apCause_p->present;
                switch (s1apCause_p->present)
                {
                case Cause_PR_radioNetwork:
                    log_msg(LOG_DEBUG, "RadioNetwork case : %lu",
                            s1apCause_p->choice.radioNetwork);
                    proto_ies->data[i].val.cause.choice.radioNetwork =
                            s1apCause_p->choice.radioNetwork;
                    break;
                default:
                    log_msg(LOG_WARNING, "Unknown cause %d",
                            s1apCause_p->present);

                }
                s1apCause_p = NULL;
            }
                break;
            default:
            {
                proto_ies->data[i].IE_type = ie_p->id;
                log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
            }

            }
        }
    }
    return 0;
}

int convertUeHoCancelToProtoIe(InitiatingMessage_t *msg,
        struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if (msg->value.present == InitiatingMessage__value_PR_HandoverCancel)
    {
        ProtocolIE_Container_129P10_t *protocolIes =
                &msg->value.choice.HandoverCancel.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        if (proto_ies->data == NULL)
        {
            log_msg(LOG_ERROR, "calloc failed for protocol IE.");
            return -1;
        }
        for (int i = 0; i < protocolIes->list.count; i++)
        {
            HandoverCancelIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                if (HandoverCancelIEs__value_PR_ENB_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                }

                if (s1apENBUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
                memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id,
                        s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                s1apENBUES1APID_p = NULL;
            }
                break;
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                if (HandoverCancelIEs__value_PR_MME_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                }

                if (s1apMMEUES1APID_p == NULL)
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE MME_UE_S1AP_ID failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                        s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                s1apMMEUES1APID_p = NULL;
            }
                break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t *s1apCause_p = NULL;
                if (HandoverCancelIEs__value_PR_Cause == ie_p->value.present)
                {
                    s1apCause_p = &ie_p->value.choice.Cause;
                }

                if (s1apCause_p == NULL)
                {
                    log_msg(LOG_ERROR, "Decoding of IE Cause failed");
                    return -1;
                }

                proto_ies->data[i].IE_type = S1AP_IE_CAUSE;
                proto_ies->data[i].val.cause.present = s1apCause_p->present;
                switch (s1apCause_p->present)
                {
                case Cause_PR_radioNetwork:
                    log_msg(LOG_DEBUG, "RadioNetwork case : %lu", s1apCause_p->choice.radioNetwork);
                    proto_ies->data[i].val.cause.choice.radioNetwork =
                            s1apCause_p->choice.radioNetwork;
                    break;
                default:
                    log_msg(LOG_WARNING, "Unknown cause %d", s1apCause_p->present);

                }
                s1apCause_p = NULL;
            }
            break;
            default:
            {
                proto_ies->data[i].IE_type = ie_p->id;
                log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
            }
            }
        }
    }

    return 0;
}

int convertErabModIndToProtoIe(InitiatingMessage_t *msg, struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if (msg->value.present
            == InitiatingMessage__value_PR_E_RABModificationIndication)
    {
        ProtocolIE_Container_129P77_t *protocolIes =
                &msg->value.choice.E_RABModificationIndication.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);
        
        for (int i = 0; i < protocolIes->list.count; i++)
        {
            E_RABModificationIndicationIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                if (E_RABModificationIndicationIEs__value_PR_ENB_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                }
		
		if (s1apENBUES1APID_p != NULL)
                {
                    proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
                    memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id,
                            s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                }
		else
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
            } break;
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                if (E_RABModificationIndicationIEs__value_PR_MME_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                }

                if (s1apMMEUES1APID_p != NULL)
                {
                    proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                    memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                            s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                }
		else
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE MME_UE_S1AP_ID failed");
                    return -1;
                }
            } break;
            case ProtocolIE_ID_id_E_RABToBeModifiedListBearerModInd:
            {
                E_RABToBeModifiedListBearerModInd_t *e_RABToBeModifiedList_p = NULL;
                if (E_RABModificationIndicationIEs__value_PR_E_RABToBeModifiedListBearerModInd
                        == ie_p->value.present)
                {
                    e_RABToBeModifiedList_p = &ie_p->value.choice.E_RABToBeModifiedListBearerModInd;
                }

		if (e_RABToBeModifiedList_p != NULL)
		{
                    proto_ies->data[i].IE_type = S1AP_IE_E_RAB_TO_BE_MOD_LIST_BEARER_MOD_IND;
                    proto_ies->data[i].val.erab_to_be_mod_list.count =
                            e_RABToBeModifiedList_p->list.count;

                    for (int j = 0; j < e_RABToBeModifiedList_p->list.count; j++)
                    {
                        E_RABToBeModifiedItemBearerModIndIEs_t *ie_p;
                        ie_p = (E_RABToBeModifiedItemBearerModIndIEs_t*) e_RABToBeModifiedList_p->list.array[j];
                        switch (ie_p->id)
                        {
                            case ProtocolIE_ID_id_E_RABToBeModifiedItemBearerModInd:
			    {
                        	E_RABToBeModifiedItemBearerModInd_t *eRabToBeModifiedItem_p = NULL;
                        	if (E_RABToBeModifiedItemBearerModIndIEs__value_PR_E_RABToBeModifiedItemBearerModInd
                                	== ie_p->value.present)
                        	{
                            	    eRabToBeModifiedItem_p =
                                        &ie_p->value.choice.E_RABToBeModifiedItemBearerModInd;
                        	}

                        	if (eRabToBeModifiedItem_p != NULL)
				{
                      		    proto_ies->data[i].val.erab_to_be_mod_list.erab_to_be_mod_item[j].e_RAB_ID =
                                        (uint8_t) eRabToBeModifiedItem_p->e_RAB_ID;

				    if (eRabToBeModifiedItem_p->dL_GTP_TEID.buf != NULL)
				    {
                        		memcpy(
                                	&(proto_ies->data[i].val.erab_to_be_mod_list.erab_to_be_mod_item[j].dl_gtp_teid),
                               		eRabToBeModifiedItem_p->dL_GTP_TEID.buf, eRabToBeModifiedItem_p->dL_GTP_TEID.size);

                        		proto_ies->data[i].val.erab_to_be_mod_list.erab_to_be_mod_item[j].dl_gtp_teid =
                                	    ntohl(
                                            proto_ies->data[i].val.erab_to_be_mod_list.erab_to_be_mod_item[j].dl_gtp_teid);
				    }
				    else
				    {
                                        log_msg(LOG_ERROR,
                                        "Decoding of IE E_RABToBeModifiedItemBearerModInd->dL_GTP_TEID failed");
                                        return -1;
                                    }

                        	    if (eRabToBeModifiedItem_p->transportLayerAddress.buf != NULL)
				    {
					memcpy(
                                	&(proto_ies->data[i].val.erab_to_be_mod_list.erab_to_be_mod_item[j].transportLayerAddress),
                                	eRabToBeModifiedItem_p->transportLayerAddress.buf,
                                	eRabToBeModifiedItem_p->transportLayerAddress.size);

                        		proto_ies->data[i].val.erab_to_be_mod_list.erab_to_be_mod_item[j].transportLayerAddress =
                                	    ntohl(
                                       	    proto_ies->data[i].val.erab_to_be_mod_list.erab_to_be_mod_item[j].transportLayerAddress);
				    }
				    else
				    {
                                        log_msg(LOG_ERROR,
                                        "Decoding of IE E_RABToBeModifiedItemBearerModInd->transportLayerAddress failed");
                                        return -1;
                                    }
				}
				else
                                {
                                    log_msg(LOG_ERROR,
                                        "Decoding of IE E_RABToBeModifiedItemBearerModInd failed");
                                    return -1;
                                }
                    	    }break;
                    	    default:
                    	    {
                                log_msg(LOG_WARNING, "Unhandled List item %lu", ie_p->id);
                            }
                        }
		            }
                }
		        else
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE E_RABToBeModifiedListBearerModInd failed");
                    return -1;
                }
            } 
            break;
	        default:
            {
                proto_ies->data[i].IE_type = ie_p->id;
		        log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
            }

            }
        }
    }
    return 0;
}

int convertErabSetupRespToProtoIe(SuccessfulOutcome_t *msg, struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if (msg->value.present
            == SuccessfulOutcome__value_PR_E_RABSetupResponse)
    {
        ProtocolIE_Container_129P13_t *protocolIes =
                &msg->value.choice.E_RABSetupResponse.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        log_msg(LOG_INFO, "No of IEs = %d", no_of_IEs);
        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);

        for (int i = 0; i < protocolIes->list.count; i++)
        {
            E_RABSetupResponseIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                if (E_RABSetupResponseIEs__value_PR_ENB_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                }

		if (s1apENBUES1APID_p != NULL)
                {
                    proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
                    memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id,
                            s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                }
		else
                {
                    log_msg(LOG_ERROR,
                            "Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
            } break;
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                if (E_RABSetupResponseIEs__value_PR_MME_UE_S1AP_ID
                        == ie_p->value.present)
                {
                    s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                }

                if (s1apMMEUES1APID_p != NULL)
                {
                    proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                    memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                            s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                }
		        else
                {
                    log_msg(LOG_ERROR, "Decoding of IE MME_UE_S1AP_ID failed");
                    return -1;
                }
            } break;
            case ProtocolIE_ID_id_E_RABSetupListBearerSURes:
            {
                E_RABSetupListBearerSURes_t *e_RABSetupList_p = NULL;
                if (E_RABSetupResponseIEs__value_PR_E_RABSetupListBearerSURes
                        == ie_p->value.present)
                {
                    e_RABSetupList_p = &ie_p->value.choice.E_RABSetupListBearerSURes;
                }

		        if (e_RABSetupList_p != NULL)
		        {
                    proto_ies->data[i].IE_type = S1AP_IE_E_RAB_SETUP_LIST_BEARER_SU_RES;

                    proto_ies->data[i].val.erab_su_list.count =
                            e_RABSetupList_p->list.count;

                    for (int j = 0; j < e_RABSetupList_p->list.count; j++)
                    {
                        E_RABSetupItemBearerSUResIEs_t *ie_p;
                        ie_p = (E_RABSetupItemBearerSUResIEs_t*) e_RABSetupList_p->list.array[j];
                        switch (ie_p->id)
                        {
                            case ProtocolIE_ID_id_E_RABSetupItemBearerSURes:
			                {
                        	    E_RABSetupItemBearerSURes_t *eRabSetupItem_p = NULL;
                        	    if (E_RABSetupItemBearerSUResIEs__value_PR_E_RABSetupItemBearerSURes
                                    	== ie_p->value.present)
                        	    {
                                	    eRabSetupItem_p =
                                            &ie_p->value.choice.E_RABSetupItemBearerSURes;
                        	    }

                        	    if (eRabSetupItem_p != NULL)
				                {

                      		        proto_ies->data[i].val.erab_su_list.erab_su_item[j].e_RAB_ID =
                                            (uint8_t) eRabSetupItem_p->e_RAB_ID;

				                    if (eRabSetupItem_p->gTP_TEID.buf != NULL)
				                    {
                                       	memcpy(
                                              	&(proto_ies->data[i].val.erab_su_list.erab_su_item[j].gtp_teid),
                                           		eRabSetupItem_p->gTP_TEID.buf, eRabSetupItem_p->gTP_TEID.size);

                                       	proto_ies->data[i].val.erab_su_list.erab_su_item[j].gtp_teid =
                                              	    ntohl(proto_ies->data[i].val.erab_su_list.erab_su_item[j].gtp_teid);
				                    }
				                    else
				                    {
                                            log_msg(LOG_ERROR,
                                            "Decoding of IE E_RABSetupItemBearerSURes->gTP_TEID failed");
                                            return -1;
                                    }

                        	        if (eRabSetupItem_p->transportLayerAddress.buf != NULL)
				                    {
					                    memcpy(
                                    	&(proto_ies->data[i].val.erab_su_list.erab_su_item[j].transportLayerAddress),
                                    	eRabSetupItem_p->transportLayerAddress.buf,
                                    	eRabSetupItem_p->transportLayerAddress.size);

                        	    	proto_ies->data[i].val.erab_su_list.erab_su_item[j].transportLayerAddress =
                                    	    ntohl(
                                           	    proto_ies->data[i].val.erab_su_list.erab_su_item[j].transportLayerAddress);
				                    }
                                    else
                                    {
                                        log_msg(LOG_ERROR,
                                                "Decoding of IE E_RABSetupItemBearerSURes->transportLayerAddress failed");
                                        return -1;
                                    }
                                }
                                else
                                {
                                    log_msg(LOG_ERROR,"Decoding of IE E_RABSetupItemBearerSURes failed");
                                        return -1;
                                }
                    	    }
                            break;
                    	    default:
                    	    {
                                log_msg(LOG_WARNING, "Unhandled List item %lu", ie_p->id);
                            }
                        }
		            }
                }
		        else
                {
                    log_msg(LOG_ERROR,"Decoding of IE E_RABSetupItemBearerSURes failed");
                    return -1;
                }
            } break;
	    case ProtocolIE_ID_id_E_RABFailedToSetupListBearerSURes:
	    {
		E_RABList_t *e_RABFailedToSetupList_p = NULL;
		if (E_RABSetupResponseIEs__value_PR_E_RABList
                        == ie_p->value.present)
		{
        	    e_RABFailedToSetupList_p = &ie_p->value.choice.E_RABList;
    		}

    		if (e_RABFailedToSetupList_p != NULL)
    		{
        	    proto_ies->data[i].IE_type = S1AP_IE_E_RAB_FAILED_TO_SETUP_LIST_BEARER_SU_RES;
		    proto_ies->data[i].val.erab_fail_list.count = e_RABFailedToSetupList_p->list.count;

		    for (int j = 0; j < e_RABFailedToSetupList_p->list.count; j++)
        	    {
            		E_RABItemIEs_t *ie_p;
            		ie_p = (E_RABItemIEs_t*) e_RABFailedToSetupList_p->list.array[j];
            		switch (ie_p->id)
            		{
                	    case ProtocolIE_ID_id_E_RABItem:
                	    {
                    		E_RABItem_t *eRabFailedToSetupItem_p = NULL;
                    		if (E_RABItemIEs__value_PR_E_RABItem == ie_p->value.present)
                    		{
                        	    eRabFailedToSetupItem_p = &ie_p->value.choice.E_RABItem;
                    		}

                    		if (eRabFailedToSetupItem_p != NULL)
                    		{
                        	    proto_ies->data[i].val.erab_fail_list.erab_fail_item[j].e_RAB_ID =
                                        (uint8_t) eRabFailedToSetupItem_p->e_RAB_ID;

                        	    Cause_t *s1apCause_p = &eRabFailedToSetupItem_p->cause;
				                if(s1apCause_p != NULL)
                        	    {
                            		proto_ies->data[i].val.erab_fail_list.erab_fail_item[j].cause.present =
                                        	s1apCause_p->present;
                            		switch (s1apCause_p->present)
                            		{
                                	    case Cause_PR_radioNetwork:
                                	    {
                                    		log_msg(LOG_DEBUG, "RadioNetwork case : %lu", s1apCause_p->choice.radioNetwork);
                                    		proto_ies->data[i].val.erab_fail_list.erab_fail_item[j].
                                        	cause.choice.radioNetwork =
                                            		s1apCause_p->choice.radioNetwork;
                                	    }break;
                                	    default:
                                    		log_msg(LOG_WARNING, "Unknown cause %d", s1apCause_p->present);
                            		}
                        	    }
                        	    else
                        	    {
                            		log_msg(LOG_ERROR, 
                                    		"Decoding of IE E_RABFailedToSetupItemBearerSURes->Cause failed");
                            		return -1;
                        	    }
                    		}    
                    		else
                    		{
                        	    log_msg(LOG_ERROR, "Decoding of IE E_RABFailedToSetupItemBearerSURes failed");
                        	    return -1;
                    		}
                	    } break;
                	    default:
                	    {
                    		log_msg(LOG_WARNING, "Unhandled List item %lu", ie_p->id);
                	    }
            		}
        	    }
    		}
    		else
    		{
        	    log_msg(LOG_ERROR, "Decoding of IE E_RABFailedToSetupListBearerSURes failed");
        	    return -1;
    		}
	    } break;  
	            default:
                {
                    proto_ies->data[i].IE_type = ie_p->id;
		            log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
                }
            }
        }
    }
    return 0;
}

int convertErabRelRespToProtoIe(SuccessfulOutcome_t *msg,
                                struct proto_IE *proto_ies)
{
    proto_ies->procedureCode = msg->procedureCode;
    proto_ies->criticality = msg->criticality;
    int no_of_IEs = 0;

    if(msg->value.present == SuccessfulOutcome__value_PR_E_RABReleaseResponse)
    {
        ProtocolIE_Container_129P17_t *protocolIes =
                &msg->value.choice.E_RABReleaseResponse.protocolIEs;
        no_of_IEs = protocolIes->list.count;
        proto_ies->no_of_IEs = no_of_IEs;

        log_msg(LOG_INFO, "No of IEs = %d", no_of_IEs);
        proto_ies->data = calloc(sizeof(struct proto_IE_data), no_of_IEs);

        for (int i = 0; i < protocolIes->list.count; i++)
        {
            E_RABReleaseResponseIEs_t *ie_p;
            ie_p = protocolIes->list.array[i];
            switch (ie_p->id)
            {
                case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
                {
                    ENB_UE_S1AP_ID_t *s1apENBUES1APID_p = NULL;
                    if(E_RABReleaseResponseIEs__value_PR_ENB_UE_S1AP_ID
                            == ie_p->value.present)
                    {
                        s1apENBUES1APID_p = &ie_p->value.choice.ENB_UE_S1AP_ID;
                    }

                    if(s1apENBUES1APID_p != NULL)
                    {
                        proto_ies->data[i].IE_type = S1AP_IE_ENB_UE_ID;
                        memcpy(&proto_ies->data[i].val.enb_ue_s1ap_id,
                               s1apENBUES1APID_p, sizeof(ENB_UE_S1AP_ID_t));
                    }
                    else
                    {
                        log_msg(LOG_ERROR,
                                "Decoding of IE eNB_UE_S1AP_ID failed");
                        return -1;
                    }
                }
                break;
                case ProtocolIE_ID_id_MME_UE_S1AP_ID:
                {
                    MME_UE_S1AP_ID_t *s1apMMEUES1APID_p = NULL;
                    if(E_RABReleaseResponseIEs__value_PR_MME_UE_S1AP_ID
                            == ie_p->value.present)
                    {
                        s1apMMEUES1APID_p = &ie_p->value.choice.MME_UE_S1AP_ID;
                    }

                    if(s1apMMEUES1APID_p != NULL)
                    {
                        proto_ies->data[i].IE_type = S1AP_IE_MME_UE_ID;
                        memcpy(&proto_ies->data[i].val.mme_ue_s1ap_id,
                               s1apMMEUES1APID_p, sizeof(MME_UE_S1AP_ID_t));
                    }
                    else
                    {
                        log_msg(LOG_ERROR,
                                "Decoding of IE MME_UE_S1AP_ID failed");
                        return -1;
                    }
                }
                break;
                case ProtocolIE_ID_id_E_RABReleaseListBearerRelComp:
                {
                    E_RABReleaseListBearerRelComp_t *e_RABReleaseList_p = NULL;
                    if(E_RABReleaseResponseIEs__value_PR_E_RABReleaseListBearerRelComp
                            == ie_p->value.present)
                    {
                        e_RABReleaseList_p =
                                &ie_p->value.choice.E_RABReleaseListBearerRelComp;
                    }

                    if(e_RABReleaseList_p != NULL)
                    {
                        proto_ies->data[i].IE_type =
                        S1AP_IE_E_RAB_RELEASE_LIST_BEARER_REL_COMP;
                        proto_ies->data[i].val.erab_releaselist.count =
                                e_RABReleaseList_p->list.count;

                        for (int j = 0; j < e_RABReleaseList_p->list.count; j++)
                        {
                            E_RABReleaseItemBearerRelCompIEs_t *ie_p;
                            ie_p =
                                    (E_RABReleaseItemBearerRelCompIEs_t*) e_RABReleaseList_p->list.array[j];
                            switch (ie_p->id)
                            {
                                case ProtocolIE_ID_id_E_RABReleaseItemBearerRelComp:
                                {
                                    E_RABReleaseItemBearerRelComp_t *eRabReleaseItem_p =
                                    NULL;
                                    if(E_RABReleaseItemBearerRelCompIEs__value_PR_E_RABReleaseItemBearerRelComp
                                            == ie_p->value.present)
                                    {
                                        eRabReleaseItem_p =
                                                &ie_p->value.choice.E_RABReleaseItemBearerRelComp;
                                    }

                                    if(eRabReleaseItem_p != NULL)
                                    {
                                        proto_ies->data[i].val.erab_releaselist.erab_id[j] =
                                                (uint8_t) eRabReleaseItem_p->e_RAB_ID;
                                    }
                                    else
                                    {
                                        log_msg(LOG_ERROR,
                                                "Decoding of IE E_RABReleaseItemBearerRelComp failed");
                                        return -1;
                                    }
                                }
                                    break;
                                default:
                                {
                                    log_msg(LOG_WARNING, "Unhandled List item %lu", ie_p->id);
                                }
                            }
                        }
                    }
                    else
                    {
                        log_msg(LOG_ERROR,
                                "Decoding of IE E_RABReleaseItemBearerRelComp failed");
                        return -1;
                    }
                }
                break;
                case ProtocolIE_ID_id_E_RABFailedToReleaseList:
                {
                    E_RABList_t *e_RABFailedToReleaseList_p = NULL;
                    if(E_RABReleaseResponseIEs__value_PR_E_RABList
                            == ie_p->value.present)
                    {
                        e_RABFailedToReleaseList_p =
                                &ie_p->value.choice.E_RABList;
                    }

                    if(e_RABFailedToReleaseList_p != NULL)
                    {
                        proto_ies->data[i].IE_type =
                        S1AP_IE_E_RAB_FAILED_TO_RELEASED_LIST;
                        proto_ies->data[i].val.erab_failed_to_release_list.count =
                                e_RABFailedToReleaseList_p->list.count;

                        for (int j = 0;
                                j < e_RABFailedToReleaseList_p->list.count; j++)
                        {
                            E_RABItemIEs_t *ie_p;
                            ie_p =
                                    (E_RABItemIEs_t*) e_RABFailedToReleaseList_p->list.array[j];
                            switch (ie_p->id)
                            {
                                case ProtocolIE_ID_id_E_RABItem:
                                {
                                    E_RABItem_t *eRabFailedToReleaseItem_p =
                                            NULL;
                                    if(E_RABItemIEs__value_PR_E_RABItem
                                            == ie_p->value.present)
                                    {
                                        eRabFailedToReleaseItem_p =
                                                &ie_p->value.choice.E_RABItem;
                                    }

                                    if(eRabFailedToReleaseItem_p != NULL)
                                    {
                                        proto_ies->data[i].val.erab_failed_to_release_list.erab_item[j].e_RAB_ID =
                                                (uint8_t) eRabFailedToReleaseItem_p->e_RAB_ID;

                                        Cause_t *s1apCause_p = &eRabFailedToReleaseItem_p->cause;
                                        if(s1apCause_p != NULL)
                                        {
                                            proto_ies->data[i].val.erab_failed_to_release_list.erab_item[j].cause.present =
                                                    s1apCause_p->present;
                                            switch (s1apCause_p->present)
                                            {
                                                case Cause_PR_radioNetwork:
                                                {
                                                    log_msg(LOG_DEBUG, "RadioNetwork case : %lu", s1apCause_p->choice.radioNetwork);
                                                    proto_ies->data[i].val.erab_failed_to_release_list.erab_item[j].cause.choice.radioNetwork =
                                                            s1apCause_p->choice.radioNetwork;
                                                }
                                                break;
                                                default:
                                                    log_msg(LOG_WARNING, "Unknown cause %d", s1apCause_p->present);
                                            }
                                        }
                                        else
                                        {
                                            log_msg(LOG_ERROR, "Decoding of IE eRabFailedToReleaseItem->Cause failed");
                                            return -1;
                                        }
                                    }
                                    else
                                    {
                                        log_msg(LOG_ERROR, "Decoding of IE eRabFailedToReleaseItem failed");
                                        return -1;
                                    }
                                }
                                    break;
                                default:
                                {
                                    log_msg(LOG_WARNING, "Unhandled List item %lu", ie_p->id);
                                }
                            }
                        }
                    }
                    else
                    {
                        log_msg(LOG_ERROR,
                                "Decoding of IE eRabFailedToReleaseItem_p failed");
                        return -1;
                    }
                }
                break;
                default:
                {
                    proto_ies->data[i].IE_type = ie_p->id;
                    log_msg(LOG_WARNING, "Unhandled IE %lu", ie_p->id);
                }
            }
        }
    }
    return 0;
}

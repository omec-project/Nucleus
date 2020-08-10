    
/*
 * Copyright 2020-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MME_SM_DEFS_H
#define MME_SM_DEFS_H

#include <stdint.h>
/**************************************
 * 
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/enum.tt>
 **************************************/

/******************************************
* MME-APP Specific States - ID
******************************************/ 	 	
     
const uint16_t attach_start = 1;     
const uint16_t attach_wf_aia = 2;     
const uint16_t attach_wf_att_cmp = 3;     
const uint16_t attach_wf_auth_resp = 4;     
const uint16_t attach_wf_auth_resp_validate = 5;     
const uint16_t attach_wf_cs_resp = 6;     
const uint16_t attach_wf_dns_resp = 7;     
const uint16_t attach_wf_esm_info_check = 8;     
const uint16_t attach_wf_esm_info_resp = 9;     
const uint16_t attach_wf_identity_response = 10;     
const uint16_t attach_wf_imsi_validate_action = 11;     
const uint16_t attach_wf_init_ctxt_resp = 12;     
const uint16_t attach_wf_init_ctxt_resp_att_cmp = 13;     
const uint16_t attach_wf_mb_resp = 14;     
const uint16_t attach_wf_sec_cmp = 15;     
const uint16_t attach_wf_ula = 16;     
const uint16_t default_mme_state = 17;     
const uint16_t detach_start = 18;     
const uint16_t detach_wf_del_session_resp = 19;     
const uint16_t detach_wf_purge_resp = 20;     
const uint16_t detach_wf_purge_resp_del_session_resp = 21;     
const uint16_t erab_mod_ind_start = 22;     
const uint16_t erab_mod_ind_wf_mb_resp = 23;     
const uint16_t intra_s1_ho_start = 24;     
const uint16_t ni_detach_start = 25;     
const uint16_t ni_detach_wf_del_sess_resp = 26;     
const uint16_t ni_detach_wf_det_accpt_del_sess_resp = 27;     
const uint16_t ni_detach_wf_detach_accept = 28;     
const uint16_t ni_detach_wf_s1_rel_comp = 29;     
const uint16_t paging_start = 30;     
const uint16_t paging_wf_service_req = 31;     
const uint16_t s1_ho_wf_ho_notify = 32;     
const uint16_t s1_ho_wf_ho_request_ack = 33;     
const uint16_t s1_ho_wf_modify_bearer_response = 34;     
const uint16_t s1_ho_wf_tau_check = 35;     
const uint16_t s1_ho_wf_tau_request = 36;     
const uint16_t s1_release_start = 37;     
const uint16_t s1_release_wf_release_access_bearer_resp = 38;     
const uint16_t s1_release_wf_ue_ctxt_release_comp = 39;     
const uint16_t service_request_start = 40;     
const uint16_t service_request_wf_aia = 41;     
const uint16_t service_request_wf_auth_resp_validate = 42;     
const uint16_t service_request_wf_auth_response = 43;     
const uint16_t service_request_wf_init_ctxt_resp = 44;     
const uint16_t service_request_wf_mb_resp = 45;     
const uint16_t service_request_wf_sec_cmp = 46;     
const uint16_t tau_start = 47;
const uint16_t END_STATE = 48;

/******************************************
* MME-APP Specific Events - ID
******************************************/

const uint16_t AIA_FROM_HSS = 101; 
const uint16_t ATT_CMP_FROM_UE = 102; 
const uint16_t ATTACH_REQ_FROM_UE = 103; 
const uint16_t AUTH_RESP_FAILURE = 104; 
const uint16_t AUTH_RESP_FROM_UE = 105; 
const uint16_t AUTH_RESP_SUCCESS = 106; 
const uint16_t AUTH_RESP_SYNC_FAILURE = 107; 
const uint16_t CLR_FROM_HSS = 108; 
const uint16_t CS_RESP_FROM_SGW = 109; 
const uint16_t DDN_FROM_SGW = 110; 
const uint16_t DEL_SESSION_RESP_FROM_SGW = 111; 
const uint16_t DETACH_ACCEPT_FROM_UE = 112; 
const uint16_t DETACH_REQ_FROM_UE = 113; 
const uint16_t DNS_RESPONSE = 114; 
const uint16_t ENB_STATUS_TRANFER_FROM_SRC_ENB = 115; 
const uint16_t eRAB_MOD_IND_START = 116; 
const uint16_t ERAB_MOD_INDICATION_FROM_ENB = 117; 
const uint16_t ESM_INFO_NOT_REQUIRED = 118; 
const uint16_t ESM_INFO_REQUIRED = 119; 
const uint16_t ESM_INFO_RESP_FROM_UE = 120; 
const uint16_t HO_CANCEL_REQ_FROM_SRC_ENB = 121; 
const uint16_t HO_FAILURE_FROM_TARGET_ENB = 122; 
const uint16_t HO_NOTIFY_FROM_ENB = 123; 
const uint16_t HO_REQUEST_ACK_FROM_ENB = 124; 
const uint16_t HO_REQUIRED_FROM_ENB = 125; 
const uint16_t IDENTITY_RESPONSE_FROM_UE = 126; 
const uint16_t IMSI_VALIDATION_FAILURE = 127; 
const uint16_t IMSI_VALIDATION_SUCCESS = 128; 
const uint16_t INIT_CTXT_RESP_FROM_UE = 129; 
const uint16_t INTRA_S1HO_START = 130; 
const uint16_t MB_RESP_FROM_SGW = 131; 
const uint16_t MME_INIT_DETACH = 132; 
const uint16_t PURGE_RESP_FROM_HSS = 133; 
const uint16_t REL_AB_RESP_FROM_SGW = 134; 
const uint16_t S1_REL_REQ_FROM_UE = 135; 
const uint16_t SEC_MODE_RESP_FROM_UE = 136; 
const uint16_t SERVICE_REQUEST_FROM_UE = 137; 
const uint16_t TAU_NOT_REQUIRED = 138; 
const uint16_t TAU_REQUEST_FROM_UE = 139; 
const uint16_t TAU_REQUIRED = 140; 
const uint16_t UE_CTXT_REL_COMP_FROM_ENB = 141; 
const uint16_t ULA_FROM_HSS = 142; 
const uint16_t VALIDATE_IMSI = 143; 
const uint16_t END_EVENT = 144;    

/******************************************
* Maps Event Name to Event ID
******************************************/
void populateEventStringMap();

/******************************************
* Maps State Name to State ID
******************************************/
void populateStateStringMap();

#endif

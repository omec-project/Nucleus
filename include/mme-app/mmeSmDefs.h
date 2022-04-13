    
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
const uint16_t attach_state = 2;     
const uint16_t attach_wf_aia = 3;     
const uint16_t attach_wf_att_cmp = 4;     
const uint16_t attach_wf_auth_resp = 5;     
const uint16_t attach_wf_auth_resp_validate = 6;     
const uint16_t attach_wf_cs_resp = 7;     
const uint16_t attach_wf_esm_info_check = 8;     
const uint16_t attach_wf_esm_info_resp = 9;     
const uint16_t attach_wf_identity_response = 10;     
const uint16_t attach_wf_imsi_validate_action = 11;     
const uint16_t attach_wf_init_ctxt_resp = 12;     
const uint16_t attach_wf_init_ctxt_resp_att_cmp = 13;     
const uint16_t attach_wf_mb_resp = 14;     
const uint16_t attach_wf_sec_cmp = 15;     
const uint16_t attach_wf_ula = 16;     
const uint16_t create_bearer_start = 17;     
const uint16_t create_bearer_wf_ded_act_complete = 18;     
const uint16_t create_bearer_wf_paging_complete = 19;     
const uint16_t ded_act_start = 20;     
const uint16_t ded_act_wf_bearer_and_session_setup = 21;     
const uint16_t ded_act_wf_bearer_setup = 22;     
const uint16_t ded_act_wf_session_setup = 23;     
const uint16_t ded_deact_start = 24;     
const uint16_t ded_deact_wf_bearer_and_session_tearup = 25;     
const uint16_t ded_deact_wf_bearer_tearup = 26;     
const uint16_t ded_deact_wf_session_tearup = 27;     
const uint16_t default_mme_state = 28;     
const uint16_t delete_bearer_start = 29;     
const uint16_t delete_bearer_wf_deact_complete = 30;     
const uint16_t delete_bearer_wf_paging_complete = 31;     
const uint16_t detach_start = 32;     
const uint16_t detach_wf_del_session_resp = 33;     
const uint16_t detach_wf_purge_resp = 34;     
const uint16_t detach_wf_purge_resp_del_session_resp = 35;     
const uint16_t erab_mod_ind_start = 36;     
const uint16_t erab_mod_ind_wf_mb_resp = 37;     
const uint16_t intra_s1_ho_start = 38;     
const uint16_t ni_detach_start = 39;     
const uint16_t ni_detach_state = 40;     
const uint16_t ni_detach_wf_del_sess_resp = 41;     
const uint16_t ni_detach_wf_det_accpt_del_sess_resp = 42;     
const uint16_t ni_detach_wf_detach_accept = 43;     
const uint16_t ni_detach_wf_s1_rel_comp = 44;     
const uint16_t paging_start = 45;     
const uint16_t paging_wf_service_req = 46;     
const uint16_t s1_ho_wf_ho_notify = 47;     
const uint16_t s1_ho_wf_ho_request_ack = 48;     
const uint16_t s1_ho_wf_modify_bearer_response = 49;     
const uint16_t s1_ho_wf_tau_check = 50;     
const uint16_t s1_ho_wf_tau_request = 51;     
const uint16_t s1_release_start = 52;     
const uint16_t s1_release_wf_release_access_bearer_resp = 53;     
const uint16_t s1_release_wf_ue_ctxt_release_comp = 54;     
const uint16_t service_request_start = 55;     
const uint16_t service_request_state = 56;     
const uint16_t service_request_wf_aia = 57;     
const uint16_t service_request_wf_auth_resp_validate = 58;     
const uint16_t service_request_wf_auth_response = 59;     
const uint16_t service_request_wf_init_ctxt_resp = 60;     
const uint16_t service_request_wf_mac_validate_action = 61;     
const uint16_t service_request_wf_mb_resp = 62;     
const uint16_t service_request_wf_sec_cmp = 63;     
const uint16_t tau_start = 64;     
const uint16_t x2_ho_mm_start = 65;     
const uint16_t x2_ho_mm_wf_sm_complete = 66;     
const uint16_t x2_ho_mm_wf_tau_check = 67;     
const uint16_t x2_ho_mm_wf_tau_req = 68;     
const uint16_t x2_ho_sm_start = 69;     
const uint16_t x2_ho_sm_wf_ho_complete = 70;     
const uint16_t x2_ho_sm_wf_mb_resp = 71;
const uint16_t END_STATE = 72;

/******************************************
* MME-APP Specific Events - ID
******************************************/

const uint16_t ACT_DED_BEARER_ACCEPT_FROM_UE = 101; 
const uint16_t ACT_DED_BEARER_REJECT_FROM_UE = 102; 
const uint16_t AIA_FROM_HSS = 103; 
const uint16_t ATT_CMP_FROM_UE = 104; 
const uint16_t ATTACH_REQ_FROM_UE = 105; 
const uint16_t AUTH_RESP_FAILURE = 106; 
const uint16_t AUTH_RESP_FROM_UE = 107; 
const uint16_t AUTH_RESP_SUCCESS = 108; 
const uint16_t AUTH_RESP_SYNC_FAILURE = 109; 
const uint16_t CLR_FROM_HSS = 110; 
const uint16_t CREATE_BEARER_REQ_FROM_GW = 111; 
const uint16_t CREATE_BEARER_START = 112; 
const uint16_t CS_RESP_FROM_SGW = 113; 
const uint16_t DDN_FROM_SGW = 114; 
const uint16_t DEACT_DED_BEARER_ACCEPT_FROM_UE = 115; 
const uint16_t DED_ACT_COMPLETE = 116; 
const uint16_t DED_BEARER_DEACT_START = 117; 
const uint16_t DED_DEACT_COMPLETE = 118; 
const uint16_t DEL_SESSION_RESP_FROM_SGW = 119; 
const uint16_t DELETE_BEARER_REQ_FROM_GW = 120; 
const uint16_t DETACH_ACCEPT_FROM_UE = 121; 
const uint16_t DETACH_COMPLETE = 122; 
const uint16_t DETACH_FAILURE = 123; 
const uint16_t DETACH_REQ_FROM_UE = 124; 
const uint16_t ENB_STATUS_TRANFER_FROM_SRC_ENB = 125; 
const uint16_t eRAB_MOD_IND_START = 126; 
const uint16_t ERAB_MOD_INDICATION_FROM_ENB = 127; 
const uint16_t ERAB_REL_RESP_FROM_ENB = 128; 
const uint16_t ERAB_SETUP_RESP_FROM_ENB = 129; 
const uint16_t ESM_INFO_NOT_REQUIRED = 130; 
const uint16_t ESM_INFO_REQUIRED = 131; 
const uint16_t ESM_INFO_RESP_FROM_UE = 132; 
const uint16_t GW_CP_REQ_INIT_PAGING = 133; 
const uint16_t GW_INIT_DED_BEARER_AND_SESSION_SETUP = 134; 
const uint16_t HO_CANCEL_REQ_FROM_SRC_ENB = 135; 
const uint16_t HO_FAILURE_FROM_TARGET_ENB = 136; 
const uint16_t HO_NOTIFY_FROM_ENB = 137; 
const uint16_t HO_REQUEST_ACK_FROM_ENB = 138; 
const uint16_t HO_REQUIRED_FROM_ENB = 139; 
const uint16_t IDENTITY_RESPONSE_FROM_UE = 140; 
const uint16_t IMSI_VALIDATION_FAILURE = 141; 
const uint16_t IMSI_VALIDATION_SUCCESS = 142; 
const uint16_t INIT_CTXT_RESP_FROM_UE = 143; 
const uint16_t INTRA_S1HO_START = 144; 
const uint16_t MB_RESP_FROM_SGW = 145; 
const uint16_t MME_INIT_DETACH = 146; 
const uint16_t NAS_PDU_PARSE_FAILURE = 147; 
const uint16_t PAGING_COMPLETE = 148; 
const uint16_t PAGING_FAILURE = 149; 
const uint16_t PATH_SWITCH_REQ_FROM_ENB = 150; 
const uint16_t PGW_INIT_DETACH = 151; 
const uint16_t PURGE_RESP_FROM_HSS = 152; 
const uint16_t REL_AB_RESP_FROM_SGW = 153; 
const uint16_t S1_REL_REQ_FROM_UE = 154; 
const uint16_t SEC_MODE_RESP_FROM_UE = 155; 
const uint16_t SERVICE_REQUEST_FROM_UE = 156; 
const uint16_t SERVICE_REQUEST_VALIDATE_MAC = 157; 
const uint16_t SERVICE_REQUEST_VALIDATE_MAC_FAILURE = 158; 
const uint16_t SERVICE_REQUEST_VALIDATE_MAC_SUCCESS = 159; 
const uint16_t START_DED_DEACTIVATION = 160; 
const uint16_t START_UE_DETACH = 161; 
const uint16_t START_X2_HO = 162; 
const uint16_t START_X2_HO_SM = 163; 
const uint16_t TAU_NOT_REQUIRED = 164; 
const uint16_t TAU_REQUEST_FROM_UE = 165; 
const uint16_t TAU_REQUIRED = 166; 
const uint16_t UE_CTXT_REL_COMP_FROM_ENB = 167; 
const uint16_t ULA_FROM_HSS = 168; 
const uint16_t VALIDATE_IMSI = 169; 
const uint16_t X2_HO_COMPLETE = 170; 
const uint16_t X2_HO_SM_COMPLETE = 171; 
const uint16_t X2_HO_SM_FAILURE = 172; 
const uint16_t END_EVENT = 173;    

/******************************************
* Maps Event Name to Event ID
******************************************/
void populateEventStringMap();

#endif

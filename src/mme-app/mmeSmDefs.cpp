
/*
 * Copyright 2019-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <mmeSmDefs.h>
#include <smTypes.h>
#include <string>
/**************************************
 * 
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/mmeSmDefs.cpp.tt>
 **************************************/
using namespace std;
using namespace SM;

SM::SmUtility* smUtil = SM::SmUtility::Instance();

void populateEventStringMap()
{
    smUtil->addEventToStrEntry(ABORT_EVENT,"ABORT_EVENT");      
    smUtil->addEventToStrEntry(ACT_DED_BEARER_ACCEPT_FROM_UE,"ACT_DED_BEARER_ACCEPT_FROM_UE");      
    smUtil->addEventToStrEntry(ACT_DED_BEARER_REJECT_FROM_UE,"ACT_DED_BEARER_REJECT_FROM_UE");      
    smUtil->addEventToStrEntry(AIA_FROM_HSS,"AIA_FROM_HSS");      
    smUtil->addEventToStrEntry(ATT_CMP_FROM_UE,"ATT_CMP_FROM_UE");      
    smUtil->addEventToStrEntry(ATTACH_REQ_FROM_UE,"ATTACH_REQ_FROM_UE");      
    smUtil->addEventToStrEntry(AUTH_RESP_FAILURE,"AUTH_RESP_FAILURE");      
    smUtil->addEventToStrEntry(AUTH_RESP_FROM_UE,"AUTH_RESP_FROM_UE");      
    smUtil->addEventToStrEntry(AUTH_RESP_SUCCESS,"AUTH_RESP_SUCCESS");      
    smUtil->addEventToStrEntry(AUTH_RESP_SYNC_FAILURE,"AUTH_RESP_SYNC_FAILURE");      
    smUtil->addEventToStrEntry(CLR_FROM_HSS,"CLR_FROM_HSS");      
    smUtil->addEventToStrEntry(CREATE_BEARER_REQ_FROM_GW,"CREATE_BEARER_REQ_FROM_GW");      
    smUtil->addEventToStrEntry(CREATE_BEARER_START,"CREATE_BEARER_START");      
    smUtil->addEventToStrEntry(CS_RESP_FROM_SGW,"CS_RESP_FROM_SGW");      
    smUtil->addEventToStrEntry(DDN_FROM_SGW,"DDN_FROM_SGW");      
    smUtil->addEventToStrEntry(DEACT_DED_BEARER_ACCEPT_FROM_UE,"DEACT_DED_BEARER_ACCEPT_FROM_UE");      
    smUtil->addEventToStrEntry(DED_ACT_COMPLETE,"DED_ACT_COMPLETE");      
    smUtil->addEventToStrEntry(DED_BEARER_DEACT_START,"DED_BEARER_DEACT_START");      
    smUtil->addEventToStrEntry(DED_DEACT_COMPLETE,"DED_DEACT_COMPLETE");      
    smUtil->addEventToStrEntry(DEFAULT_EVENT,"DEFAULT_EVENT");      
    smUtil->addEventToStrEntry(DEL_SESSION_RESP_FROM_SGW,"DEL_SESSION_RESP_FROM_SGW");      
    smUtil->addEventToStrEntry(DELETE_BEARER_REQ_FROM_GW,"DELETE_BEARER_REQ_FROM_GW");      
    smUtil->addEventToStrEntry(DETACH_ACCEPT_FROM_UE,"DETACH_ACCEPT_FROM_UE");      
    smUtil->addEventToStrEntry(DETACH_COMPLETE,"DETACH_COMPLETE");      
    smUtil->addEventToStrEntry(DETACH_FAILURE,"DETACH_FAILURE");      
    smUtil->addEventToStrEntry(DETACH_REQ_FROM_UE,"DETACH_REQ_FROM_UE");      
    smUtil->addEventToStrEntry(ENB_STATUS_TRANFER_FROM_SRC_ENB,"ENB_STATUS_TRANFER_FROM_SRC_ENB");      
    smUtil->addEventToStrEntry(ENB_STATUS_TRANSFER_RECV_FROM_ENB,"ENB_STATUS_TRANSFER_RECV_FROM_ENB");      
    smUtil->addEventToStrEntry(eRAB_MOD_IND_START,"eRAB_MOD_IND_START");      
    smUtil->addEventToStrEntry(ERAB_MOD_INDICATION_FROM_ENB,"ERAB_MOD_INDICATION_FROM_ENB");      
    smUtil->addEventToStrEntry(ERAB_REL_RESP_FROM_ENB,"ERAB_REL_RESP_FROM_ENB");      
    smUtil->addEventToStrEntry(ERAB_SETUP_RESP_FROM_ENB,"ERAB_SETUP_RESP_FROM_ENB");      
    smUtil->addEventToStrEntry(ESM_INFO_NOT_REQUIRED,"ESM_INFO_NOT_REQUIRED");      
    smUtil->addEventToStrEntry(ESM_INFO_REQUIRED,"ESM_INFO_REQUIRED");      
    smUtil->addEventToStrEntry(ESM_INFO_RESP_FROM_UE,"ESM_INFO_RESP_FROM_UE");      
    smUtil->addEventToStrEntry(FR_REQ_FROM_SRC_MME,"FR_REQ_FROM_SRC_MME");      
    smUtil->addEventToStrEntry(FWD_ACC_CTXT_ACK_RCVD,"FWD_ACC_CTXT_ACK_RCVD");      
    smUtil->addEventToStrEntry(FWD_REL_COMP_NOTIFY_RCVD_FROM_TGT_MME,"FWD_REL_COMP_NOTIFY_RCVD_FROM_TGT_MME");      
    smUtil->addEventToStrEntry(FWD_RELOCATION_RES_RCVD,"FWD_RELOCATION_RES_RCVD");      
    smUtil->addEventToStrEntry(GW_CP_REQ_INIT_PAGING,"GW_CP_REQ_INIT_PAGING");      
    smUtil->addEventToStrEntry(GW_INIT_DED_BEARER_AND_SESSION_SETUP,"GW_INIT_DED_BEARER_AND_SESSION_SETUP");      
    smUtil->addEventToStrEntry(HO_CANCEL_REQ_FROM_SRC_ENB,"HO_CANCEL_REQ_FROM_SRC_ENB");      
    smUtil->addEventToStrEntry(HO_FAILURE_FROM_TARGET_ENB,"HO_FAILURE_FROM_TARGET_ENB");      
    smUtil->addEventToStrEntry(HO_FWD_ACC_CNTX_NOTI,"HO_FWD_ACC_CNTX_NOTI");      
    smUtil->addEventToStrEntry(HO_NOTIFY_FROM_ENB,"HO_NOTIFY_FROM_ENB");      
    smUtil->addEventToStrEntry(HO_REQ_ACK_FROM_TARGET_ENB,"HO_REQ_ACK_FROM_TARGET_ENB");      
    smUtil->addEventToStrEntry(HO_REQ_TO_TARGET_ENB,"HO_REQ_TO_TARGET_ENB");      
    smUtil->addEventToStrEntry(HO_REQUEST_ACK_FROM_ENB,"HO_REQUEST_ACK_FROM_ENB");      
    smUtil->addEventToStrEntry(HO_REQUIRED_FROM_ENB,"HO_REQUIRED_FROM_ENB");      
    smUtil->addEventToStrEntry(IDENTIFICATION_RESPONSE_FROM_MME,"IDENTIFICATION_RESPONSE_FROM_MME");      
    smUtil->addEventToStrEntry(IDENTITY_RESPONSE_FROM_UE,"IDENTITY_RESPONSE_FROM_UE");      
    smUtil->addEventToStrEntry(IMSI_VALIDATION_FAILURE,"IMSI_VALIDATION_FAILURE");      
    smUtil->addEventToStrEntry(IMSI_VALIDATION_SUCCESS,"IMSI_VALIDATION_SUCCESS");      
    smUtil->addEventToStrEntry(INIT_CTXT_RESP_FROM_UE,"INIT_CTXT_RESP_FROM_UE");      
    smUtil->addEventToStrEntry(INTER_S1HO_START,"INTER_S1HO_START");      
    smUtil->addEventToStrEntry(INTRA_S1HO_START,"INTRA_S1HO_START");      
    smUtil->addEventToStrEntry(MB_RESP_FROM_SGW,"MB_RESP_FROM_SGW");      
    smUtil->addEventToStrEntry(MME_INIT_DETACH,"MME_INIT_DETACH");      
    smUtil->addEventToStrEntry(OVERLOAD_START,"OVERLOAD_START");      
    smUtil->addEventToStrEntry(OVERLOAD_STOP,"OVERLOAD_STOP");      
    smUtil->addEventToStrEntry(PAGING_COMPLETE,"PAGING_COMPLETE");      
    smUtil->addEventToStrEntry(PAGING_FAILURE,"PAGING_FAILURE");      
    smUtil->addEventToStrEntry(PGW_INIT_DETACH,"PGW_INIT_DETACH");      
    smUtil->addEventToStrEntry(PURGE_RESP_FROM_HSS,"PURGE_RESP_FROM_HSS");      
    smUtil->addEventToStrEntry(REL_AB_RESP_FROM_SGW,"REL_AB_RESP_FROM_SGW");      
    smUtil->addEventToStrEntry(S1_HO_NOTIFY_FROM_TARGET_ENB,"S1_HO_NOTIFY_FROM_TARGET_ENB");      
    smUtil->addEventToStrEntry(S1_REL_REQ_FROM_UE,"S1_REL_REQ_FROM_UE");      
    smUtil->addEventToStrEntry(SEC_MODE_RESP_FROM_UE,"SEC_MODE_RESP_FROM_UE");      
    smUtil->addEventToStrEntry(SERVICE_REQUEST_FROM_UE,"SERVICE_REQUEST_FROM_UE");      
    smUtil->addEventToStrEntry(START_DED_DEACTIVATION,"START_DED_DEACTIVATION");      
    smUtil->addEventToStrEntry(START_UE_DETACH,"START_UE_DETACH");      
    smUtil->addEventToStrEntry(STATE_GUARD_TIMEOUT,"STATE_GUARD_TIMEOUT");      
    smUtil->addEventToStrEntry(TAU_NOT_REQUIRED,"TAU_NOT_REQUIRED");      
    smUtil->addEventToStrEntry(TAU_REQUEST_FROM_UE,"TAU_REQUEST_FROM_UE");      
    smUtil->addEventToStrEntry(TAU_REQUIRED,"TAU_REQUIRED");      
    smUtil->addEventToStrEntry(UE_CTXT_REL_COMP_FROM_ENB,"UE_CTXT_REL_COMP_FROM_ENB");      
    smUtil->addEventToStrEntry(ULA_FROM_HSS,"ULA_FROM_HSS");      
    smUtil->addEventToStrEntry(VALIDATE_IMSI,"VALIDATE_IMSI");      
}

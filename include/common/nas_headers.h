/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __NAS_HEADERS_H_
#define __NAS_HEADERS_H_
#ifdef __cplusplus
extern "C"{
#endif


/****S1AP Procedude codes****/
#define S1AP_SETUP_REQUEST_CODE 17
#define S1AP_INITIAL_UE_MSG_CODE 12
#define S1AP_UE_CONTEXT_RELEASE_REQUEST_CODE 18
#define S1AP_UE_CONTEXT_RELEASE_CODE 23
/*uplink NAS Transport*/
#define S1AP_UL_NAS_TX_MSG_CODE 13
#define S1AP_INITIAL_CTX_RESP_CODE 9

/*S1AP Protocol IE types*/
#define S1AP_IE_GLOBAL_ENB_ID 59
#define S1AP_IE_ENB_NAME 60
#define S1AP_IE_SUPPORTED_TAS 64
#define S1AP_IE_DEF_PAGING_DRX 137
#define S1AP_IE_MMENAME 61
#define S1AP_IE_SERVED_GUMMEIES 105
#define S1AP_IE_REL_MME_CAPACITY 87

#define S1AP_IE_MME_UE_ID 0
#define S1AP_IE_CAUSE 2
#define S1AP_IE_ENB_UE_ID 8
#define S1AP_IE_NAS_PDU  26
#define S1AP_IE_TAI  67
#define S1AP_IE_UTRAN_CGI  100
#define S1AP_IE_S_TMSI  96
#define S1AP_IE_RRC_EST_CAUSE  134
#define S1AP_ERAB_SETUP_CTX_SUR 51

/* should be part of mme app only */
/*NAS message type codes*/
#define NAS_ESM_RESP 0xda
#define NAS_AUTH_RESP 0x53
#define NAS_AUTH_REJECT 0x54
#define NAS_AUTH_FAILURE 0x5c
#define NAS_IDENTITY_REQUEST 0x55
#define NAS_IDENTITY_RESPONSE 0x56
#define NAS_SEC_MODE_COMPLETE 0x5e
#define NAS_SEC_MODE_REJECT  0x5f
#define NAS_ATTACH_REQUEST 0x41
#define NAS_ATTACH_COMPLETE 0x43
#define NAS_ATTACH_REJECT 0x44
#define NAS_TAU_REQUEST    0x48
#define NAS_TAU_COMPLETE   0x4a
#define NAS_DETACH_REQUEST 0x45
#define NAS_DETACH_ACCEPT 0x46
#define NAS_SERVICE_REQUEST 0x4D

enum eps_nas_mesage_type {
	AttachAccept = 0x42,
	AttachReject = 0x44,
	DetachAccept = 0x46,
	DetachRequest = 0x45,
	TauAccept    = 0x49,
    TauReject    = 0x4b,
	ServiceReject = 0x4e,
	AuthenticationRequest = 0x52,
    IdentityRequest       = 0x55,
	SecurityModeCommand = 0x5d,
    EMMInformation = 0x61,
	ESMInformationRequest = 0xd9,
};

/*NAS MSG IE CODES */
/* Message content : 
   3gpp 24.301
   Table 8.2.4.1: IEI Column.*/
typedef enum
{
    NAS_IE_TYPE_EPS_MOBILE_ID_IMSI=0x1,
    NAS_IE_TYPE_UE_NETWORK_CAPABILITY=0x2,
    NAS_IE_TYPE_ESM_MSG=0x3,
    NAS_IE_TYPE_TMSI_STATUS=0x09,
    NAS_IE_TYPE_MS_NETWORK_FEATURE_SUPPORT=0x0C,
    NAS_IE_TYPE_GUTI_TYPE=0x0E,
    NAS_IE_TYPE_ADDITIONAL_UPDATE_TYPE=0xF,
    NAS_IE_TYPE_MS_CLASSMARK_2=0x11,
    NAS_IE_TYPE_LAI=0x13,
    NAS_IE_TYPE_PTMSI_SIGNATURE=0x19,
    NAS_IE_TYPE_MS_CLASSMARK_3=0x20,
    NAS_IE_TYPE_APN=0x28,
    NAS_IE_TYPE_AUTH_FAIL_PARAM=0x30,
    NAS_IE_TYPE_MS_NETWORK_CAPABILITY=0x31,
    NAS_IE_TYPE_DRX_PARAM=0x5C,
    NAS_IE_TYPE_TAI=0x52,
    NAS_IE_TYPE_EMM_CAUSE = 0x53,
    NAS_IE_TYPE_VOICE_DOMAIN_PREF_UE_USAGE_SETTING=0x5D,
    NAS_IE_TYPE_TX_FLAG=0xAA,
    NAS_IE_TYPE_PCO=0xAB,
    NAS_IE_TYPE_PTI=0xAC,
}nas_ie_type;

#ifdef __cplusplus
}
#endif
#endif /*__S1AP_MSG_CODES*/

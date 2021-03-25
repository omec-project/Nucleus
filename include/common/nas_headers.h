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
#define S1AP_INITIAL_UE_MSG_CODE 12
#define S1AP_SETUP_REQUEST_CODE 17
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

/* NAS_ESM_HEADER_LEN (9) =
 *     Sec_Hdr_Type(1/2) + Proto_Discr_1(1/2) +
 *     MAC (4) + Seq_no (1) +
 *     EPS_BEARER_ID(1/2) + Proto_Discr_2(1/2) +
 *     PTI(1) + NAS_ESM_MSG_TYPE (1) */
#define ESM_HEADER_LEN 9

#define NAS_EPS_MOBILE_ID_IMSI 0x01
#define NAS_EPS_MOBILE_ID_IMEI 0x03
#define NAS_EPS_MOBILE_ID_GUTI 0x06

/* NAS MESSAGE TYPES
 * Refer - 24.301 , Table - 9.8.1,9.8.2 */
enum eps_nas_mesage_type {
	AttachRequest = 0x41,
	AttachAccept = 0x42,
	AttachComplete = 0x43,
	AttachReject = 0x44,
	DetachRequest = 0x45,
	DetachAccept = 0x46,
	TauRequest = 0x48,
	TauAccept    = 0x49,
	TauComplete = 0x4a,
	TauReject    = 0x4b,
	ServiceRequest = 0x4d,
	ServiceReject = 0x4e,
	AuthenticationRequest = 0x52,
	AuthenticationResponse = 0x53,
	AuthenticationReject = 0x54,
	IdentityRequest = 0x55,
	IdentityResponse = 0x56,
	AuthenticationFailure = 0x5c,
	SecurityModeCommand = 0x5d,
	SecurityModeComplete = 0x5e,
	SecurityModeReject = 0x5f,
	EMMInformation = 0x61,
	ActivateDedicatedBearerContextRequest = 0xc5,
	ActivateDedicatedBearerContextAccept = 0xc6,
	ActivateDedicatedBearerContextReject = 0xc7,
	DeactivateEPSBearerContextRequest = 0xcd,
	DeactivateEPSBearerContextAccept = 0xce,
	ESMInformationRequest = 0xd9,
	ESMInformationResponse = 0xda,
};

/*NAS MSG IE CODES */
/* Message content : 
   3gpp 24.301
   Table 8.2.4.1: IEI Column.*/
typedef enum
{
    NAS_IE_TYPE_EPS_MOBILE_ID_IMSI=0x1,
    NAS_IE_TYPE_ESM_MSG=0x3,
    NAS_IE_TYPE_GPRS_CIPHERING_KEY_SEQUENCE_NUMBER = 0x80,
    NAS_IE_TYPE_TMSI_STATUS=0x90,
    NAS_IE_TYPE_UE_RADIO_CAPABILITY_INFORMATION_UPDATE_NEEDED = 0xA0,
    NAS_IE_TYPE_NON_CURRENT_NATIVE_NAS_KEY_SET_IDENTIFIER = 0xB0,
    NAS_IE_TYPE_MS_NETWORK_FEATURE_SUPPORT=0xC0,
    NAS_IE_TYPE_DEVICE_PROPERTIES = 0xD0,
    NAS_IE_TYPE_GUTI_TYPE=0xE0,
    NAS_IE_TYPE_ADDITIONAL_UPDATE_TYPE=0xF0,
    NAS_IE_TYPE_TMSI_BASED_NRI_CONTAINER = 0x10,
    NAS_IE_TYPE_MS_CLASSMARK_2=0x11,
    NAS_IE_TYPE_LAI=0x13,
    NAS_IE_TYPE_ADDITIONAL_INFORMATION_REQUESTED = 0x17,
    NAS_IE_TYPE_PTMSI_SIGNATURE=0x19,
    NAS_IE_TYPE_MS_CLASSMARK_3=0x20,
    NAS_IE_TYPE_PCO=0x27,
    NAS_IE_TYPE_APN=0x28,
    NAS_IE_TYPE_AUTH_FAIL_PARAM=0x30,
    NAS_IE_TYPE_MS_NETWORK_CAPABILITY=0x31,
    NAS_IE_TYPE_NBIFOM_CONTAINER = 0x33,
    NAS_IE_TYPE_SUPPORTED_CODECS = 0x40,
    NAS_IE_TYPE_ADDITIONAL_GUTI = 0x50,
    NAS_IE_TYPE_TAI=0x52,
    NAS_IE_TYPE_EMM_CAUSE = 0x53,
    NAS_IE_TYPE_NONCEUE = 0x55,
    NAS_IE_TYPE_EPS_BEARER_CONTEXT_STATUS = 0x57,
    NAS_IE_TYPE_ESM_CAUSE = 0x58,
    NAS_IE_TYPE_UE_NETWORK_CAPABILITY=0x58,
    NAS_IE_TYPE_DRX_PARAM=0x5C,
    NAS_IE_TYPE_VOICE_DOMAIN_PREF_UE_USAGE_SETTING=0x5D,
    NAS_IE_TYPE_T3412_EXTENDED_VALUE = 0x5E,
    NAS_IE_TYPE_EXTENDED_APN_AMBR = 0x5F,
    NAS_IE_TYPE_EPS_NETWORK_FEATURE_SUPPORT = 0X64,
    NAS_IE_TYPE_T3324_VALUE = 0x6A,
    NAS_IE_TYPE_UE_STATUS = 0x6D,
    NAS_IE_TYPE_EXTENDED_DRX_PARAMETERS = 0x6E,
    NAS_IE_TYPE_UE_ADDITIONAL_SECURITY_CAPABILITY=0x6F,
    NAS_IE_TYPE_EXT_PCO = 0x7B,
    NAS_IE_TYPE_TX_FLAG=0xAA,
    NAS_IE_TYPE_PTI=0xAC,
}nas_ie_type;

#ifdef __cplusplus
}
#endif
#endif /*__S1AP_MSG_CODES*/

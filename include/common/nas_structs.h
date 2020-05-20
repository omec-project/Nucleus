
/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __NAS_STRUCTS_H_
#define __NAS_STRUCTS_H_
#include <stdint.h>
#include <stdbool.h>
#include "ProcedureCode.h"
#include "nas_headers.h"

#define AUTH_REQ_NO_OF_NAS_IES 2
#define SEC_MODE_NO_OF_NAS_IES 1
#define ICS_REQ_NO_OF_NAS_IES  5
#define TAU_RSP_NO_OF_NAS_IES  2
#define SEC_DIRECTION_UPLINK   0
#define SEC_DIRECTION_DOWNLINK 1

/* ESM messages */
#define ESM_MSG_ACTV_DEF_BEAR__CTX_REQ 0xc1

#define AUTH_SYNC_FAILURE 21

#include "sec.h"
#include "Criticality.h"
#include "S1AP-PDU.h"

#define MAX_NAS_MSG_SIZE 500

/*TODO: Needed fro paging. Ignoring for now*/
struct proto_conf {
	int placeholder;
};
struct esm_sec_info {
	struct proto_conf proto_config;
};

#define MAX_PCO_OPTION_SIZE 255
struct pco 
{
    unsigned char pco_length;
    unsigned char pco_options[MAX_PCO_OPTION_SIZE];
};

typedef struct pdn_address {
	uint8_t spare :5;
	uint8_t type :3;
	uint32_t ipv4; /* TODO: Revisit 24.301 - 9.9.4.9.1 */
} pdn_address;

typedef struct linked_transcation_id {
	uint8_t flag :1;
	uint8_t val :7;
} linked_transcation_id;

enum protocol_discriminator{
	EPSSessionManagementMessage = 2,
    EPSMobilityManagementMessages = 7,
};
	

typedef struct esm_qos
{
	uint8_t reliability_class :3;
	uint8_t delay_class :3;
	uint8_t spare1 :2;
	uint8_t precedence_class :3;
	uint8_t spare2 :1;
	uint8_t peak_throughput :4;
	uint8_t mean_throughput :5;
	uint8_t spare3 :3;
	uint8_t delivery_err_sdu :3;
	uint8_t delivery_order :2;
	uint8_t traffic_class :3;
	uint8_t max_sdu_size;
	uint8_t mbr_ul;
	uint8_t mbr_dl;
	uint8_t sdu_err_ratio :4;
	uint8_t residual_ber :4;
	uint8_t trffic_prio :2;
	uint8_t transfer_delay :6;
	uint8_t gbr_ul;
	uint8_t gbr_dl;
	uint8_t src_stat_desc :4;
	uint8_t sig_ind :1;
	uint8_t spare4 :3;
	uint8_t mbr_dl_ext;
	uint8_t gbr_dl_ext;
	uint8_t mbr_ul_ext;
	uint8_t gbr_ul_ext;

} esm_qos;

/* TODO : Revisit 24.301 - 9.9.4.2.1 */
typedef struct ESM_APN_AMBR {
	uint8_t dl;
	uint8_t reserved;
	uint8_t dlext;
	uint8_t ulext;
	uint8_t dlext2;
	/* uint8_t dl_total; */
	uint8_t ulext2;
	/* uint8_t ul_total; */
} ESM_APN_AMBR;

#define NAS_MSG_UE_IE_GUTI  0x00000001
#define NAS_MSG_UE_IE_IMSI  0x00000002
#define UE_ID_IMSI(flags)   ((flags & NAS_MSG_UE_IE_IMSI) == NAS_MSG_UE_IE_IMSI)
#define UE_ID_GUTI(flags)   ((flags & NAS_MSG_UE_IE_GUTI) == NAS_MSG_UE_IE_GUTI)



#define AUTHREQ_NAS_SECURITY_PARAM 0x01
#define SERVICE_REQ_SECURITY_HEADER 12

#define BINARY_IMSI_LEN 8 /*same as packet capture. TODO: Write macros*/

/* NAS Security Encryption Algorithm */
typedef enum security_encryption_algo {
	Algo_EEA0 = 0,

}security_encryption_algo;

/* NAS Security Integrity Algorithm */
typedef enum security_integrity_algo {
	Algo_EIA0 = 0,
	Algo_128EIA1 = 1,
}security_integrity_algo;


/* s1apCauseMisc */
typedef long     s1apCauseMisc_t;
/* s1apCauseProtocol */
typedef long     s1apCauseProtocol_t;
/* s1apCauseNas */
typedef long     s1apCauseNas_t;
/* s1apCauseTransport */
typedef long     s1apCauseTransport_t;
/* s1apCauseRadioNetwork */
typedef long     s1apCauseRadioNetwork_t;


	
typedef enum emmCause {
    emmCause_ue_id_not_derived_by_network   = 9,
    emmCause_network_failure = 11
} e_emmCause;

//this should be nas specifi ?
typedef enum s1apCauseNas {
    s1apCauseNas_normal_release = 0,
    s1apCauseNas_authentication_failure = 1,
    s1apCauseNas_detach = 2,
    s1apCauseNas_unspecified    = 3,
    s1apCauseNas_csg_subscription_expiry    = 4
} e_s1apCauseNas;

/*36.413: 9.2.3.8 - MCC, MCN : Only 3 bytes are used*/
struct PLMN {
	unsigned char  idx[3];
};

typedef struct guti {
	uint8_t spare :4;
	uint8_t odd_even_indication :1;
	uint8_t id_type :3;
	struct PLMN plmn_id;
	uint16_t mme_grp_id;
	uint8_t mme_code;
	uint32_t m_TMSI;
} guti;


struct TAI {
	struct PLMN plmn_id;
	short tac; /*2 bytes. 36.413: 9.2.3.7*/
};

/*36.413 - 9.2.1.38*/
struct CGI {
	struct PLMN plmn_id;
	int cell_id;
};

typedef struct MS_net_capab {
    bool          pres;
	unsigned char element_id;
	unsigned char len;
	unsigned char capab[6];
}MS_net_capab;

struct UE_net_capab {
	unsigned char len;
	unsigned char capab[6];
};

/*24.008 - 10.5.6.1
APN name can be in range of min 3 octets to max 102 octets
*/
#define MAX_APN_LEN 102

struct apn_name {
	unsigned char len;
	unsigned char val[MAX_APN_LEN];/*TODO: Make dynamic as range is big 3-102*/
};

#define MAC_SIZE 4
#define SHORT_MAC_SIZE 2
typedef struct nas_pdu_header {
	unsigned char security_header_type:4;
	unsigned char proto_discriminator:4;
	unsigned char message_type;
	unsigned char security_encryption_algo:4;
	unsigned char security_integrity_algo:4;
	unsigned char nas_security_param;
	unsigned char mac[MAC_SIZE];
	unsigned char short_mac[SHORT_MAC_SIZE];
	unsigned char ksi;
	unsigned char seq_no;
	unsigned char eps_bearer_identity;
	unsigned char procedure_trans_identity;
	unsigned char detach_type;
} nas_pdu_header;


enum drx_params {
	PAGINX_DRX32,
	PAGINX_DRX64,
	PAGINX_DRX128,
	PAGINX_DRX256,
	PAGINX_DRX512,
};

#define EMM_MAX_TAI_LIST 16
typedef struct tai_list {
	uint8_t spare :1;
	uint8_t type :2;
	uint8_t num_of_elements :4;
	struct TAI partial_list[EMM_MAX_TAI_LIST];
} tai_list;


typedef struct esm_msg_container {
	uint8_t eps_bearer_id :4;
	uint8_t proto_discriminator :4;
	uint8_t procedure_trans_identity;
	uint8_t session_management_msgs;
	uint8_t eps_qos;  /* TODO: Revisit 24.301 - 9.9.4.3.1 */
	struct apn_name apn;
	struct apn_name selected_apn;
	pdn_address pdn_addr;
	linked_transcation_id linked_ti;
	esm_qos negotiated_qos;
	ESM_APN_AMBR apn_ambr;
    struct pco pco_opt;
} esm_msg_container;




#define NAS_RAND_SIZE 16
#define NAS_AUTN_SIZE 16
/**Information elements structs end**/
typedef union nas_pdu_elements_union {
	unsigned char rand[NAS_RAND_SIZE];
	unsigned char autn[NAS_AUTN_SIZE];
	unsigned char IMSI[BINARY_IMSI_LEN];
	unsigned char short_mac[SHORT_MAC_SIZE];
	struct esm_sec_info esm_info;
	enum drx_params drx;
	struct MS_net_capab ms_network;
	struct UE_net_capab ue_network;
	struct XRES   auth_resp; /*Authentication response*/
	struct AUTS   auth_fail_resp; /*Authentication response*/
	struct apn_name apn;
	unsigned char attach_res;
	unsigned char t3412;
	tai_list tailist;
	esm_msg_container esm_msg;
	guti mi_guti;
	bool esm_info_tx_required;
	unsigned char pti;
	unsigned char eps_res;
	unsigned char spare;
    struct pco pco_opt;
	unsigned char ue_id_type;
	unsigned char eps_update_result;
	unsigned char tau_timer;
}nas_pdu_elements_union;

typedef struct nas_pdu_elements {
   nas_ie_type msgType;
   nas_pdu_elements_union pduElement;
}nas_pdu_elements;

#define NAS_MSG_UE_IE_GUTI  0x00000001
#define NAS_MSG_UE_IE_IMSI  0x00000002
#define UE_ID_IMSI(flags)   ((flags & NAS_MSG_UE_IE_IMSI) == NAS_MSG_UE_IE_IMSI)
#define UE_ID_GUTI(flags)   ((flags & NAS_MSG_UE_IE_GUTI) == NAS_MSG_UE_IE_GUTI)
typedef struct nasPDU {
	nas_pdu_header header;
	unsigned char elements_len;
	nas_pdu_elements *elements;
  unsigned int flags;
	unsigned int dl_count;
} nasPDU;


/* NAS Security Header */
typedef enum security_header_type {
    Plain = 0,
    IntegrityProtected,
    IntegrityProtectedCiphered,
    IntegrityProtectedEPSSecCntxt,
}security_header_type;
	
typedef struct nas_pdu_header_sec {
        unsigned char security_header_type:4;
        unsigned char proto_discriminator:4;
        unsigned char mac[MAC_SIZE];
        unsigned char seq_no;
}nas_pdu_header_sec;
	
typedef struct nas_pdu_header_short {
        unsigned char security_header_type:4;
        unsigned char proto_discriminator:4;
        unsigned char message_type;
}nas_pdu_header_short;

typedef struct nas_pdu_header_long {
        unsigned char security_header_type:4;
        unsigned char proto_discriminator:4;
        unsigned char procedure_trans_identity;
        unsigned char message_type;
}nas_pdu_header_long;

enum ie_fail_internal_cause {
	AIA_FAIL,
	SECURITY_MOD_REJECT_FAIL,
	MAX_FAIL_CAUSE
};

#endif /*__S1AP_STRUCTS_H*/

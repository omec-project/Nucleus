
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

#define NAS_EXT_APN_AMBR_MIN 65281000  // 65.2 Gbps

/* ESM messages */
#define ESM_MSG_ACTV_DEF_BEAR__CTX_REQ 0xc1

#define AUTH_RESPONSE 53
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

/* Should we put this in NAS header ?*/
/* sec. 10.5.6.3 of 3GPP TS 24.008 has following statement:
 * The protocol configuration options is a type 4 information element with a minimum length of 3 octets and a maximum length of 253 octets.
 */
#define MAX_PCO_OPTION_SIZE 255
struct pco 
{
    uint8_t pco_length;
    uint8_t pco_options[MAX_PCO_OPTION_SIZE];
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
    emmCause_network_failure = 17,
    emmCause_mac_failure = 20
} e_emmCause;

typedef enum nasDetachType {
    invalidDetachType = 0,
    reattachRequired = 1,
    reattachNotRequired = 2,
    imsiDetach = 3
} e_nasDetachType;

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
    /*Start should always be idx. Dont move down */
    unsigned char  mnc_digits;
};

struct PLMN_C {
	uint16_t mcc;
	uint16_t mnc; 
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

/* UE Network capability */
/* Refer spec 24.301 v 15.6.0 sec:9.9.3.34*/
typedef struct ue_net_capab_bitmap {
    //Octet 3
    uint8_t eea7:1;
    uint8_t eea6:1;
    uint8_t eea5:1;
    uint8_t eea4:1;
    uint8_t eea3_128:1;
    uint8_t eea2_128:1;
    uint8_t eea1_128:1;
    uint8_t eea0:1;

    //Octet 4
    uint8_t eia7:1;
    uint8_t eia6:1;
    uint8_t eia5:1;
    uint8_t eia4:1;
    uint8_t eia3_128:1;
    uint8_t eia2_128:1;
    uint8_t eia1_128:1;
    uint8_t eia0:1;

    //Octet 5
    uint8_t uea7:1;
    uint8_t uea6:1;
    uint8_t uea5:1;
    uint8_t uea4:1;
    uint8_t uea3:1;
    uint8_t uea2:1;
    uint8_t uea1:1;
    uint8_t uea0:1;

    //Octet 6
    uint8_t uia7:1;
    uint8_t uia6:1;
    uint8_t uia5:1;
    uint8_t uia4:1;
    uint8_t uia3:1;
    uint8_t uia2:1;
    uint8_t uia1:1;
    uint8_t ucs2:1;

    //Octet 7
    uint8_t nf:1;
    uint8_t vcc_1xsr:1;
    uint8_t lcs:1;
    uint8_t lpp:1;
    uint8_t csfb_acc:1;
    uint8_t ash_h245:1;
    uint8_t proSe:1;
    uint8_t proSe_dd:1;

    //Octet 8
    uint8_t proSe_dc:1;
    uint8_t proSe_relay:1;
    uint8_t cp_ciot:1;
    uint8_t up_ciot:1;
    uint8_t s1u_data:1;
    uint8_t er_wo_pdn:1;
    uint8_t hc_cp_ciot:1;
    uint8_t epco:1;

    //Octet 9
    uint8_t multipleDRB:1;
    uint8_t v2xpc5:1;
    uint8_t restrictEC:1;
    uint8_t cpBackOff:1;
    uint8_t dcnr:1;
    uint8_t n1Mode:1;
    uint8_t sgc:1;
    uint8_t bearers_15:1;
} ue_net_capab_bitmap;

typedef struct UE_net_capab {
        uint8_t len;
        union ue_net_capabilities {
            ue_net_capab_bitmap bits;
            uint8_t octets[7];
        } u;
} UE_net_capab;

/* UE additional security capability*/
/* Refer spec 24.301 v 15.6.0 sec:9.9.3.53*/
typedef struct ue_add_sec_capabilities {
    //Octet 3
    uint8_t ea7_5g:1;
    uint8_t ea6_5g:1;
    uint8_t ea5_5g:1;
    uint8_t ea4_5g:1;
    uint8_t ea3_5g_128:1;
    uint8_t ea2_5g_128:1;
    uint8_t ea1_5g_128:1;
    uint8_t ea0_5g:1;

    //Octet 4
    uint8_t ea15_5g:1;
    uint8_t ea14_5g:1;
    uint8_t ea13_5g:1;
    uint8_t ea12_5g:1;
    uint8_t ea11_5g:1;
    uint8_t ea10_5g:1;
    uint8_t ea9_5g:1;
    uint8_t ea8_5g:1;

    //Octet 5
    uint8_t ia7_5g:1;
    uint8_t ia6_5g:1;
    uint8_t ia5_5g:1;
    uint8_t ia4_5g:1;
    uint8_t ia3_5g_128:1;
    uint8_t ia2_5g_128:1;
    uint8_t ia1_5g_128:1;
    uint8_t ia0_5g:1;

    //Octet 6
    uint8_t ia15_5g:1;
    uint8_t ia14_5g:1;
    uint8_t ia13_5g:1;
    uint8_t ia12_5g:1;
    uint8_t ia11_5g:1;
    uint8_t ia10_5g:1;
    uint8_t ia9_5g:1;
    uint8_t ia8_5g:1;

}ue_add_sec_capabilities;

/*EPS network feature support*/
/*Refer spec 24.301 v 15.6.0 sec:9.9.3.12A*/
typedef struct eps_network_feature_support {
    //Octet 3
    uint8_t ims_vops:1;
    uint8_t emc_bs:1;
    uint8_t epc_lcs:1;
    uint8_t cs_lcs:2;
    uint8_t esr_ps:1;
    uint8_t er_wo_pdn:1;
    uint8_t cp_ciot:1;

    //Octet 4
    uint8_t up_ciot:1;
    uint8_t s1u_data:1;
    uint8_t hc_cp_ciot:1;
    uint8_t epco:1;
    uint8_t restrictEc:1;
    uint8_t restrictDcnr:1;
    uint8_t iwkn26:1;
    uint8_t bearers15:1;
}eps_network_feature_support;

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
    unsigned char emm_cause;
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

/* 24.301 v15.6.0 - 9.9.4.3.1 */
typedef struct eps_qos_t {
        uint8_t len;
	uint8_t qci;
        uint8_t mbr_ul;
        uint8_t mbr_dl;
        uint8_t gbr_ul;
        uint8_t gbr_dl;
        uint8_t mbr_ul_ext;
        uint8_t mbr_dl_ext;
        uint8_t gbr_ul_ext;
        uint8_t gbr_dl_ext;
        uint8_t mbr_ul_ext_2;
        uint8_t mbr_dl_ext_2;
        uint8_t gbr_ul_ext_2;
        uint8_t gbr_dl_ext_2;
} eps_qos_t;

#define TFT_MAX_SIZE 256

typedef struct bearer_tft{
    uint16_t len;
    uint8_t data[TFT_MAX_SIZE];
} bearer_tft;


/* 24.301 v15.6.0 - 9.9.4.4 */
typedef enum esm_cause_t {
    NOT_SET = 0,
    OPERATOR_DETERMINED_BARRING = 4,
    INSUFFICIENT_RESOURCES = 26,
    MISSING_OR_UNKNOWN_APN = 27,
    UNKNOWN_PDN_TYPE = 28,
    USER_AUTHENTICATION_FAILED = 29,
    REQUEST_REJECTED_BY_SGW_OR_PGW = 30,
    REQUEST_REJECTED_UNSPECIFIED = 31,
    SERVICE_OPTION_NOT_SUPPORTED = 32,
    REQUESTED_SERVICE_OPTION_NOT_SUBSCRIBED = 33,
    SERVICE_OPTION_TEMPORARILY_OUT_OF_ORDER = 34,
    PTI_ALREADY_IN_USE = 35,
    REGULAR_DEACTIVATION = 36,
    EPS_QOS_NOT_ACCEPTED = 37,
    NETWORK_FAILURE = 38,
    REACTIVATION_REQUESTED = 39,
    SEMANTIC_ERROR_IN_THE_TFT_OPERATION = 41,
    SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION = 42,
    INVALID_EPS_BEARER_IDENTITY = 43,
    SEMANTIC_ERROR_IN_PACKET_FILTERS = 44,
    SYNTACTICAL_ERROR_IN_PACKET_FILTERS = 45,
    UNUSED = 46,
    PTI_MISMATCH = 47,
    LAST_PDN_CONNECTION_DISALLOWED = 49,
    PDN_TYPE_IPV4_ONLY_ALLOWED = 50,
    PDN_TYPE_IPV6_ONLY_ALLOWED = 51,
    SINGLE_ADDRESS_BEARERS_ONLY_ALLOWED = 52,
    ESM_INFORMATION_NOT_RECEIVED = 53,
    PDN_CONNECTION_DOES_NOT_EXIST = 54,
    MULTIPLE_PDN_CONNECTIONS_FOR_A_GIVEN_APN_NOT_ALLOWED = 55,
    COLLISION_WITH_NETWORK_INITIATED_REQUEST = 56,
    PDN_TYPE_IPV4V6_ONLY_ALLOWED = 57,
    PDN_TYPE_NON_IP_ONLY_ALLOWED = 58,
    UNSUPPORTED_QCI_VALUE = 59,
    BEARER_HANDLING_NOT_SUPPORTED = 60,
    MAXIMUM_NUMBER_OF_EPS_BEARERS_REACHED = 65,
    REQUESTED_APN_NOT_SUPPORTED_IN_GIVEN_RAT_AND_PLMN_COMBINATION = 66,
    INVALID_PTI_VALUE = 81,
    SEMANTICALLY_INCORRECT_MESSAGE = 95,
    INVALID_MANDATORY_INFORMATION = 96,
    MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 97,
    MESSAGE_TYPE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE = 98,
    INFORMATION_ELELMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED = 99,
    CONDITIONAL_IE_ERROR = 100,
    MESSAGE_NOT_COMPATIBLE_WITH_THE_PROTOCOL_STATE = 101,
    PROTOCOL_ERROR_UNSPECIFIED = 111,
    APN_RESTRICTION_VALUE_INCOMPATIBLE_WITH_ACTIVE_EPS_BEARER_CONTEXT = 112,
    MULTIPLE_ACCESSES_TO_A_PDN_CONNECTION_NOT_ALLOWED = 113,
} esm_cause_t;

typedef struct extended_apn_ambr {
    uint8_t length;
    uint8_t ext_apn_ambr[6];
} extended_apn_ambr;

typedef struct esm_msg_container {
	uint8_t eps_bearer_id :4;
	uint8_t proto_discriminator :4;
	uint8_t procedure_trans_identity;
	uint8_t session_management_msgs;
	esm_cause_t esm_cause;
	eps_qos_t eps_qos;  /* TODO: Revisit 24.301 - 9.9.4.3.1 */
	struct apn_name apn;
	struct apn_name selected_apn;
	pdn_address pdn_addr;
	linked_transcation_id linked_ti;
	esm_qos negotiated_qos;
	ESM_APN_AMBR apn_ambr;
	extended_apn_ambr extd_apn_ambr;
	struct pco pco_opt;
	bearer_tft tft;
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
	UE_net_capab ue_network;
	ue_add_sec_capabilities ue_add_sec_capab;
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
	eps_network_feature_support eps_nw_feature_supp;
}nas_pdu_elements_union;

typedef struct nas_pdu_elements {
   nas_ie_type msgType;
   nas_pdu_elements_union pduElement;
}nas_pdu_elements;

typedef struct nas_optional_ies_flags {
    bool ue_add_sec_cap_presence;
    bool eps_nw_feature_supp_presence;
}nas_optional_ies_flags;

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
	nas_optional_ies_flags opt_ies_flags;
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

typedef enum nas_ciph_algo
{
    NAS_CIPH_ALGORITHMS_EEA0=0,
    NAS_CIPH_ALGORITHMS_EEA1,
    NAS_CIPH_ALGORITHMS_EEA2,
    NAS_CIPH_ALGORITHMS_EEA3,
    NAS_CIPH_ALGORITHMS_EEA4,
    NAS_CIPH_ALGORITHMS_EEA5,
    NAS_CIPH_ALGORITHMS_EEA6,
    NAS_CIPH_ALGORITHMS_EEA7,
}nas_ciph_algo_enum;

typedef enum nas_int_algo
{
    NAS_INT_ALGORITHMS_EIA0=0,
    NAS_INT_ALGORITHMS_EIA1,
    NAS_INT_ALGORITHMS_EIA2,
    NAS_INT_ALGORITHMS_EIA3,
    NAS_INT_ALGORITHMS_EIA4,
    NAS_INT_ALGORITHMS_EIA5,
    NAS_INT_ALGORITHMS_EIA6,
    NAS_INT_ALGORITHMS_EIA7,
}nas_int_algo_enum;


#endif /*__S1AP_STRUCTS_H*/


/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __S1AP_STRUCTS_H_
#define __S1AP_STRUCTS_H_

#include <stdint.h>
#include <stdbool.h>
#include "nas_structs.h"
#include "sec.h"
#include "ProcedureCode.h"
#include "Criticality.h"
#include "S1AP-PDU.h"
#include "InitiatingMessage.h"

#define AUTH_REQ_NO_OF_IES 3
#define SEC_MODE_NO_OF_IES 3
#define ESM_REQ_NO_OF_IES 3
#define EMM_INFO_REQ_NO_OF_IES 3
#define ICS_REQ_NO_OF_IES 6
#define DTCH_ACCEPT_NO_OF_IES 3
#define UE_CTX_RELEASE_NO_OF_IES 3
#define NI_UE_CTX_RELEASE_NO_OF_IES 3
#define ATTACH_REJECT_NO_OF_IES 3
#define SERVICE_REJECT_NO_OF_IES 3 
#define ATTACH_ID_REQUEST_NO_OF_IES 3
#define TAU_RSP_NO_OF_IES 3
#define NI_DTCH_REQUEST_NO_OF_IES 3

#define MSISDN_STR_LEN 10
#define BCD_IMSI_STR_LEN 15
#define MME_NAME_STR_LEN 30

#define MAX_ERAB_SIZE 15
// ajaymerge -- defined at multiple places...need consistency 
#define BUFFER_SIZE 1000 /* S1AP packet max size */

typedef struct Buffer {
	unsigned char buf[BUFFER_SIZE];
	unsigned char pos;
}Buffer;


typedef struct ue_sec_capabilities {
	uint8_t eea1 :1;
	uint8_t eea2 :1;
	uint8_t eea3 :1;
	uint8_t eea4 :1;
	uint8_t eea5 :1;
	uint8_t eea6_128 :1;
	uint8_t eea7_128 :1;
	uint8_t eea8 :1;

	uint8_t eia1 :1;
	uint8_t eia2 :1;
	uint8_t eia3 :1;
	uint8_t eia4 :1;
	uint8_t eia5 :1;
	uint8_t eia6_128 :1;
	uint8_t eia8_128 :1;
	uint8_t eia8 :1;

	uint8_t uea1 :1;
	uint8_t uea2 :1;
	uint8_t uea3 :1;
	uint8_t uea4 :1;
	uint8_t uea5 :1;
	uint8_t uea6 :1;
	uint8_t uea7 :1;
	uint8_t uea8 :1;

	uint8_t uia1 :1;
	uint8_t uia2 :1;
	uint8_t uia3 :1;
	uint8_t uia4 :1;
	uint8_t uia5 :1;
	uint8_t uia6 :1;
	uint8_t uia7 :1;
	uint8_t spare1 :1;

	uint8_t gea1 :1;
	uint8_t gea2 :1;
	uint8_t gea3 :1;
	uint8_t gea4 :1;
	uint8_t gea5 :1;
	uint8_t gea6 :1;
	uint8_t gea7 :1;
	uint8_t spare2 :1;
} ue_sec_capabilities;

#pragma pack()

/****Information elements presentations **/
#define BINARY_IMSI_LEN 8 /*same as packet capture. TODO: Write macros*/
#define BCD_IMSI_STR_LEN 15
#define MME_NAME_STR_LEN 30

/*36.413: 9.1.8.4*/
#define ENB_NAME_SIZE 150
struct ie_enb_name {
	char enb_name[ENB_NAME_SIZE];
};

/*36.413: 9.2.1.37*/
#define MACRO_ENB_ID_SIZE 20
struct ie_global_enb_id {
	int plmn;
	char macro_enb_id[MACRO_ENB_ID_SIZE];
	/*TODO: make union of enb IDs*/
};


typedef struct erab_to_be_modified_item {
	uint8_t e_RAB_ID;
	uint32_t transportLayerAddress;
        uint32_t dl_gtp_teid;
} erab_to_be_modified_item;

typedef struct erab_to_be_modified_list {
	uint8_t count;
	erab_to_be_modified_item erab_to_be_mod_item[MAX_ERAB_SIZE];
} erab_to_be_modified_list;

typedef struct erab_modified_list {
	uint8_t count;
	uint8_t erab_id[MAX_ERAB_SIZE];
} erab_modified_list;
typedef struct ERABs_Subject_to_Forwarding {
	uint8_t e_RAB_ID;
	uint32_t dL_transportLayerAddress;
	uint32_t dL_gtp_teid;
} ERABs_Subject_to_Forwarding;

struct ERABs_Subject_to_Forwarding_List{
	int count;
	ERABs_Subject_to_Forwarding eRABs_Subject_to_Forwarding[10];
};
enum s1ap_cn_domain
{
    CN_DOMAIN_PS,
    CN_DOMAIN_CS,
    CN_DOMAIN_NONE
};

typedef enum handoverType {
        IntraLTE,
        LTEtoUTRAN,
        LTEtoGERAN,
        UTRANtoLTE,
        GERANtoLTE,
        LTEtoNR,
        NRtoLTE
}handoverType;

typedef struct ERAB_admitted{
        uint8_t e_RAB_ID;
        uint32_t transportLayerAddress;
        uint32_t gtp_teid;
        uint32_t dL_transportLayerAddress;
        uint32_t dL_gtp_teid;

}ERAB_admitted;

typedef struct targetId{
        struct ie_global_enb_id global_enb_id;
        struct TAI selected_tai;
}targetId;

#define TRANS_CONT_SIZE 512
struct src_target_transparent_container{
    int count;
    unsigned char buffer[TRANS_CONT_SIZE];
};

struct src_target_transparent_containerIE{
    int size;
    uint8_t* buffer_p;
};
struct count_t{
        int pdcp_sn;
        int hfn;
};

struct receive_status_of_ul_pdcp_sdu{
    int count;
    unsigned char buffer[255];
};

struct enB_status_transfer_transparent_container{
        unsigned short  eRAB_id;
        struct count_t ul_count_value;
        struct count_t dl_count_value;
        struct receive_status_of_ul_pdcp_sdu receive_status_of_ul_pdcp_sdu;
};

struct enB_status_transfer_transparent_container_list{
        int count;
        struct enB_status_transfer_transparent_container enB_status_transfer_transparent_container[10];
};

struct security_context{
        int next_hop_chaining_count ;
        uint8_t next_hop_nh[32];
};

struct gummei {
	struct PLMN plmn_id;
	uint16_t mme_grp_id;
	uint8_t mme_code;
};

#pragma pack()
/* Dependencies */
typedef enum s1apCause_PR {
    s1apCause_PR_NOTHING,   /* No components present */
    s1apCause_PR_radioNetwork,
    s1apCause_PR_transport,
    s1apCause_PR_nas,
    s1apCause_PR_protocol,
    s1apCause_PR_misc

} s1apCause_PR;

typedef enum s1apCause_PR_transporauseRadioNetwork {
    s1apCauseRadioNetwork_unspecified   = 0,
    s1apCauseRadioNetwork_tx2relocoverall_expiry    = 1,
    s1apCauseRadioNetwork_successful_handover   = 2,
    s1apCauseRadioNetwork_release_due_to_eutran_generated_reason    = 3,
    s1apCauseRadioNetwork_handover_cancelled    = 4,
    s1apCauseRadioNetwork_partial_handover  = 5,
    s1apCauseRadioNetwork_ho_failure_in_target_EPC_eNB_or_target_system = 6,
    s1apCauseRadioNetwork_ho_target_not_allowed = 7,
    s1apCauseRadioNetwork_tS1relocoverall_expiry    = 8,
    s1apCauseRadioNetwork_tS1relocprep_expiry   = 9,
    s1apCauseRadioNetwork_cell_not_available    = 10,
    s1apCauseRadioNetwork_unknown_targetID  = 11,
    s1apCauseRadioNetwork_no_radio_resources_available_in_target_cell   = 12,
    s1apCauseRadioNetwork_unknown_mme_ue_s1ap_id    = 13,
    s1apCauseRadioNetwork_unknown_enb_ue_s1ap_id    = 14,
    s1apCauseRadioNetwork_unknown_pair_ue_s1ap_id   = 15,
    s1apCauseRadioNetwork_handover_desirable_for_radio_reason   = 16,
    s1apCauseRadioNetwork_time_critical_handover    = 17,
    s1apCauseRadioNetwork_resource_optimisation_handover    = 18,
    s1apCauseRadioNetwork_reduce_load_in_serving_cell   = 19,
    s1apCauseRadioNetwork_user_inactivity   = 20,
    s1apCauseRadioNetwork_radio_connection_with_ue_lost = 21,
    s1apCauseRadioNetwork_load_balancing_tau_required   = 22,
    s1apCauseRadioNetwork_cs_fallback_triggered = 23,
    s1apCauseRadioNetwork_ue_not_available_for_ps_service   = 24,
    s1apCauseRadioNetwork_radio_resources_not_available = 25,
    s1apCauseRadioNetwork_failure_in_radio_interface_procedure  = 26,
    s1apCauseRadioNetwork_invalid_qos_combination   = 27,
    s1apCauseRadioNetwork_interrat_redirection  = 28,
    s1apCauseRadioNetwork_interaction_with_other_procedure  = 29,
    s1apCauseRadioNetwork_unknown_E_RAB_ID  = 30,
    s1apCauseRadioNetwork_multiple_E_RAB_ID_instances   = 31,
    s1apCauseRadioNetwork_encryption_and_or_integrity_protection_algorithms_not_supported   = 32,
    s1apCauseRadioNetwork_s1_intra_system_handover_triggered    = 33,
    s1apCauseRadioNetwork_s1_inter_system_handover_triggered    = 34,
    s1apCauseRadioNetwork_x2_handover_triggered = 35,
    s1apCauseRadioNetwork_redirection_towards_1xRTT = 36,
    s1apCauseRadioNetwork_not_supported_QCI_value   = 37,
    s1apCauseRadioNetwork_invalid_CSG_Id    = 38,
    s1apCauseRadioNetwork_release_due_to_pre_emption    = 39
} e_s1apCauseRadioNetwork;

typedef enum s1apCauseTransport {
    s1apCauseTransport_transport_resource_unavailable   = 0,
    s1apCauseTransport_unspecified  = 1
} e_s1apCauseTransport;

typedef enum s1apCauseProtocol {
    s1apCauseProtocol_transfer_syntax_error = 0,
    s1apCauseProtocol_abstract_syntax_error_reject  = 1,
    s1apCauseProtocol_abstract_syntax_error_ignore_and_notify   = 2,
    s1apCauseProtocol_message_not_compatible_with_receiver_state    = 3,
    s1apCauseProtocol_semantic_error    = 4,
    s1apCauseProtocol_abstract_syntax_error_falsely_constructed_message = 5,
    s1apCauseProtocol_unspecified   = 6
} e_s1apCauseProtocol;

typedef enum s1apCauseMisc {
    s1apCauseMisc_control_processing_overload   = 0,
    s1apCauseMisc_not_enough_user_plane_processing_resources    = 1,
    s1apCauseMisc_hardware_failure  = 2,
    s1apCauseMisc_om_intervention   = 3,
    s1apCauseMisc_unspecified   = 4,
    s1apCauseMisc_unknown_PLMN  = 5
} e_s1apCauseMisc;



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

typedef struct s1apCause {
    s1apCause_PR present;
    union s1apCause_u {
        s1apCauseRadioNetwork_t  radioNetwork;
        s1apCauseTransport_t     transport;
        s1apCauseNas_t   nas;
        s1apCauseProtocol_t  protocol;
        s1apCauseMisc_t  misc;
    } choice;
} s1apCause_t;

/*36.413: 9.2.1.3a*/
enum ie_RRC_est_cause {
	EMERGENCY,
	HIGHPRIORITYACCESS,
	MT_ACCESS,
	MO_SIGNALLING,
	MO_DATA,
	DELAYTOLERANTACCESS,
	MO_VOICECALL,
	MO_EXCEPTIONDATA
};

//Service Request
struct STMSI {
	uint8_t mme_code;
    uint32_t m_TMSI;

};

/**E-RAB structure declarations**/
struct eRAB_header { //TODO: making provision, chec -is it needed?
	unsigned char criticality;
};

struct eRAB_setup_ctx_SU {
	unsigned short eRAB_id;
	unsigned short dont_know_byte;
	unsigned int transp_layer_addr;
	unsigned int gtp_teid;
};

union eRAB_IE {
	struct eRAB_setup_ctx_SU su_res;
};

typedef struct eRAB_elements {
	struct eRAB_header header;
	unsigned char no_of_elements;
	union eRAB_IE *elements;
}eRAB_elements;
/**eRAB structures end**/

/* TODO : Change type */
typedef struct ue_aggregate_maximum_bitrate {
	uint32_t uEaggregateMaxBitRateDL;
	uint32_t uEaggregateMaxBitRateUL;
} ue_aggregate_maximum_bitrate;

typedef struct allocation_retention_prio {
	uint8_t prioLevel :4;
	uint8_t preEmptionCapab :1;
	uint8_t preEmptionVulnebility :1;
	uint8_t spare :2;
} allocation_retention_prio;


typedef struct E_RAB_Level_QoS_Params {
	uint8_t qci;
	//uint8_t ext;
    allocation_retention_prio arPrio;
} E_RAB_Level_QoS_Params;


typedef struct ERABSetup {
	uint8_t e_RAB_ID;
	E_RAB_Level_QoS_Params e_RAB_QoS_Params;
	uint32_t transportLayerAddress;
	uint32_t gtp_teid;
	struct nasPDU nas;
} ERABSetup;


struct ERAB_admitted_list{
        int count ;
        ERAB_admitted erab_admitted[MAX_ERAB_SIZE];
};

struct ERABSetupList{
        int count ;
        ERABSetup eRABSetup[MAX_ERAB_SIZE];
};


#define SECURITY_KEY_SIZE 32
typedef struct proto_IE_data {
	int 			IE_type;
    union value{
        struct ie_enb_name 	enb_name;
        struct ie_global_enb_id global_enb_id;
        long			enb_ue_s1ap_id;
        long			mme_ue_s1ap_id;
        struct 			nasPDU nas;
        struct s1apCause cause;
        struct TAI 		 tai;
        struct CGI 		 utran_cgi;
        struct STMSI	 s_tmsi;
        enum ie_RRC_est_cause 	rrc_est_cause;
        struct eRAB_elements 	erab;
        ue_aggregate_maximum_bitrate ue_aggrt_max_bit_rate;
        ERABSetup E_RABToBeSetupItemCtxtSUReq;
        erab_to_be_modified_list erab_to_be_mod_list;
        ue_sec_capabilities ue_sec_capab;
        uint8_t sec_key[SECURITY_KEY_SIZE];
        struct targetId target_id;
        enum handoverType handoverType;
        struct src_target_transparent_containerIE srcToTargetTranspContainer;
        struct src_target_transparent_containerIE targetToSrcTranspContainer;
    	struct enB_status_transfer_transparent_container_list enB_status_transfer_transparent_containerlist;
        struct ERAB_admitted_list erab_admittedlist;
    }val;
}proto_IEs;

struct proto_IE {
    ProcedureCode_t  procedureCode;
    Criticality_t    criticality;
	short 		no_of_IEs;
	proto_IEs	*data;
	uint8_t     ie_nas_index;
    uint8_t     ie_tai_index;
    uint8_t     ie_cgi_index;
};

// refer 36.413 . Section 9.3.6 
enum protocolie_id {
	id_MME_UE_S1AP_ID = 0,
	id_Cause = 2,
	id_eNB_UE_S1AP_ID = 8,
	id_ERABToBeSetupListCtxtSUReq = 24,
	id_NAS_PDU = 26,
	id_ERABToBeSetupItemCtxtSUReq = 52,
	id_uEaggregatedMaximumBitrate = 66,
	id_SecurityKey = 73,
    id_ueAssociatedLogicalS1Conn = 91,
    id_ResetType = 92, 
	id_UE_S1AP_IDs = 99,
	id_UESecurityCapabilities = 107,
};

enum criticality{
	CRITICALITY_REJECT = 0x0,
	CRITICALITY_IGNORE = 0x40,
	CRITICALITY_NOTIFY,
};

/* Refer 36.413 section - 9.3.6 */
enum procedure_code {
	id_InitialContextSetup = 9,
	id_downlinkNASTransport = 11,
    id_reset           = 14,
	id_errorIndication = 15,
	id_UEContexRelease = 23,
};

typedef struct s1ap_PDU {
	unsigned char procedurecode;
	unsigned char criticality;
	struct proto_IE value;
}s1ap_PDU;

struct s1ap_header{
	unsigned short procedure_code;
	unsigned char criticality;
};

#if 0
// ajaymerge - need to delete following code ? 
//TBD: changing the pos to uint16_t is
// creating NAS encode failures. Need to revisit this change.
#define BUFFER_SIZE 1000 /* S1AP packet max size */

typedef struct Buffer {
	unsigned char buf[BUFFER_SIZE];
	unsigned char pos;
}Buffer;
#endif
#endif /*__S1AP_STRUCTS_H*/

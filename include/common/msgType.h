/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_COMMON_MSGTYPE_H_
#define INCLUDE_COMMON_MSGTYPE_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "common_proc_info.h"
#include "err_codes.h"
#include "s6_common_types.h"
#include "s11_structs.h"
#include "s1ap_structs.h"
#include "s1ap_ie.h"

#define REQ_ARGS 0x0000

typedef enum msg_type_t {
    attach_request = 0,
    attach_reject,
    auth_info_request,
    auth_info_answer,
    update_loc_request,
    update_loc_answer,
    auth_request,
    auth_response,
    id_request,
    id_response,
    sec_mode_command,
    sec_mode_complete,
    esm_info_request,
    esm_info_response,
    create_session_request,
    create_session_response,
    init_ctxt_request,
    init_ctxt_response,
    modify_bearer_request,
    modify_bearer_response,
    attach_complete,
    detach_request,
    detach_accept,
    purge_request,
    purge_answser,
    delete_session_request,
    delete_session_response,
    s1_release_request,
    s1_release_command,
    s1_release_complete,
    release_bearer_request,
    release_bearer_response,
    ni_detach_request,
    detach_accept_from_ue,
    cancel_location_request,
    cancel_location_answer,
    downlink_data_notification,
    ddn_acknowledgement,
    ddn_failure_indication,
    paging_request,
    service_request,
    service_reject,
    ics_req_paging,
    tau_request,
    tau_response,
    tau_reject,
    emm_info_request,
    s1_reset,
    handover_required,
    handover_request,	
    handover_request_acknowledge,
    handover_command,	
    enb_status_transfer,
    mme_status_transfer,	
    handover_notify,
    handover_failure,
    handover_cancel,
    handover_preparation_failure,
    handover_cancel_ack,
    erab_mod_indication,
    erab_mod_confirmation,
    create_bearer_request,
    create_bearer_response,
    delete_bearer_request,
    delete_bearer_response,
    erab_setup_request,
    erab_setup_response,
    erab_release_command,
    erab_release_response,
    activate_dedicated_eps_bearer_ctxt_request,
    activate_dedicated_eps_bearer_ctxt_accept,
    activate_dedicated_eps_bearer_ctxt_reject,
    deactivate_eps_bearer_context_request,
    deactivate_eps_bearer_context_accept,
    enb_status_msg,
    max_msg_type
} msg_type_t;

struct s1_incoming_msg_header {
    uint32_t destInstAddr;
    uint32_t srcInstAddr;
    msg_type_t msg_type;
    int ue_idx;
    int s1ap_enb_ue_id;
}__attribute__ ((packed));
typedef struct s1_incoming_msg_header s1_incoming_msg_header_t;

struct rawNas_Q_msg {
	uint8_t 	nasMsgBuf[MAX_NAS_MSG_SIZE]; 
	uint16_t 	nasMsgSize; 
}__attribute__ ((packed));
typedef struct rawNas_Q_msg rawNas_Q_msg_t;

/*************************
 * Incoming S1AP Messages
 *************************/
struct ue_attach_info {
    s1_incoming_msg_header_t header;
    int criticality;
    unsigned char IMSI[BINARY_IMSI_LEN];
    struct TAI tai;
    struct CGI utran_cgi;
    struct MS_net_capab ms_net_capab;
    UE_net_capab ue_net_capab;
    bool ue_add_sec_cap_present;
    ue_add_sec_capabilities ue_add_sec_capab;
    enum ie_RRC_est_cause rrc_cause;
    int enb_fd;
    char esm_info_tx_required;
    unsigned char pti;
    unsigned int  flags; /* imsi - 0x00000001, GUTI - 0x00000002 */
    guti mi_guti;
    unsigned char seq_no;
    unsigned char dns_present;
    uint16_t pco_length;
    unsigned char pco_options[MAX_PCO_OPTION_SIZE];
}__attribute__ ((packed));
typedef struct ue_attach_info ue_attach_info_t; 

struct authresp_Q_msg {
    s1_incoming_msg_header_t header;
    int status;
    struct XRES res;
	struct AUTS auts;
}__attribute__ ((packed));
typedef struct authresp_Q_msg authresp_Q_msg_t;

struct secmode_resp_Q_msg {
    s1_incoming_msg_header_t header;
    int status;
}__attribute__ ((packed));
typedef struct secmode_resp_Q_msg secmode_resp_Q_msg_t;

struct esm_resp_Q_msg {
    s1_incoming_msg_header_t header;
    int status;
    struct apn_name apn;
}__attribute__ ((packed));
typedef struct esm_resp_Q_msg esm_resp_Q_msg_t;

struct initctx_resp_Q_msg {
    s1_incoming_msg_header_t header;
    erab_su_resp_list erab_setup_resp_list;
}__attribute__ ((packed));
typedef struct initctx_resp_Q_msg initctx_resp_Q_msg_t; 

struct attach_complete_Q_msg {
    s1_incoming_msg_header_t header;
    unsigned short	status;
}__attribute__ ((packed));
typedef struct attach_complete_Q_msg attach_complete_Q_msg_t;

struct service_req_Q_msg {
    s1_incoming_msg_header_t header;
	int enb_fd;
	unsigned int ksi;
	unsigned int seq_no;
	unsigned short mac;
	struct TAI tai;
	struct CGI utran_cgi;
	struct STMSI s_tmsi;
}__attribute__ ((packed));
typedef struct service_req_Q_msg service_req_Q_msg_t;

struct handover_required_Q_msg {
    s1_incoming_msg_header_t header;
	int s1ap_mme_ue_id;
	int target_enb_context_id;
	int src_enb_context_id;
	enum handoverType handoverType;
	struct s1apCause cause;
	struct targetId target_id;
	struct src_target_transparent_container srcToTargetTranspContainer;
}__attribute__ ((packed));
typedef struct handover_required_Q_msg handover_required_Q_msg_t;

struct handover_req_acknowledge_Q_msg{
    s1_incoming_msg_header_t header;
	int s1ap_mme_ue_id;
	struct ERAB_admitted_list erab_admitted_list;
	struct src_target_transparent_container targetToSrcTranspContainer;
}__attribute__ ((packed));
typedef struct handover_req_acknowledge_Q_msg handover_req_acknowledge_Q_msg_t;

struct handover_notify_Q_msg{
    s1_incoming_msg_header_t header;
	int s1ap_mme_ue_id;
	struct CGI utran_cgi;
	struct TAI tai;
}__attribute__ ((packed));
typedef struct handover_notify_Q_msg handover_notify_Q_msg_t;

struct enb_status_transfer_Q_msg {
    s1_incoming_msg_header_t header;
	int s1ap_mme_ue_id;
	struct enB_status_transfer_transparent_container_list enB_status_transfer_transparent_containerlist;
}__attribute__ ((packed));
typedef struct enb_status_transfer_Q_msg enb_status_transfer_Q_msg_t;

struct handover_failure_Q_msg {
    s1_incoming_msg_header_t header;
	struct s1apCause cause;
}__attribute__ ((packed));
typedef struct handover_failure_Q_msg handover_failure_Q_msg_t;

struct handover_cancel_Q_msg {
    s1_incoming_msg_header_t header;
	struct s1apCause cause;
}__attribute__ ((packed));
typedef struct handover_cancel_Q_msg handover_cancel_Q_msg_t;

struct tauReq_Q_msg {
    s1_incoming_msg_header_t header;
    int ue_m_tmsi;
    int seq_num;
    int enb_fd;
    UE_net_capab ue_net_capab;
    bool ue_add_sec_cap_present;
    ue_add_sec_capabilities ue_add_sec_capab;
    struct TAI tai;
    struct CGI eUtran_cgi;
}__attribute__ ((packed));
typedef struct tauReq_Q_msg tauReq_Q_msg_t;

struct identityResp_Q_msg {
    s1_incoming_msg_header_t header;
	int status;
	unsigned char IMSI[BINARY_IMSI_LEN];
}__attribute__ ((packed));
typedef struct identityResp_Q_msg identityResp_Q_msg_t;

struct detach_req_Q_msg {
    s1_incoming_msg_header_t header;
	int ue_m_tmsi;
}__attribute__ ((packed));
typedef struct detach_req_Q_msg detach_req_Q_msg_t;

struct erab_mod_ind_Q_msg {
    s1_incoming_msg_header_t header;
	erab_to_be_modified_list erab_to_be_mod_list;
}__attribute__ ((packed));
typedef struct erab_mod_ind_Q_msg erab_mod_ind_Q_msg_t;

struct erab_rel_resp_Q_msg {
    s1_incoming_msg_header_t header;
    int s1ap_mme_ue_id;
    erab_release_list erab_rel_list;
    erab_list erab_failed_to_release_list;
}__attribute__ ((packed));
typedef struct erab_rel_resp_Q_msg erab_rel_resp_Q_msg_t;

struct deactivate_epsbearerctx_accept_Q_msg {
    s1_incoming_msg_header_t header;
    uint8_t eps_bearer_id;
    uint8_t pti;
    struct pco pco_opt;
}__attribute__ ((packed));
typedef struct deactivate_epsbearerctx_accept_Q_msg deactivate_epsbearerctx_accept_Q_msg_t;


/* Refer 36.413 - 9.1.3.2 */
struct erabSuResp_Q_msg {
    s1_incoming_msg_header_t header;
    int s1ap_mme_ue_id;
    erab_setup_list erab_su_list;
    erab_failed_to_setup_list erab_fail_list;
}__attribute__ ((packed));
typedef struct erabSuResp_Q_msg erabSuResp_Q_msg_t;

/* Refer 24.301 - 8.3.1.1 */
struct dedicatedBearerContextAccept_Q_msg {
    s1_incoming_msg_header_t header;
    uint8_t eps_bearer_id;
    uint8_t pti;
    struct pco pco_opt;
}__attribute__ ((packed));
typedef struct dedicatedBearerContextAccept_Q_msg dedicatedBearerContextAccept_Q_msg_t;

/* Refer 24.301  - 8.3.2.1 */
struct dedicatedBearerContextReject_Q_msg {
    s1_incoming_msg_header_t header;
    uint8_t eps_bearer_id;
    uint8_t pti;
    esm_cause_t esm_cause;
}__attribute__ ((packed));
typedef struct dedicatedBearerContextReject_Q_msg dedicatedBearerContextReject_Q_msg_t;

struct s1apEnbStatus_Msg {
    s1_incoming_msg_header_t header;
    uint8_t ver;
    uint8_t status; // 0 down, 1 : up
    uint16_t restart_counter;
    uint32_t context_id;
    int enbId_m;
    int tacid;
    char eNbName[128];
}__attribute__ ((packed));
typedef struct s1apEnbStatus_Msg s1apEnbStatus_Msg_t;

/*************************
 * Outgoing S1AP Messages
 *************************/
struct authreq_info {
		msg_type_t msg_type;
    	int ue_idx;
    	int enb_s1ap_ue_id;
    	int enb_fd;
		uint8_t 	nasMsgBuf[300]; 
		uint8_t 	nasMsgSize; 
}__attribute__ ((packed));
typedef struct authreq_info authreq_info_t;

struct sec_mode_Q_msg {
		msg_type_t msg_type;
    	int ue_idx;
    	int enb_s1ap_ue_id;
    	int enb_fd;
		uint8_t 	nasMsgBuf[300]; 
		uint8_t 	nasMsgSize; 
}__attribute__ ((packed));
typedef struct sec_mode_Q_msg sec_mode_Q_msg_t;

struct esm_req_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	int enb_fd;
	uint8_t 	nasMsgBuf[300]; 
	uint8_t 	nasMsgSize; 
}__attribute__ ((packed));
typedef struct esm_req_Q_msg esm_req_Q_msg_t;

struct init_ctx_req_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	int enb_fd;
	uint64_t exg_max_ul_bitrate;
	uint64_t exg_max_dl_bitrate;
	extended_ue_ambr ext_ue_ambr;
	unsigned char sec_key[32];
	struct fteid gtp_teid;
	unsigned char bearer_id;
    uint8_t qci;
    uint8_t pl;
    uint8_t pvi;
    uint8_t pci;
	bool ho_restrict_list_presence;
	ho_restriction_list ho_restrict_list;
	uint8_t 	nasMsgBuf[300]; 
	uint8_t 	nasMsgSize; //dont change size..lot of dependency on size  
}__attribute__ ((packed));
typedef struct init_ctx_req_Q_msg init_ctx_req_Q_msg_t;

struct detach_accept_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	int enb_fd;
	uint8_t 	nasMsgBuf[300]; 
	uint8_t 	nasMsgSize; //dont change size..lot of dependency on size  
}__attribute__ ((packed));
typedef struct detach_accept_Q_msg detach_accept_Q_msg_t;

struct s1relcmd_info{
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	int enb_fd;
	s1apCause_t cause;
}__attribute__ ((packed));
typedef struct s1relcmd_info s1relcmd_info_t;

struct ni_detach_request_Q_msg {
    msg_type_t msg_type;
    int ue_idx;
    int enb_s1ap_ue_id;
    int enb_fd;
	uint8_t 	nasMsgBuf[300]; 
	uint8_t 	nasMsgSize; 
}__attribute__ ((packed));
typedef struct ni_detach_request_Q_msg ni_detach_request_Q_msg_t;

struct paging_req_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	int enb_fd;
	enum s1ap_cn_domain cn_domain;
	unsigned char IMSI[BINARY_IMSI_LEN];
	struct TAI tai;
};
#define PAGING_REQUEST_BUF_SIZE sizeof(struct paging_req_Q_msg)

struct ics_req_paging_Q_msg {
	msg_type_t msg_type;
    int ue_idx;
    int enb_s1ap_ue_id;
    int enb_fd;
	unsigned long ueag_max_ul_bitrate;
	unsigned long ueag_max_dl_bitrate;
	erab_setup_list erab_su_list;
	unsigned char sec_key[32];
};
#define ICS_REQ_PAGING_BUF_SIZE sizeof(struct ics_req_paging_Q_msg)

struct commonRej_info
{
  msg_type_t msg_type;
  int ue_idx; /*mme s1ap UE id*/
  int s1ap_enb_ue_id;
  int enb_fd;
  unsigned char cause;
  uint8_t 	nasMsgBuf[300]; 
  uint8_t 	nasMsgSize; 
};

#define S1AP_REQ_REJECT_BUF_SIZE sizeof(struct commonRej_info)

struct attachIdReq_info
{
	msg_type_t msg_type;
	int ue_idx; /*mme s1ap UE id*/
	int s1ap_enb_ue_id;
 	int enb_fd;
	uint8_t 	nasMsgBuf[300]; 
	uint8_t 	nasMsgSize; 
};
#define S1AP_ID_REQ_BUF_SIZE sizeof(struct attachIdReq_info)

struct tauResp_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_fd;
	int s1ap_enb_ue_id;
	int status;
	uint8_t 	nasMsgBuf[300]; 
	uint8_t 	nasMsgSize; 
	struct TAI tai;
	unsigned int m_tmsi;
};

#define S1AP_TAURESP_BUF_SIZE sizeof(struct tauResp_Q_msg)

struct ue_emm_info {
	msg_type_t msg_type;
	uint32_t enb_fd;
	uint32_t enb_s1ap_ue_id;
	uint32_t mme_s1ap_ue_id;
	uint8_t 	nasMsgBuf[300]; 
	uint8_t 	nasMsgSize; 
};

#define UE_EMM_INFO_BUF_SIZE sizeof(struct ue_emm_info)

struct erab_mod_confirm {
	msg_type_t msg_type;
	uint32_t enb_context_id;
	uint32_t enb_s1ap_ue_id;
	uint32_t mme_s1ap_ue_id;
	erab_modified_list erab_mod_list;
};

#define ERAB_MOD_CONFIRM_BUF_SIZE sizeof(struct erab_mod_confirm)

struct handover_request_Q_msg {
	msg_type_t msg_type;
	uint32_t target_enb_context_id;
	uint32_t s1ap_mme_ue_id;
	enum handoverType handoverType;
	s1apCause_t cause;
	struct src_target_transparent_container src_to_target_transparent_container;
	ue_aggregate_maximum_bitrate ue_aggrt_max_bit_rate;
	erab_setup_list erab_su_list;
	struct security_context security_context;
	struct gummei gummei;
};

#define S1AP_HO_REQUEST_BUF_SIZE sizeof(struct handover_request_Q_msg)

struct handover_command_Q_msg {
	msg_type_t msg_type;
	int src_enb_context_id;
	int s1ap_mme_ue_id;
	int s1ap_enb_ue_id;
	enum handoverType handoverType;
	struct ERABs_Subject_to_Forwarding_List erabs_Subject_to_Forwarding_List;
	struct src_target_transparent_container target_to_src_transparent_container;
};
#define S1AP_HO_COMMAND_BUF_SIZE sizeof(struct handover_command_Q_msg)

struct mme_status_transfer_Q_msg {
	msg_type_t msg_type;
	int s1ap_mme_ue_id;
	int s1ap_enb_ue_id;
	struct enB_status_transfer_transparent_container_list enB_status_transfer_transparent_containerlist;
	int target_enb_context_id;
};
typedef struct mme_status_transfer_Q_msg mme_status_transfer_Q_msg_t;

#define S1AP_MME_STATUS_TRANSFER_BUF_SIZE sizeof(struct mme_status_transfer_Q_msg)

struct handover_preparation_failure_Q_msg {
	msg_type_t msg_type;
	int src_enb_context_id;
	int s1ap_mme_ue_id;
	int s1ap_enb_ue_id;
	s1apCause_t cause;
};
#define S1AP_HANDOVER_PREPARATION_FAILURE_BUF_SIZE sizeof(struct handover_preparation_failure_Q_msg)

struct handover_cancel_ack_Q_msg {
	msg_type_t msg_type;
	int src_enb_context_id;
	int s1ap_mme_ue_id;
	int s1ap_enb_ue_id;
};
#define S1AP_HANDOVER_CANCEL_ACK_BUF_SIZE sizeof(struct handover_cancel_ack_Q_msg)

/* Refer 36.413 - 9.1.3.1 */
struct erabsu_ctx_req_Q_msg {
    msg_type_t msg_type;
    uint32_t mme_ue_s1ap_id;
    uint32_t enb_s1ap_ue_id;
    ue_aggregate_maximum_bitrate ue_aggrt_max_bit_rate;
    erab_setup_list erab_su_list;
    uint32_t enb_context_id;
    Buffer nas_buf[DED_BEARER_COUNT];
};

#define S1AP_ERABSUREQ_BUF_SIZE sizeof(struct erabsu_ctx_req_Q_msg)

struct erab_release_command_Q_msg {
    msg_type_t msg_type;
    uint32_t mme_ue_s1ap_id;
    uint32_t enb_s1ap_ue_id;
    ue_aggregate_maximum_bitrate ue_aggrt_max_bit_rate;
    erab_list erab_to_be_released_list;
    uint32_t enb_context_id;
    uint8_t nasMsgBuf[300];
    uint8_t nasMsgSize;
};
#define S1AP_ERAB_RELEASE_COMMAND_BUF_SIZE sizeof(struct erab_release_command_Q_msg)

struct initial_ue_msg {
    s1_incoming_msg_header_t header;
    rawNas_Q_msg_t  nasMsg;
    int enb_fd;
    int criticality;
    unsigned char IMSI[BINARY_IMSI_LEN];
    struct TAI tai;
    struct CGI utran_cgi;
    enum ie_RRC_est_cause rrc_cause;
	struct STMSI s_tmsi;
}__attribute__ ((packed));
typedef struct initial_ue_msg initial_ue_msg_t; 

struct uplink_nas {
    s1_incoming_msg_header_t header;
    rawNas_Q_msg_t  nasMsg;
    int enb_fd;
   	uint32_t	s1ap_enb_ue_id;
   	uint32_t	s1ap_mme_ue_id;
	struct TAI tai;
	struct CGI utran_cgi;
	struct STMSI s_tmsi;
}__attribute__ ((packed));
typedef struct uplink_nas uplink_nas_t;

struct ue_context_rel_req {
    s1_incoming_msg_header_t header;
}__attribute__ ((packed));
typedef struct ue_context_rel_req ue_context_rel_req_t; 

/*************************
 * Outgoing GTP Messages
 *************************/
struct CS_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	unsigned char IMSI[BINARY_IMSI_LEN];
	struct apn_name selected_apn;
	struct TAI tai;
	struct CGI utran_cgi;
	unsigned char MSISDN[MSISDN_STR_LEN];
	uint32_t max_requested_bw_dl;
	uint32_t max_requested_bw_ul;
	unsigned int  paa_v4_addr;
	uint16_t pco_length;
	unsigned char pco_options[MAX_PCO_OPTION_SIZE];
	bool dcnr_flag;
	uint32_t sgw_ip;
	uint32_t pgw_ip;
};
#define S11_CSREQ_STAGE5_BUF_SIZE sizeof(struct CS_Q_msg)

#define S11_MB_INDICATION_FLAG_SIZE 3
struct MB_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	struct TAI tai;
	struct CGI utran_cgi;
	unsigned short indication[S11_MB_INDICATION_FLAG_SIZE];/*Provision*/
	struct fteid s11_sgw_c_fteid;
	bool userLocationInformationIePresent;
	bool servingNetworkIePresent;
	bearer_ctx_list_t bearer_ctx_list;
};
#define S11_MBREQ_STAGE7_BUF_SIZE sizeof(struct MB_Q_msg)

#define S11_DS_INDICATION_FLAG_SIZE 3
struct DS_Q_msg {
	msg_type_t msg_type;
    int ue_idx;
	unsigned char indication[S11_DS_INDICATION_FLAG_SIZE];/*Provision*/
	unsigned char bearer_id;
	struct fteid s11_sgw_c_fteid;
};
#define S11_DTCHREQ_STAGE1_BUF_SIZE sizeof(struct DS_Q_msg)


#define S11_RB_INDICATION_FLAG_SIZE 3
struct RB_Q_msg{
	msg_type_t msg_type;
	int ue_idx;
	unsigned short indication[S11_RB_INDICATION_FLAG_SIZE];
	unsigned char bearer_id;
	struct fteid s11_sgw_c_fteid;
	struct fteid s1u_enb_fteid;
};
#define S11_RBREQ_STAGE1_BUF_SIZE sizeof(struct RB_Q_msg)

struct DDN_ACK_Q_msg{
	msg_type_t msg_type;
	uint32_t seq_no;
	uint8_t cause;
	struct fteid s11_sgw_c_fteid;
};
#define S11_DDN_ACK_BUF_SIZE sizeof(struct DDN_ACK_Q_msg)

struct DDN_FAIL_Q_msg{
	msg_type_t msg_type;
	uint32_t seq_no;
	uint8_t cause;
	struct fteid s11_sgw_c_fteid;
};
#define S11_DDN_FAIL_BUF_SIZE sizeof(struct DDN_FAIL_Q_msg)

struct CB_RESP_Q_msg {
    msg_type_t msg_type;
    int ue_idx;
    uint16_t destination_port;
    uint8_t cause;
    uint32_t seq_no;
    bearer_ctxt_cb_resp_list_t bearer_ctxt_cb_resp_list;
    struct pco pco;
    struct fteid s11_sgw_c_fteid;
 };
 #define S11_CBRESP_BUF_SIZE sizeof(struct CB_RESP_Q_msg)
 
struct DB_RESP_Q_msg {
    msg_type_t msg_type;
    int ue_idx;
    uint16_t destination_port;
    uint32_t seq_no;
    uint8_t cause;
    uint8_t linked_bearer_id;
    bearer_ctxt_db_resp_list_t bearer_ctxt_db_resp_list;
    struct pco pco;
    struct fteid s11_sgw_c_fteid;
 };
#define S11_DBRESP_BUF_SIZE sizeof(struct DB_RESP_Q_msg)

struct ID_Q_msg{
    msg_type;
    int ue_idx;
    guti _guti;

};
#define S10_IDREQ_STAGE5_BUF_SIZE sizeof(struct ID_Q_msg)

struct ID_RESP_Q_msg{
    msg_type;
    int ue_idx;
    uint8_t cause;
    ImsiIeData imsi;    ////conditonal optional
    /*
    uint8_t trace_information;
    uint8_t ue_usage_type;
    uint8_t monitoring_event_information;
    uint8_t private_extension;
   */
    bool trace_information;
    bool ue_usage_type;
    bool monitoring_event_information;
    bool private_extension;

};
#define S10_IDRESP_STAGE5_BUF_SIZE sizeof(struct ID_RESP_Q_msg)
 
/*************************
 * Incoming GTP Messages
 *************************/

typedef struct gtp_incoming_msg_data_t {
    uint32_t destInstAddr;
    uint32_t srcInstAddr;
    msg_type_t msg_type;
}gtp_incoming_msg_data_t;

struct csr_Q_msg {
    gtp_incoming_msg_data_t header;
    int s11_mme_cp_teid;
    int status;
    struct fteid s11_sgw_fteid;
    struct fteid s5s8_pgwc_fteid;
    struct fteid s1u_sgw_fteid;
    struct fteid s5s8_pgwu_fteid;
    struct PAA pdn_addr;
    bearer_qos_t bearerQos;
    uint16_t pco_length;
    unsigned char pco_options[MAX_PCO_OPTION_SIZE];
    uint32_t apn_ambr_ul;
    uint32_t apn_ambr_dl;
};

struct MB_resp_Q_msg {
    gtp_incoming_msg_data_t header;
    int s11_mme_cp_teid;
    uint8_t cause;
    bearer_ctxt_mb_resp_list_t bearer_ctxt_mb_resp_list;
};

struct DS_resp_Q_msg {
    gtp_incoming_msg_data_t header;
    int s11_mme_cp_teid;
};

struct RB_resp_Q_msg {
    gtp_incoming_msg_data_t header;
    int s11_mme_cp_teid;
    struct fteid s1u_sgw_fteid;
};

struct ddn_Q_msg {
    gtp_incoming_msg_data_t header;
    int s11_mme_cp_teid;
    struct ARP arp;
    uint8_t cause;
    uint8_t eps_bearer_id;
    uint32_t seq_no;
    uint32_t sgw_ip;
};

struct cb_req_Q_msg {
    gtp_incoming_msg_data_t header;
    int s11_mme_cp_teid;
    uint32_t sgw_ip;
    uint16_t source_port;
    uint8_t linked_eps_bearer_id;
    uint32_t seq_no;
    struct pco pco;
    bearer_ctx_list_t bearer_ctx_list;

};

struct db_req_Q_msg {
    gtp_incoming_msg_data_t header;
    int s11_mme_cp_teid;
    uint32_t sgw_ip;
    uint16_t source_port;
    uint8_t cause;
    uint8_t linked_bearer_id;
    uint32_t seq_no;
    uint8_t eps_bearer_ids_count;
    uint8_t eps_bearer_ids[DED_BEARER_COUNT];
    struct pco pco;
};
struct id_resp_Q_msg{
    gtp_incoming_msg_data_t header;
    uint8_t cause;
    ImsiIeData imsi;    ////conditonal optional
    uint8_t trace_information;
    uint8_t ue_usage_type;
    uint8_t monitoring_event_information;
    uint8_t private_extension;
   
};

struct ID_req_Q_msg(){
    gtp_incoming_msg_data_t header;
    guti _guti;
};


#define GTP_READ_MSG_BUF_SIZE sizeof(gtp_incoming_msg_data_t)



/*************************
 * Outgoing S6 Messages
 *************************/
struct s6a_Q_msg {
	msg_type_t msg_type;
	unsigned char imsi[16];
	struct TAI tai;
	struct AUTS auts;
	unsigned int ue_idx;
	supported_features_list supp_features_list;
};
#define RESET_S6A_REQ_MSG(msg)  {(msg)->auts.len = 0; (msg)->ue_idx=0;memset((msg), 0, sizeof(*msg));}

struct s6a_purge_Q_msg {
	int ue_idx;
	unsigned char IMSI[BINARY_IMSI_LEN];
};

/*************************
 * Incoming S6 Messages
 *************************/
typedef struct E_UTRAN_sec_vector {
    struct RAND rand;
    struct XRES xres;
    struct AUTN autn;
    struct KASME kasme;
} E_UTRAN_sec_vector;

struct s6_incoming_msg_header {
	uint32_t destInstAddr;
	uint32_t srcInstAddr;
	msg_type_t msg_type;
	int ue_idx;
	unsigned char IMSI[16];
};
typedef struct s6_incoming_msg_header s6_incoming_msg_header_t;

struct aia_Q_msg {
    s6_incoming_msg_header_t header;
    int res;
    E_UTRAN_sec_vector sec;
};
typedef struct aia_Q_msg aia_Q_msg_t;

struct ula_Q_msg {
    s6_incoming_msg_header_t header;
    unsigned int access_restriction_data;
    int subscription_status;
    int net_access_mode;
    unsigned int RAU_TAU_timer;
    int res;
    uint32_t max_requested_bw_dl;
    uint32_t max_requested_bw_ul;
    uint32_t extended_max_requested_bw_dl;
    uint32_t extended_max_requested_bw_ul;
    unsigned int apn_config_profile_ctx_id;
    int all_APN_cfg_included_ind;
    char MSISDN[MSISDN_STR_LEN];
    struct apn_name selected_apn;
    uint32_t static_addr;
    supported_features_list supp_features_list;
};
typedef struct ula_Q_msg ula_Q_msg_t;

struct purge_resp_Q_msg {
    s6_incoming_msg_header_t header;
    int status;
};
typedef struct purge_resp_Q_msg purge_resp_Q_msg_t;

enum CancellationType {
    MME_UPDATE_PROCEDURE = 0,
    SGSN_UPDATE_PROCEDURE = 1,
    SUBSCRIPTION_WITHDRAWAL = 2,
    INVALID_TYPE
};

struct clr_Q_msg {
    s6_incoming_msg_header_t header;
    msg_type_t msg_type;
    char origin_host[18];
    char origin_realm[15];
    uint8_t imsi[15];   
    enum CancellationType c_type;
}__attribute__ ((packed));
typedef struct clr_Q_msg clr_Q_msg_t;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_COMMON_MSGTYPE_H_ */

/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INCLUDE_COMMON_MSGTYPE_H_
#define INCLUDE_COMMON_MSGTYPE_H_

#include "err_codes.h"
#include "s6_common_types.h"
#include "s11_structs.h"
#include "s1ap_structs.h"
#include "s1ap_ie.h"

#define NAS_RAND_SIZE 16
#define NAS_AUTN_SIZE 16
extern int dns_enabled;

typedef enum msg_data_t
{
    ue_data = 0,
    session_data
}msg_data_t;

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
    paging_request,
    service_request,
    service_reject,
    ics_req_paging,
    tau_request,
    tau_response,
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
    max_msg_type
} msg_type_t;


/*************************
 * Incoming S1AP Messages
 *************************/
struct ue_attach_info {
    int s1ap_enb_ue_id;
    int criticality;
    unsigned char IMSI[BINARY_IMSI_LEN];
    struct TAI tai;
    struct CGI utran_cgi;
    struct MS_net_capab ms_net_capab;
    struct UE_net_capab ue_net_capab;
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
	
};

struct authresp_Q_msg {
    int status;
    struct XRES res;
	struct AUTS auts;
};

struct secmode_resp_Q_msg {
    int ue_idx;
    int status;
};

struct esm_resp_Q_msg {
    int status;
    struct apn_name apn;
};

struct initctx_resp_Q_msg{
    unsigned short 	eRAB_id;
    unsigned int 	transp_layer_addr;
    unsigned int 	gtp_teid;
};

struct attach_complete_Q_msg {
    unsigned short	status;
};

struct service_req_Q_msg {
	int enb_fd;
	unsigned int ksi;
	unsigned int seq_no;
	unsigned short mac;
	struct TAI tai;
	struct CGI utran_cgi;
	struct STMSI s_tmsi;
};

struct handover_required_Q_msg {
	int s1ap_enb_ue_id;
	int s1ap_mme_ue_id;
	int target_enb_context_id;
	int src_enb_context_id;
	enum handoverType handoverType;
	struct s1apCause cause;
	struct targetId target_id;
	struct src_target_transparent_container srcToTargetTranspContainer;
};

struct handover_req_acknowledge_Q_msg{
	int s1ap_enb_ue_id;
	int s1ap_mme_ue_id;
	struct ERAB_admitted_list erab_admitted_list;
	struct src_target_transparent_container targetToSrcTranspContainer;
};

struct handover_notify_Q_msg{
	int s1ap_enb_ue_id;
	int s1ap_mme_ue_id;
	struct CGI utran_cgi;
	struct TAI tai;
};

struct enb_status_transfer_Q_msg {
	int s1ap_mme_ue_id;
	int s1ap_enb_ue_id;
	struct enB_status_transfer_transparent_container_list enB_status_transfer_transparent_containerlist;
};

struct handover_failure_Q_msg {
	struct s1apCause cause;
};

struct handover_cancel_Q_msg {
	struct s1apCause cause;
};

struct tauReq_Q_msg {
    int seq_num;
    int enb_fd;
    struct TAI tai;
    struct CGI eUtran_cgi;
};

struct identityResp_Q_msg {
	int status;
	unsigned char IMSI[BINARY_IMSI_LEN];
};

struct detach_req_Q_msg {
	int ue_m_tmsi;
};

typedef union s1_incoming_msgs_t {
    struct ue_attach_info ue_attach_info_m;
    struct authresp_Q_msg authresp_Q_msg_m;
    struct secmode_resp_Q_msg secmode_resp_Q_msg_m;
    struct esm_resp_Q_msg esm_resp_Q_msg_m;
    struct initctx_resp_Q_msg initctx_resp_Q_msg_m;
    struct attach_complete_Q_msg attach_complete_Q_msg_m;
    struct service_req_Q_msg service_req_Q_msg_m;
    struct identityResp_Q_msg identityResp_Q_msg_m;
    struct tauReq_Q_msg tauReq_Q_msg_m;
    struct detach_req_Q_msg detachReq_Q_msg_m;
    struct handover_required_Q_msg handover_required_Q_msg_m;
    struct handover_req_acknowledge_Q_msg handover_req_acknowledge_Q_msg_m;
    struct handover_notify_Q_msg handover_notify_Q_msg_m;
    struct enb_status_transfer_Q_msg enb_status_transfer_Q_msg_m;
    struct handover_failure_Q_msg handover_failure_Q_msg_m;
    struct handover_cancel_Q_msg handover_cancel_Q_msg_m;
}s1_incoming_msgs_t;

typedef struct s1_incoming_msg_data_t {
    uint32_t destInstAddr;
    uint32_t srcInstAddr;
    msg_type_t msg_type;
    int ue_idx;
    int s1ap_enb_ue_id;
    s1_incoming_msgs_t msg_data;
}s1_incoming_msg_data_t;

#define S1_READ_MSG_BUF_SIZE sizeof(s1_incoming_msg_data_t)

/*************************
 * Outgoing S1AP Messages
 *************************/
struct authreq_info {
		msg_type_t msg_type;
    	int ue_idx;
    	int enb_s1ap_ue_id;
    	unsigned char rand[NAS_RAND_SIZE];
    	unsigned char autn[NAS_AUTN_SIZE];
     	//struct TAI tai;
    	int enb_fd;
};

#define S1AP_AUTHREQ_STAGE2_BUF_SIZE sizeof(struct authreq_info)

struct sec_mode_Q_msg {
		msg_type_t msg_type;
    	int ue_idx;
    	int enb_s1ap_ue_id;
    	struct UE_net_capab ue_network;
		struct MS_net_capab  ms_net_capab;
    	struct KASME key;
    	uint8_t int_key[NAS_INT_KEY_SIZE];
    	uint32_t dl_seq_no;
    	int enb_fd;
};

#define S1AP_SECREQ_STAGE3_BUF_SIZE sizeof(struct sec_mode_Q_msg)


struct esm_req_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	uint8_t pti;
	uint8_t int_key[NAS_INT_KEY_SIZE];
	unsigned short dl_seq_no;
	int enb_fd;
};

#define S1AP_ESMREQ_STAGE4_BUF_SIZE sizeof(struct esm_req_Q_msg)

struct init_ctx_req_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	unsigned long exg_max_ul_bitrate;
	unsigned long exg_max_dl_bitrate;
	struct fteid gtp_teid;
	struct TAI tai;
	struct apn_name apn;
	struct apn_name selected_apn;
	struct PAA pdn_addr;
	unsigned char sec_key[32];
	unsigned char bearer_id;
	uint8_t int_key[NAS_INT_KEY_SIZE];
	uint16_t dl_seq_no;
	int enb_fd;
	unsigned char pti;
	unsigned int m_tmsi;
	uint16_t pco_length;
	unsigned char pco_options[MAX_PCO_OPTION_SIZE];
};

#define S1AP_ICSREQ_STAGE6_BUF_SIZE sizeof(struct init_ctx_req_Q_msg)

struct detach_accept_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	uint8_t int_key[NAS_INT_KEY_SIZE];
	uint16_t dl_seq_no;
	int enb_fd;
};

#define S1AP_DTCHACCEPT_STAGE2_BUF_SIZE sizeof(struct detach_accept_Q_msg)


struct s1relcmd_info{
	msg_type_t msg_type;
	int ue_idx;
	int enb_s1ap_ue_id;
	int enb_fd;
	s1apCause_t cause;
};
#define S1AP_RELCMD_STAGE2_BUF_SIZE sizeof(struct s1relcmd_info)


struct ni_detach_request_Q_msg {
    msg_type_t msg_type;
    int ue_idx;
    int enb_s1ap_ue_id;
    uint8_t int_key[NAS_INT_KEY_SIZE];
    uint16_t dl_seq_no;
    int enb_fd;
    unsigned char detach_type;
};
#define S1AP_NI_DTCHREQUEST_BUF_SIZE sizeof(struct ni_detach_request_Q_msg)

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
	unsigned char bearer_id;
	struct fteid gtp_teid;
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
};

#define S1AP_REQ_REJECT_BUF_SIZE sizeof(struct commonRej_info)

struct attachIdReq_info
{
	msg_type_t msg_type;
	int ue_idx; /*mme s1ap UE id*/
	int s1ap_enb_ue_id;
 	int enb_fd;
    unsigned char ue_type;
};
#define S1AP_ID_REQ_BUF_SIZE sizeof(struct attachIdReq_info)

struct tauResp_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	int enb_fd;
	int s1ap_enb_ue_id;
	int status;
	int dl_seq_no;
	uint8_t int_key[NAS_INT_KEY_SIZE];
	struct TAI tai;
	unsigned int m_tmsi;
};

#define S1AP_TAURESP_BUF_SIZE sizeof(struct tauResp_Q_msg)

struct ue_emm_info {
	msg_type_t msg_type;
	uint32_t enb_fd;
	uint32_t enb_s1ap_ue_id;
	uint32_t mme_s1ap_ue_id;
	char     short_network_name[16];
	char     full_network_name[128];
	uint8_t int_key[NAS_INT_KEY_SIZE];
	unsigned short dl_seq_no;
};

#define UE_EMM_INFO_BUF_SIZE sizeof(struct ue_emm_info)

struct handover_request_Q_msg {
	msg_type_t msg_type;
	uint32_t target_enb_context_id;
	uint32_t s1ap_mme_ue_id;
	enum handoverType handoverType;
	s1apCause_t cause;
	struct src_target_transparent_container src_to_target_transparent_container;
	ue_aggregate_maximum_bitrate ue_aggrt_max_bit_rate;
	struct ERABSetupList eRABSetupList;
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
	unsigned int max_requested_bw_dl;
	unsigned int max_requested_bw_ul;
	unsigned int  paa_v4_addr;
	uint16_t pco_length;
	unsigned char pco_options[MAX_PCO_OPTION_SIZE];
	unsigned long sgw_ip;
};
#define S11_CSREQ_STAGE5_BUF_SIZE sizeof(struct CS_Q_msg)

#define S11_MB_INDICATION_FLAG_SIZE 3
struct MB_Q_msg {
	msg_type_t msg_type;
	int ue_idx;
	struct TAI tai;
	struct CGI utran_cgi;
	unsigned short indication[S11_MB_INDICATION_FLAG_SIZE];/*Provision*/
	unsigned char bearer_id;
	struct fteid s11_sgw_c_fteid;
	struct fteid s1u_enb_fteid;
	bool userLocationInformationIePresent;
    	bool servingNetworkIePresent;
};
#define S11_MBREQ_STAGE7_BUF_SIZE sizeof(struct MB_Q_msg)

#define S11_DS_INDICATION_FLAG_SIZE 3
struct DS_Q_msg {
	msg_type_t msg_type;
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
	int s11_sgw_cp_teid;
	uint32_t seq_no;
	uint8_t cause;
};
#define S11_DDN_ACK_BUF_SIZE sizeof(struct DDN_ACK_Q_msg)

struct DDN_FAIL_Q_msg{
	msg_type_t msg_type;
	int ue_idx;
	uint32_t seq_no;
	uint8_t cause;
};
#define S11_DDN_FAIL_BUF_SIZE sizeof(struct DDN_FAIL_Q_msg)
/*************************
 * Incoming GTP Messages
 *************************/
struct csr_Q_msg {
    int status;
    struct fteid s11_sgw_fteid;
    struct fteid s5s8_pgwc_fteid;
    struct fteid s1u_sgw_fteid;
    struct fteid s5s8_pgwu_fteid;
    struct PAA pdn_addr;
    uint16_t pco_length;
    unsigned char pco_options[MAX_PCO_OPTION_SIZE];
};

struct MB_resp_Q_msg {
    struct fteid s1u_sgw_fteid;
};


struct RB_resp_Q_msg {
    struct fteid s1u_sgw_fteid;
};

struct ddn_Q_msg {
    struct ARP arp;
    uint8_t cause;
    uint8_t eps_bearer_id;
    uint32_t seq_no;
};

typedef union gtp_incoming_msgs_t {
    struct csr_Q_msg csr_Q_msg_m;
    struct MB_resp_Q_msg MB_resp_Q_msg_m;
    struct RB_resp_Q_msg RB_resp_Q_msg_m;
    struct ddn_Q_msg ddn_Q_msg_m;    
}gtp_incoming_msgs_t;

typedef struct gtp_incoming_msg_data_t {
    uint32_t destInstAddr;
    uint32_t srcInstAddr;
    msg_type_t msg_type;
    int ue_idx;
    gtp_incoming_msgs_t msg_data;
}gtp_incoming_msg_data_t;

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
#define S6A_REQ_Q_MSG_SIZE sizeof(struct s6a_Q_msg)

struct s6a_purge_Q_msg {
	int ue_idx;
	unsigned char IMSI[BINARY_IMSI_LEN];
};
#define S6A_PURGEREQ_STAGE1_BUF_SIZE sizeof(struct s6a_purge_Q_msg)

/*************************
 * Incoming S6 Messages
 *************************/
typedef struct E_UTRAN_sec_vector {
    struct RAND rand;
    struct XRES xres;
    struct AUTN autn;
    struct KASME kasme;
} E_UTRAN_sec_vector;

struct aia_Q_msg {
    int res;
    E_UTRAN_sec_vector sec;
};

struct ula_Q_msg {
    unsigned int access_restriction_data;
    int subscription_status;
    int net_access_mode;
    unsigned int RAU_TAU_timer;
    int res;
    unsigned int max_requested_bw_dl;
    unsigned int max_requested_bw_ul;
    unsigned int extended_max_requested_bw_dl;
    unsigned int extended_max_requested_bw_ul;
    unsigned int apn_config_profile_ctx_id;
    int all_APN_cfg_included_ind;
    char MSISDN[MSISDN_STR_LEN];
    struct apn_name selected_apn;
    uint32_t static_addr;
    supported_features_list supp_features_list;

};

struct purge_resp_Q_msg {
    int status;
};


enum CancellationType {
    MME_UPDATE_PROCEDURE = 0,
    SGSN_UPDATE_PROCEDURE = 1,
    SUBSCRIPTION_WITHDRAWAL = 2,
    INVALID_TYPE
};

struct clr_Q_msg {
    msg_type_t msg_type;
    char origin_host[18];
    char origin_realm[15];
    uint8_t imsi[15];   
    enum CancellationType c_type;
};


typedef union s6_incoming_msgs_t {
    struct aia_Q_msg aia_Q_msg_m;
    struct ula_Q_msg ula_Q_msg_m;
    struct clr_Q_msg clr_Q_msg_m;	//NI Detach
    struct purge_resp_Q_msg purge_resp_Q_msg_m;
}s6_incoming_msgs_t;

typedef struct s6_incoming_msg_data_t {
	uint32_t destInstAddr;
	uint32_t srcInstAddr;
	msg_type_t msg_type;
	int ue_idx;
	unsigned char IMSI[16];
	s6_incoming_msgs_t msg_data;
}s6_incoming_msg_data_t;

#define S6_READ_MSG_BUF_SIZE sizeof(s6_incoming_msg_data_t)

#endif /* INCLUDE_COMMON_MSGTYPE_H_ */

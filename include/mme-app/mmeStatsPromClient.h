/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _INCLUDE_mmeStats_H__
#define _INCLUDE_mmeStats_H__

#include <prometheus/gauge.h>
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>


using namespace prometheus;
extern std::shared_ptr<Registry> registry;


void mmeStatsSetupPrometheusThread(void);

enum class mmeStatsCounter {
	MME_NUM_UE_SUB_STATE_ACTIVE,
	MME_NUM_UE_SUB_STATE_IDLE,
	MME_MSG_RX_NAS_ATTACH_REQUEST,
	MME_MSG_RX_NAS_IDENTITY_RESPONSE,
	MME_MSG_RX_NAS_AUTHENTICATION_RESPONSE,
	MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE,
	MME_MSG_RX_NAS_ESM_RESPONSE,
	MME_MSG_RX_S1AP_INIT_CONTEXT_RESPONSE,
	MME_MSG_RX_NAS_ATTACH_COMPLETE,
	MME_MSG_RX_NAS_DETACH_REQUEST,
	MME_MSG_RX_S1AP_RELEASE_REQUEST,
	MME_MSG_RX_S1AP_RELEASE_COMPLETE,
	MME_MSG_RX_S1AP_DETACH_ACCEPT,
	MME_MSG_RX_S1AP_SERVICE_REQUEST,
	MME_MSG_RX_S1AP_TAU_REQUEST,
	MME_MSG_RX_S1AP_HANDOVER_REQUEST_ACK,
	MME_MSG_RX_S1AP_HANDOVER_NOTIFY,
	MME_MSG_RX_S1AP_HANDOVER_REQUIRED,
	MME_MSG_RX_S1AP_ENB_STATUS_TRANSFER,
	MME_MSG_RX_S1AP_HANDOVER_CANCEL,
	MME_MSG_RX_S1AP_HANDOVER_FAILURE,
	MME_MSG_RX_S1AP_ERAB_MODIFICATION_INDICATION,
	MME_MSG_RX_S6A_AUTHENTICATION_INFORMATION_ANSWER,
	MME_MSG_RX_S6A_UPDATE_LOCATION_ANSWER,
	MME_MSG_RX_S6A_PURGE_ANSWER,
	MME_MSG_RX_S6A_CANCEL_LOCATION_REQUEST,
	MME_MSG_RX_S11_CREATE_SESSION_RESPONSE,
	MME_MSG_RX_S11_MODIFY_BEARER_RESPONSE,
	MME_MSG_RX_S11_DELETE_SESSION_RESPONSE,
	MME_MSG_RX_S11_RELEASE_BEARER_RESPONSE,
	MME_MSG_RX_S11_DOWNLINK_NOTIFICATION_INDICATION,
	MME_MSG_TX_NAS_IDENTITY_REQUEST,
	MME_MSG_TX_NAS_AUTHENTICATION_REQUEST,
	MME_MSG_TX_NAS_SECURITY_MODE_COMMAND,
	MME_MSG_TX_NAS_ESM_INFORMATION_REQUEST,
	MME_MSG_TX_NAS_INITIAL_CONTEXT_REQUEST,
	MME_MSG_TX_NAS_EMM_INFORMATION_REQ,
	MME_MSG_TX_NAS_ATTACH_REJECT,
	MME_MSG_TX_NAS_SERVICE_REJECT,
	MME_MSG_TX_NAS_TAU_RESPONSE,
	MME_MSG_TX_S1AP_ERAB_MODIFICATION_INDICATION,
	MME_MSG_TX_NAS_NETWORK_INITIATED_DETACH,
	MME_MSG_TX_S1AP_S1_RELEASE_COMMAND,
	MME_MSG_TX_S1AP_HANDOVER_REQUEST,
	MME_MSG_TX_S1AP_HANDOVER_COMMAND,
	MME_MSG_TX_S1AP_MME_STATUS_TRANSFER,
	MME_MSG_TX_S1AP_HANDOVER_PREPARATION_FAILURE,
	MME_MSG_TX_S1AP_HANDOVER_CANCEL_ACK,
	MME_MSG_TX_S1AP_PAGING_REQUEST,
	MME_MSG_TX_S1AP_ICS_REQUEST_PAGING,
	MME_MSG_TX_S1AP_DETACH_ACCEPT,
	MME_MSG_TX_S11_CREATE_SESSION_REQUEST,
	MME_MSG_TX_S11_MODIFY_BEARER_REQUEST,
	MME_MSG_TX_S11_DELETE_SESSION_REQUEST,
	MME_MSG_TX_S11_DOWNLINK_DATA_NOTIFICATION_ACK,
	MME_MSG_TX_S11_RELEASE_BEARER_REQUEST,
	MME_MSG_TX_S6A_AUTHENTICATION_INFO_REQUEST,
	MME_MSG_TX_S6A_UPDATE_LOCATION_REQUEST,
	MME_MSG_TX_S6A_PURGE_REQUEST,
	MME_PROCEDURES_ATTACH_PROC_IMSI,
	MME_PROCEDURES_ATTACH_PROC_GUTI,
	MME_PROCEDURES_ATTACH_PROC_RESULT_SUCCESS,
	MME_PROCEDURES_ATTACH_PROC_RESULT_FAILURE
};

struct Node 
{
    mmeStatsCounter id;
	std::string label_k;
	std::string label_v;

	Node(mmeStatsCounter id, std::string label_k, std::string label_v)
	{
		this->id = id;
		this->label_k = label_k;
		this->label_v = label_v;
	}

	// operator== is required to compare keys in case of hash collision
	bool operator==(const Node &p) const
	{
		return label_k == p.label_k && label_v == p.label_v && id == p.id;
	}
};

struct hash_fn
{
	std::size_t operator() (const Node &node) const
	{
		std::size_t h1 = std::hash<std::string>()(node.label_k);
		std::size_t h2 = std::hash<std::string>()(node.label_v);
		std::size_t h3 = std::size_t(node.id);
		return h1 ^ h2 ^ h3;
	}
};


class DynamicMetricObject {
	public:
};


class mme_num_ue_DynamicMetricObject : public DynamicMetricObject {
	public:
		mme_num_ue_DynamicMetricObject(Family<Gauge> &mme_num_ue_family,std::string labelk0, std::string labelv0,std::string labelk, std::string labelv):
		 DynamicMetricObject(),
		 gauge(mme_num_ue_family.Add({{labelk0, labelv0},{labelk, labelv}}))
		{
		}
		~mme_num_ue_DynamicMetricObject()
		{
		}
		Gauge &gauge;
};
class mme_num_ue_gauges {
	public:
	mme_num_ue_gauges();
	~mme_num_ue_gauges();
	Family<Gauge> &mme_num_ue_family;
	Gauge &current_sub_state_Active;
	Gauge &current_sub_state_Idle;

	mme_num_ue_DynamicMetricObject* add_dynamic(std::string labelk0, std::string labelv0,std::string labelk, std::string labelv) {
		return new mme_num_ue_DynamicMetricObject(mme_num_ue_family,labelk0, labelv0,labelk, labelv);
 	}
};




class mme_msg_rx_DynamicMetricObject : public DynamicMetricObject {
	public:
		mme_msg_rx_DynamicMetricObject(Family<Counter> &mme_msg_rx_family,std::string labelk0,std::string labelv0,std::string labelk1,std::string labelv1,std::string labelk, std::string labelv):
		 DynamicMetricObject(),
		 counter(mme_msg_rx_family.Add({{labelk0, labelv0},{labelk1, labelv1},{labelk, labelv}}))
		{
		}
		~mme_msg_rx_DynamicMetricObject()
		{
		}
		Counter &counter;
};
class mme_msg_rx_counters {
	public:
	mme_msg_rx_counters();
	~mme_msg_rx_counters();
	Family<Counter> &mme_msg_rx_family;
	Counter &attach_request_attach_request;
	Counter &nas_identity_response_identity_response;
	Counter &nas_authentication_response_authentication_response;
	Counter &nas_security_mode_response_security_mode_response;
	Counter &nas_esm_response_esm_response;
	Counter &s1ap_init_context_response_init_context_response;
	Counter &nas_attach_complete_attach_complete;
	Counter &nas_detach_request_detach_request;
	Counter &s1ap_release_request_release_request;
	Counter &s1ap_release_complete_release_complete;
	Counter &s1ap_detach_accept_detach_accept;
	Counter &s1ap_service_request_service_request;
	Counter &s1ap_tau_request_tau_request;
	Counter &s1ap_handover_request_ack_handover_request_ack;
	Counter &s1ap_handover_notify_handover_notify;
	Counter &s1ap_handover_required_handover_required;
	Counter &s1ap_enb_status_transfer_enb_status_transfer;
	Counter &s1ap_handover_cancel_handover_cancel;
	Counter &s1ap_handover_failure_handover_failure;
	Counter &s1ap_erab_modification_indication_erab_modification_indication;
	Counter &s6a_authentication_information_answer_authentication_information_answer;
	Counter &s6a_update_location_answer_update_location_answer;
	Counter &s6a_purge_answer_purge_answer;
	Counter &s6a_cancel_location_request_cancel_location_request;
	Counter &s11_create_session_response_create_session_response;
	Counter &s11_modify_bearer_response_modify_bearer_response;
	Counter &s11_delete_session_response_delete_session_response;
	Counter &s11_release_bearer_response_release_bearer_response;
	Counter &s11_downlink_notification_indication_downlink_notification_indication;

	mme_msg_rx_DynamicMetricObject* add_dynamic(std::string labelk0,std::string labelv0,std::string labelk1,std::string labelv1,std::string labelk, std::string labelv) {
		return new mme_msg_rx_DynamicMetricObject(mme_msg_rx_family,labelk0,labelv0,labelk1,labelv1,labelk, labelv);
 	}
};



class mme_msg_tx_DynamicMetricObject : public DynamicMetricObject {
	public:
		mme_msg_tx_DynamicMetricObject(Family<Counter> &mme_msg_tx_family,std::string labelk0,std::string labelv0,std::string labelk1,std::string labelv1,std::string labelk, std::string labelv):
		 DynamicMetricObject(),
		 counter(mme_msg_tx_family.Add({{labelk0, labelv0},{labelk1, labelv1},{labelk, labelv}}))
		{
		}
		~mme_msg_tx_DynamicMetricObject()
		{
		}
		Counter &counter;
};
class mme_msg_tx_counters {
	public:
	mme_msg_tx_counters();
	~mme_msg_tx_counters();
	Family<Counter> &mme_msg_tx_family;
	Counter &nas_identity_request_identity_request;
	Counter &nas_authentication_request_authentication_request;
	Counter &nas_security_mode_command_security_mode_command;
	Counter &nas_esm_information_request_esm_information_request;
	Counter &nas_initial_context_request_initial_context_request;
	Counter &nas_emm_information_req_emm_information_req;
	Counter &nas_attach_reject_attach_reject;
	Counter &nas_service_reject_service_reject;
	Counter &nas_tau_response_tau_response;
	Counter &s1ap_erab_modification_erab_modification_indication;
	Counter &nas_network_initiated_detach_network_initiated_detach;
	Counter &s1ap_s1_release_command_s1_release_command;
	Counter &s1ap_handover_request_handover_request;
	Counter &s1ap_handover_command_handover_command;
	Counter &s1ap_mme_status_transfer_mme_status_transfer;
	Counter &s1ap_handover_preparation_failure_handover_preparation_failure;
	Counter &s1ap_handover_cancel_ack_handover_cancel_ack;
	Counter &s1ap_paging_request_paging_request;
	Counter &s1ap_ics_request_paging_ics_request_paging;
	Counter &s1ap_detach_accept_detach_accept;
	Counter &s11_create_session_request_create_session_request;
	Counter &s11_modify_bearer_request_modify_bearer_request;
	Counter &s11_delete_session_request_delete_session_request;
	Counter &s11_downlink_data_notification_ack_downlink_data_notification_ack;
	Counter &s11_release_bearer_request_release_bearer_request;
	Counter &s6a_authentication_info_request_authentication_info_request;
	Counter &s6a_update_location_request_update_location_request;
	Counter &s6a_purge_request_purge_request;

	mme_msg_tx_DynamicMetricObject* add_dynamic(std::string labelk0,std::string labelv0,std::string labelk1,std::string labelv1,std::string labelk, std::string labelv) {
		return new mme_msg_tx_DynamicMetricObject(mme_msg_tx_family,labelk0,labelv0,labelk1,labelv1,labelk, labelv);
 	}
};



class mme_procedures_DynamicMetricObject : public DynamicMetricObject {
	public:
		mme_procedures_DynamicMetricObject(Family<Counter> &mme_procedures_family,std::string labelk0,std::string labelv0,std::string labelk1,std::string labelv1,std::string labelk, std::string labelv):
		 DynamicMetricObject(),
		 counter(mme_procedures_family.Add({{labelk0, labelv0},{labelk1, labelv1},{labelk, labelv}}))
		{
		}
		~mme_procedures_DynamicMetricObject()
		{
		}
		Counter &counter;
};
class mme_procedures_counters {
	public:
	mme_procedures_counters();
	~mme_procedures_counters();
	Family<Counter> &mme_procedures_family;
	Counter &attach_ue_proc_imsi;
	Counter &attach_ue_proc_guti;
	Counter &attach_ue_proc_result_success;
	Counter &attach_ue_proc_result_failure;

	mme_procedures_DynamicMetricObject* add_dynamic(std::string labelk0,std::string labelv0,std::string labelk1,std::string labelv1,std::string labelk, std::string labelv) {
		return new mme_procedures_DynamicMetricObject(mme_procedures_family,labelk0,labelv0,labelk1,labelv1,labelk, labelv);
 	}
};

class mmeStats {
	 public:
		mmeStats();
		~mmeStats() {}
		static mmeStats* Instance(); 
		void mmeStatspromThreadSetup(void);
		void increment(mmeStatsCounter name, std::map<std::string, std::string> labels={}); 
		void decrement(mmeStatsCounter name, std::map<std::string, std::string> labels={}); 
	 public:
		mme_num_ue_gauges *mme_num_ue_m;
		mme_msg_rx_counters *mme_msg_rx_m;
		mme_msg_tx_counters *mme_msg_tx_m;
		mme_procedures_counters *mme_procedures_m;
		std::unordered_map<struct Node, DynamicMetricObject*, hash_fn> metrics_map;
};

#endif /* _INCLUDE_mmeStats_H__ */

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


void mmeStatsSetupPrometheusThread(uint16_t port);

enum class mmeStatsCounter {
	MME_NUM_ACTIVE_SUBSCRIBERS,
	MME_NUM_IDLE_SUBSCRIBERS,
	MME_NUM_PDNS,
	MME_NUM_BEARERS,
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
	MME_PROCEDURES_ATTACH_PROC_SUCCESS,
	MME_PROCEDURES_ATTACH_PROC_FAILURE,
	MME_PROCEDURES_DETACH_PROC_NETWORK_INIT,
	MME_PROCEDURES_DETACH_PROC_UE_INIT,
	MME_PROCEDURES_DETACH_PROC_SUCCESS,
	MME_PROCEDURES_DETACH_PROC_FAILURE,
	MME_PROCEDURES_S1_RELEASE_PROC,
	MME_PROCEDURES_S1_RELEASE_PROC_SUCCESS,
	MME_PROCEDURES_S1_RELEASE_PROC_FAILURE,
	MME_PROCEDURES_SERVICE_REQUEST_PROC_DDN_INIT,
	MME_PROCEDURES_SERVICE_REQUEST_PROC_UE_INIT,
	MME_PROCEDURES_SERVICE_REQUEST_PROC_SUCCESS,
	MME_PROCEDURES_SERVICE_REQUEST_PROC_FAILURE,
	MME_PROCEDURES_TAU_PROC,
	MME_PROCEDURES_TAU_PROC_SUCCESS,
	MME_PROCEDURES_TAU_PROC_FAILURE,
	MME_PROCEDURES_S1_ENB_HANDOVER_PROC,
	MME_PROCEDURES_S1_ENB_HANDOVER_PROC_SUCCESS,
	MME_PROCEDURES_S1_ENB_HANDOVER_PROC_FAILURE,
	MME_PROCEDURES_ERAB_MOD_IND_PROC,
	MME_PROCEDURES_ERAB_MOD_IND_PROC_SUCCESS,
	MME_PROCEDURES_ERAB_MOD_IND_PROC_FAILURE
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


class mme_num_DynamicMetricObject1 : public DynamicMetricObject {
	public:
		mme_num_DynamicMetricObject1(Family<Gauge> &mme_num_family,std::string label_k0, std::string label_v0,std::string label_k1, std::string label_v1,std::string dlabel_k0, std::string dlabel_v0):
		 DynamicMetricObject(),
		 gauge(mme_num_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}}))
		{
		}
		mme_num_DynamicMetricObject1(Family<Gauge> &mme_num_family,std::string label_k0, std::string label_v0,std::string dlabel_k0, std::string dlabel_v0):
		 DynamicMetricObject(),
		 gauge(mme_num_family.Add({{label_k0, label_v0},{dlabel_k0, dlabel_v0}}))
		{
		}
		~mme_num_DynamicMetricObject1()
		{
		}
		Gauge &gauge;
};




class mme_num_DynamicMetricObject2 : public DynamicMetricObject {
	public:
		mme_num_DynamicMetricObject2(Family<Gauge> &mme_num_family,std::string label_k0, std::string label_v0,std::string label_k1, std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1):
		 DynamicMetricObject(),
		 gauge(mme_num_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}, {dlabel_k1, dlabel_v1}}))
		{
		}
		mme_num_DynamicMetricObject2(Family<Gauge> &mme_num_family,std::string label_k0, std::string label_v0,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1):
		 DynamicMetricObject(),
		 gauge(mme_num_family.Add({{label_k0, label_v0},{dlabel_k0, dlabel_v0}, {dlabel_k1, dlabel_v1}}))
		{
		}
		~mme_num_DynamicMetricObject2()
		{
		}
		Gauge &gauge;
};


class mme_num_DynamicMetricObject3 : public DynamicMetricObject {
	public:


		mme_num_DynamicMetricObject3(Family<Gauge> &mme_num_family,std::string label_k0, std::string label_v0,std::string label_k1, std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1, std::string dlabel_k2, std::string dlabel_v2):
		 DynamicMetricObject(),
		 gauge(mme_num_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}, {dlabel_k1, dlabel_v1}, {dlabel_k2, dlabel_v2}}))
		{
		}
		mme_num_DynamicMetricObject3(Family<Gauge> &mme_num_family,std::string label_k0, std::string label_v0,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1, std::string dlabel_k2, std::string dlabel_v2):
		 DynamicMetricObject(),
		 gauge(mme_num_family.Add({{label_k0, label_v0},{dlabel_k0, dlabel_v0}, {dlabel_k1, dlabel_v1}, {dlabel_k2, dlabel_v2}}))
		{
		}
		~mme_num_DynamicMetricObject3()
		{
		}
		Gauge &gauge;
};
class mme_num_gauges {
	public:
	mme_num_gauges();
	~mme_num_gauges();
	Family<Gauge> &mme_num_family;
	Gauge &current__Active_subscribers;
	Gauge &current__Idle_subscribers;
	Gauge &current__pdns;
	Gauge &current__bearers;

	mme_num_DynamicMetricObject1* add_dynamic1(std::string label_k0, std::string label_v0,std::string label_k1, std::string label_v1,std::string dlabel_k0, std::string dlabel_v0) {
		return new mme_num_DynamicMetricObject1(mme_num_family,label_k0, label_v0,label_k1, label_v1,dlabel_k0, dlabel_v0);
 	}

	mme_num_DynamicMetricObject1* add_dynamic1(std::string label_k0, std::string label_v0,std::string dlabel_k0, std::string dlabel_v0) {
		return new mme_num_DynamicMetricObject1(mme_num_family,label_k0, label_v0,dlabel_k0, dlabel_v0);
 	}

	mme_num_DynamicMetricObject2* add_dynamic2(std::string label_k0, std::string label_v0,std::string label_k1, std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1) {
		return new mme_num_DynamicMetricObject2(mme_num_family,label_k0, label_v0,label_k1, label_v1,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1);
 	}

	mme_num_DynamicMetricObject2* add_dynamic2(std::string label_k0, std::string label_v0,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1) {
		return new mme_num_DynamicMetricObject2(mme_num_family,label_k0, label_v0,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1);
 	}

	mme_num_DynamicMetricObject3* add_dynamic3(std::string label_k0, std::string label_v0,std::string label_k1, std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1, std::string dlabel_k2, std::string dlabel_v2) {
		return new mme_num_DynamicMetricObject3(mme_num_family,label_k0, label_v0,label_k1, label_v1,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1, dlabel_k2, dlabel_v2);
 	}

	mme_num_DynamicMetricObject3* add_dynamic3(std::string label_k0, std::string label_v0,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1, std::string dlabel_k2, std::string dlabel_v2) {
		return new mme_num_DynamicMetricObject3(mme_num_family,label_k0, label_v0,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1, dlabel_k2, dlabel_v2);
 	}
};




class mme_msg_rx_DynamicMetricObject1 : public DynamicMetricObject {
	public:
		mme_msg_rx_DynamicMetricObject1(Family<Counter> &mme_msg_rx_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0):
		 DynamicMetricObject(),
		 counter(mme_msg_rx_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}}))
		{
		}
		~mme_msg_rx_DynamicMetricObject1()
		{
		}
		Counter &counter;
};


class mme_msg_rx_DynamicMetricObject2 : public DynamicMetricObject {
	public:
		mme_msg_rx_DynamicMetricObject2(Family<Counter> &mme_msg_rx_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1):
		 DynamicMetricObject(),
		 counter(mme_msg_rx_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}, {dlabel_k1, dlabel_v1}}))
		{
		}
		~mme_msg_rx_DynamicMetricObject2()
		{
		}
		Counter &counter;
};


class mme_msg_rx_DynamicMetricObject3 : public DynamicMetricObject {
	public:
		mme_msg_rx_DynamicMetricObject3(Family<Counter> &mme_msg_rx_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1,std::string dlabel_k2, std::string dlabel_v2):
		 DynamicMetricObject(),
		 counter(mme_msg_rx_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0},{dlabel_k1, dlabel_v1},{dlabel_k2, dlabel_v2}}))
		{
		}
		~mme_msg_rx_DynamicMetricObject3()
		{
		}
		Counter &counter;
};
class mme_msg_rx_counters {
	public:
	mme_msg_rx_counters();
	~mme_msg_rx_counters();
	Family<Counter> &mme_msg_rx_family;
	Counter &mme_msg_rx_nas_attach_request;
	Counter &mme_msg_rx_nas_identity_response;
	Counter &mme_msg_rx_nas_authentication_response;
	Counter &mme_msg_rx_nas_security_mode_response;
	Counter &mme_msg_rx_nas_esm_response;
	Counter &mme_msg_rx_s1ap_init_context_response;
	Counter &mme_msg_rx_nas_attach_complete;
	Counter &mme_msg_rx_nas_detach_request;
	Counter &mme_msg_rx_s1ap_release_request;
	Counter &mme_msg_rx_s1ap_release_complete;
	Counter &mme_msg_rx_s1ap_detach_accept;
	Counter &mme_msg_rx_s1ap_service_request;
	Counter &mme_msg_rx_s1ap_tau_request;
	Counter &mme_msg_rx_s1ap_handover_request_ack;
	Counter &mme_msg_rx_s1ap_handover_notify;
	Counter &mme_msg_rx_s1ap_handover_required;
	Counter &mme_msg_rx_s1ap_enb_status_transfer;
	Counter &mme_msg_rx_s1ap_handover_cancel;
	Counter &mme_msg_rx_s1ap_handover_failure;
	Counter &mme_msg_rx_s1ap_erab_modification_indication;
	Counter &mme_msg_rx_s6a_authentication_information_answer;
	Counter &mme_msg_rx_s6a_update_location_answer;
	Counter &mme_msg_rx_s6a_purge_answer;
	Counter &mme_msg_rx_s6a_cancel_location_request;
	Counter &mme_msg_rx_s11_create_session_response;
	Counter &mme_msg_rx_s11_modify_bearer_response;
	Counter &mme_msg_rx_s11_delete_session_response;
	Counter &mme_msg_rx_s11_release_bearer_response;
	Counter &mme_msg_rx_s11_downlink_notification_indication;

	mme_msg_rx_DynamicMetricObject1* add_dynamic1(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0) {
		return new mme_msg_rx_DynamicMetricObject1(mme_msg_rx_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0);
 	}

	mme_msg_rx_DynamicMetricObject2* add_dynamic2(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1) {
		return new mme_msg_rx_DynamicMetricObject2(mme_msg_rx_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1);
 	}

	mme_msg_rx_DynamicMetricObject3* add_dynamic3(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1,std::string dlabel_k2, std::string dlabel_v2) {
		return new mme_msg_rx_DynamicMetricObject3(mme_msg_rx_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1,dlabel_k2, dlabel_v2);
 	}
};



class mme_msg_tx_DynamicMetricObject1 : public DynamicMetricObject {
	public:
		mme_msg_tx_DynamicMetricObject1(Family<Counter> &mme_msg_tx_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0):
		 DynamicMetricObject(),
		 counter(mme_msg_tx_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}}))
		{
		}
		~mme_msg_tx_DynamicMetricObject1()
		{
		}
		Counter &counter;
};


class mme_msg_tx_DynamicMetricObject2 : public DynamicMetricObject {
	public:
		mme_msg_tx_DynamicMetricObject2(Family<Counter> &mme_msg_tx_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1):
		 DynamicMetricObject(),
		 counter(mme_msg_tx_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}, {dlabel_k1, dlabel_v1}}))
		{
		}
		~mme_msg_tx_DynamicMetricObject2()
		{
		}
		Counter &counter;
};


class mme_msg_tx_DynamicMetricObject3 : public DynamicMetricObject {
	public:
		mme_msg_tx_DynamicMetricObject3(Family<Counter> &mme_msg_tx_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1,std::string dlabel_k2, std::string dlabel_v2):
		 DynamicMetricObject(),
		 counter(mme_msg_tx_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0},{dlabel_k1, dlabel_v1},{dlabel_k2, dlabel_v2}}))
		{
		}
		~mme_msg_tx_DynamicMetricObject3()
		{
		}
		Counter &counter;
};
class mme_msg_tx_counters {
	public:
	mme_msg_tx_counters();
	~mme_msg_tx_counters();
	Family<Counter> &mme_msg_tx_family;
	Counter &mme_msg_tx_nas_identity_request;
	Counter &mme_msg_tx_nas_authentication_request;
	Counter &mme_msg_tx_nas_security_mode_command;
	Counter &mme_msg_tx_nas_esm_information_request;
	Counter &mme_msg_tx_nas_initial_context_request;
	Counter &mme_msg_tx_nas_emm_information_req;
	Counter &mme_msg_tx_nas_attach_reject;
	Counter &mme_msg_tx_nas_service_reject;
	Counter &mme_msg_tx_nas_tau_response;
	Counter &mme_msg_tx_s1ap_erab_modification_indication;
	Counter &mme_msg_tx_nas_network_initiated_detach;
	Counter &mme_msg_tx_s1ap_s1_release_command;
	Counter &mme_msg_tx_s1ap_handover_request;
	Counter &mme_msg_tx_s1ap_handover_command;
	Counter &mme_msg_tx_s1ap_mme_status_transfer;
	Counter &mme_msg_tx_s1ap_handover_preparation_failure;
	Counter &mme_msg_tx_s1ap_handover_cancel_ack;
	Counter &mme_msg_tx_s1ap_paging_request;
	Counter &mme_msg_tx_s1ap_ics_request_paging;
	Counter &mme_msg_tx_s1ap_detach_accept;
	Counter &mme_msg_tx_s11_create_session_request;
	Counter &mme_msg_tx_s11_modify_bearer_request;
	Counter &mme_msg_tx_s11_delete_session_request;
	Counter &mme_msg_tx_s11_downlink_data_notification_ack;
	Counter &mme_msg_tx_s11_release_bearer_request;
	Counter &mme_msg_tx_s6a_authentication_info_request;
	Counter &mme_msg_tx_s6a_update_location_request;
	Counter &mme_msg_tx_s6a_purge_request;

	mme_msg_tx_DynamicMetricObject1* add_dynamic1(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0) {
		return new mme_msg_tx_DynamicMetricObject1(mme_msg_tx_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0);
 	}

	mme_msg_tx_DynamicMetricObject2* add_dynamic2(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1) {
		return new mme_msg_tx_DynamicMetricObject2(mme_msg_tx_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1);
 	}

	mme_msg_tx_DynamicMetricObject3* add_dynamic3(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1,std::string dlabel_k2, std::string dlabel_v2) {
		return new mme_msg_tx_DynamicMetricObject3(mme_msg_tx_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1,dlabel_k2, dlabel_v2);
 	}
};



class mme_procedures_DynamicMetricObject1 : public DynamicMetricObject {
	public:
		mme_procedures_DynamicMetricObject1(Family<Counter> &mme_procedures_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0):
		 DynamicMetricObject(),
		 counter(mme_procedures_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}}))
		{
		}
		mme_procedures_DynamicMetricObject1(Family<Counter> &mme_procedures_family,std::string label_k0,std::string label_v0,std::string dlabel_k0, std::string dlabel_v0):
		 DynamicMetricObject(),
		 counter(mme_procedures_family.Add({{label_k0, label_v0},{dlabel_k0, dlabel_v0}}))
		{
		}
		~mme_procedures_DynamicMetricObject1()
		{
		}
		Counter &counter;
};


class mme_procedures_DynamicMetricObject2 : public DynamicMetricObject {
	public:
		mme_procedures_DynamicMetricObject2(Family<Counter> &mme_procedures_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1):
		 DynamicMetricObject(),
		 counter(mme_procedures_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0}, {dlabel_k1, dlabel_v1}}))
		{
		}
		mme_procedures_DynamicMetricObject2(Family<Counter> &mme_procedures_family,std::string label_k0,std::string label_v0,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1):
		 DynamicMetricObject(),
		 counter(mme_procedures_family.Add({{label_k0, label_v0},{dlabel_k0, dlabel_v0}, {dlabel_k1, dlabel_v1}}))
		{
		}
		~mme_procedures_DynamicMetricObject2()
		{
		}
		Counter &counter;
};


class mme_procedures_DynamicMetricObject3 : public DynamicMetricObject {
	public:
		mme_procedures_DynamicMetricObject3(Family<Counter> &mme_procedures_family,std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1,std::string dlabel_k2, std::string dlabel_v2):
		 DynamicMetricObject(),
		 counter(mme_procedures_family.Add({{label_k0, label_v0},{label_k1, label_v1},{dlabel_k0, dlabel_v0},{dlabel_k1, dlabel_v1},{dlabel_k2, dlabel_v2}}))
		{
		}
		mme_procedures_DynamicMetricObject3(Family<Counter> &mme_procedures_family,std::string label_k0,std::string label_v0,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1,std::string dlabel_k2, std::string dlabel_v2):
		 DynamicMetricObject(),
		 counter(mme_procedures_family.Add({{label_k0, label_v0},{dlabel_k0, dlabel_v0},{dlabel_k1, dlabel_v1},{dlabel_k2, dlabel_v2}}))
		{
		}
		~mme_procedures_DynamicMetricObject3()
		{
		}
		Counter &counter;
};
class mme_procedures_counters {
	public:
	mme_procedures_counters();
	~mme_procedures_counters();
	Family<Counter> &mme_procedures_family;
	Counter &mme_procedures_attach_proc_imsi;
	Counter &mme_procedures_attach_proc_guti;
	Counter &mme_procedures_attach_proc_success;
	Counter &mme_procedures_attach_proc_failure;
	Counter &mme_procedures_detach_proc_network_init;
	Counter &mme_procedures_detach_proc_ue_init;
	Counter &mme_procedures_detach_proc_success;
	Counter &mme_procedures_detach_proc_failure;
	Counter &mme_procedures_s1_release_proc;
	Counter &mme_procedures_s1_release_proc_success;
	Counter &mme_procedures_s1_release_proc_failure;
	Counter &mme_procedures_service_request_proc_ddn_init;
	Counter &mme_procedures_service_request_proc_ue_init;
	Counter &mme_procedures_service_request_proc_success;
	Counter &mme_procedures_service_request_proc_failure;
	Counter &mme_procedures_tau_proc;
	Counter &mme_procedures_tau_proc_success;
	Counter &mme_procedures_tau_proc_failure;
	Counter &mme_procedures_s1_enb_handover_proc;
	Counter &mme_procedures_s1_enb_handover_proc_success;
	Counter &mme_procedures_s1_enb_handover_proc_failure;
	Counter &mme_procedures_erab_mod_ind_proc;
	Counter &mme_procedures_erab_mod_ind_proc_success;
	Counter &mme_procedures_erab_mod_ind_proc_failure;

	mme_procedures_DynamicMetricObject1* add_dynamic1(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0) {
		return new mme_procedures_DynamicMetricObject1(mme_procedures_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0);
 	}

	mme_procedures_DynamicMetricObject1* add_dynamic1(std::string label_k0,std::string label_v0,std::string dlabel_k0, std::string dlabel_v0) {
		return new mme_procedures_DynamicMetricObject1(mme_procedures_family,label_k0,label_v0,dlabel_k0, dlabel_v0);
 	}

	mme_procedures_DynamicMetricObject2* add_dynamic2(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1) {
		return new mme_procedures_DynamicMetricObject2(mme_procedures_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1);
 	}

	mme_procedures_DynamicMetricObject2* add_dynamic2(std::string label_k0,std::string label_v0,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1) {
		return new mme_procedures_DynamicMetricObject2(mme_procedures_family,label_k0,label_v0,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1);
 	}

	mme_procedures_DynamicMetricObject3* add_dynamic3(std::string label_k0,std::string label_v0,std::string label_k1,std::string label_v1,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1,std::string dlabel_k2, std::string dlabel_v2) {
		return new mme_procedures_DynamicMetricObject3(mme_procedures_family,label_k0,label_v0,label_k1,label_v1,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1,dlabel_k2, dlabel_v2);
 	}

	mme_procedures_DynamicMetricObject3* add_dynamic3(std::string label_k0,std::string label_v0,std::string dlabel_k0, std::string dlabel_v0, std::string dlabel_k1, std::string dlabel_v1,std::string dlabel_k2, std::string dlabel_v2) {
		return new mme_procedures_DynamicMetricObject3(mme_procedures_family,label_k0,label_v0,dlabel_k0, dlabel_v0, dlabel_k1, dlabel_v1,dlabel_k2, dlabel_v2);
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
		mme_num_gauges *mme_num_m;
		mme_msg_rx_counters *mme_msg_rx_m;
		mme_msg_tx_counters *mme_msg_tx_m;
		mme_procedures_counters *mme_procedures_m;
		std::unordered_map<struct Node, DynamicMetricObject*, hash_fn> metrics_map;
};

#endif /* _INCLUDE_mmeStats_H__ */

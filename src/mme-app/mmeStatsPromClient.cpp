/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
#include <chrono>
#include <map>
#include <memory>
#include <thread>
#include "mmeStatsPromClient.h"

using namespace prometheus;
std::shared_ptr<Registry> registry;

void mmeStatsSetupPrometheusThread(void)
{
    registry = std::make_shared<Registry>();
    /* Create single instance */ 
    mmeStats::Instance(); 
    Exposer exposer{"0.0.0.0:3081", 1};
    std::string metrics("/metrics");
    exposer.RegisterCollectable(registry, metrics);
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
mmeStats::mmeStats()
{
	 mme_num_ue_m = new mme_num_ue_gauges;
	 mme_msg_rx_m = new mme_msg_rx_counters;
	 mme_msg_tx_m = new mme_msg_tx_counters;
	 mme_procedures_m = new mme_procedures_counters;
}
mmeStats* mmeStats::Instance() 
{
	static mmeStats object;
	return &object; 
}


mme_num_ue_gauges::mme_num_ue_gauges():
mme_num_ue_family(BuildGauge().Name("mme_number_of_ue_attached").Help("Number of UE attached at MME").Labels({{"mme_num_ue","subscribers"}}).Register(*registry)),
current_sub_state_Active(mme_num_ue_family.Add({{"sub_state","Active"}})),
current_sub_state_Idle(mme_num_ue_family.Add({{"sub_state","Idle"}}))
{
}


mme_num_ue_gauges::~mme_num_ue_gauges()
{
}




mme_msg_rx_counters::mme_msg_rx_counters():
mme_msg_rx_family(BuildCounter().Name("number_of_messages_received").Help("Number of messages recceived by mme ").Labels({{"direction","incoming"}}).Register(*registry)),
attach_request_attach_request(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","attach_request"}})),
nas_identity_response_identity_response(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","identity_response"}})),
nas_authentication_response_authentication_response(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","authentication_response"}})),
nas_security_mode_response_security_mode_response(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","security_mode_response"}})),
nas_esm_response_esm_response(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","esm_response"}})),
s1ap_init_context_response_init_context_response(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","init_context_response"}})),
nas_attach_complete_attach_complete(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","attach_complete"}})),
nas_detach_request_detach_request(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","detach_request"}})),
s1ap_release_request_release_request(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","release_request"}})),
s1ap_release_complete_release_complete(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","release_complete"}})),
s1ap_detach_accept_detach_accept(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","detach_accept"}})),
s1ap_service_request_service_request(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","service_request"}})),
s1ap_tau_request_tau_request(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","tau_request"}})),
s1ap_handover_request_ack_handover_request_ack(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_request_ack"}})),
s1ap_handover_notify_handover_notify(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_notify"}})),
s1ap_handover_required_handover_required(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_required"}})),
s1ap_enb_status_transfer_enb_status_transfer(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","enb_status_transfer"}})),
s1ap_handover_cancel_handover_cancel(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_cancel"}})),
s1ap_handover_failure_handover_failure(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_failure"}})),
s1ap_erab_modification_indication_erab_modification_indication(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","erab_modification_indication"}})),
s6a_authentication_information_answer_authentication_information_answer(mme_msg_rx_family.Add({{"interface","s6a"},{"msg_type","authentication_information_answer"}})),
s6a_update_location_answer_update_location_answer(mme_msg_rx_family.Add({{"interface","s6a"},{"msg_type","update_location_answer"}})),
s6a_purge_answer_purge_answer(mme_msg_rx_family.Add({{"interface","s6a"},{"msg_type","purge_answer"}})),
s6a_cancel_location_request_cancel_location_request(mme_msg_rx_family.Add({{"interface","s6a"},{"msg_type","cancel_location_request"}})),
s11_create_session_response_create_session_response(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","create_session_response"}})),
s11_modify_bearer_response_modify_bearer_response(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","modify_bearer_response"}})),
s11_delete_session_response_delete_session_response(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","delete_session_response"}})),
s11_release_bearer_response_release_bearer_response(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","release_bearer_response"}})),
s11_downlink_notification_indication_downlink_notification_indication(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","downlink_notification_indication"}}))
{
}


mme_msg_rx_counters::~mme_msg_rx_counters()
{
}




mme_msg_tx_counters::mme_msg_tx_counters():
mme_msg_tx_family(BuildCounter().Name("number_of_messages_sent").Help("Number of messages sent by mme ").Labels({{"direction","outgoing"}}).Register(*registry)),
nas_identity_request_identity_request(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","identity_request"}})),
nas_authentication_request_authentication_request(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","authentication_request"}})),
nas_security_mode_command_security_mode_command(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","security_mode_command"}})),
nas_esm_information_request_esm_information_request(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","esm_information_request"}})),
nas_initial_context_request_initial_context_request(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","initial_context_request"}})),
nas_emm_information_req_emm_information_req(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","emm_information_req"}})),
nas_attach_reject_attach_reject(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","attach_reject"}})),
nas_service_reject_service_reject(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","service_reject"}})),
nas_tau_response_tau_response(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","tau_response"}})),
s1ap_erab_modification_erab_modification_indication(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","erab_modification_indication"}})),
nas_network_initiated_detach_network_initiated_detach(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","network_initiated_detach"}})),
s1ap_s1_release_command_s1_release_command(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","s1_release_command"}})),
s1ap_handover_request_handover_request(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","handover_request"}})),
s1ap_handover_command_handover_command(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","handover_command"}})),
s1ap_mme_status_transfer_mme_status_transfer(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","mme_status_transfer"}})),
s1ap_handover_preparation_failure_handover_preparation_failure(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","handover_preparation_failure"}})),
s1ap_handover_cancel_ack_handover_cancel_ack(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","handover_cancel_ack"}})),
s1ap_paging_request_paging_request(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","paging_request"}})),
s1ap_ics_request_paging_ics_request_paging(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","ics_request_paging"}})),
s1ap_detach_accept_detach_accept(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","detach_accept"}})),
s11_create_session_request_create_session_request(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","create_session_request"}})),
s11_modify_bearer_request_modify_bearer_request(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","modify_bearer_request"}})),
s11_delete_session_request_delete_session_request(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","delete_session_request"}})),
s11_downlink_data_notification_ack_downlink_data_notification_ack(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","downlink_data_notification_ack"}})),
s11_release_bearer_request_release_bearer_request(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","release_bearer_request"}})),
s6a_authentication_info_request_authentication_info_request(mme_msg_tx_family.Add({{"interface","s6a"},{"msg_type","authentication_info_request"}})),
s6a_update_location_request_update_location_request(mme_msg_tx_family.Add({{"interface","s6a"},{"msg_type","update_location_request"}})),
s6a_purge_request_purge_request(mme_msg_tx_family.Add({{"interface","s6a"},{"msg_type","purge_request"}}))
{
}


mme_msg_tx_counters::~mme_msg_tx_counters()
{
}




mme_procedures_counters::mme_procedures_counters():
mme_procedures_family(BuildCounter().Name("number_of_procedures").Help("Number of procedures executed/started by mme ").Labels({{"mme-app","procedure"}}).Register(*registry)),
attach_ue_proc_imsi(mme_procedures_family.Add({{"procedure","attach_proc"},{"attach_type","imsi"}})),
attach_ue_proc_guti(mme_procedures_family.Add({{"procedure","attach_proc"},{"attach_type","guti"}})),
attach_ue_proc_result_success(mme_procedures_family.Add({{"procedure","attach_proc_result"},{"proc_result","success"}})),
attach_ue_proc_result_failure(mme_procedures_family.Add({{"procedure","attach_proc_result"},{"proc_result","failure"}})),
detach_ue_proc_network_init(mme_procedures_family.Add({{"procedure","detach_proc"},{"detach_type","network_init"}})),
detach_ue_proc_ue_init(mme_procedures_family.Add({{"procedure","detach_proc"},{"detach_type","ue_init"}})),
detach_ue_proc_result_success(mme_procedures_family.Add({{"procedure","detach_proc_result"},{"proc_result","success"}})),
detach_ue_proc_result_failure(mme_procedures_family.Add({{"procedure","detach_proc_result"},{"proc_result","failure"}})),
s1_release_proc_s1_release(mme_procedures_family.Add({{"procedure","s1_release"}})),
s1_release_proc_result_success(mme_procedures_family.Add({{"procedure","s1_release_proc_result"},{"proc_result","success"}})),
s1_release_proc_result_failure(mme_procedures_family.Add({{"procedure","s1_release_proc_result"},{"proc_result","failure"}})),
service_request_proc_ddn_init(mme_procedures_family.Add({{"procedure","service_request_proc"},{"init_by","ddn_init"}})),
service_request_proc_ue_init(mme_procedures_family.Add({{"procedure","service_request_proc"},{"init_by","ue_init"}})),
service_request_proc_result_success(mme_procedures_family.Add({{"procedure","service_request_proc_result"},{"proc_result","success"}})),
service_request_proc_result_failure(mme_procedures_family.Add({{"procedure","service_request_proc_result"},{"proc_result","failure"}})),
tau_proc_tau_proc(mme_procedures_family.Add({{"procedure","tau_proc"}})),
tau_proc_result_success(mme_procedures_family.Add({{"procedure","tau_proc_result"},{"proc_result","success"}})),
tau_proc_result_failure(mme_procedures_family.Add({{"procedure","tau_proc_result"},{"proc_result","failure"}})),
s1_enb_handover_proc_s1_enb_handiver_proc(mme_procedures_family.Add({{"procedure","s1_enb_handiver_proc"}})),
s1_enb_handover_proc_result_success(mme_procedures_family.Add({{"procedure","s1_enb_handover_proc_result"},{"proc_result","success"}})),
s1_enb_handover_proc_result_failure(mme_procedures_family.Add({{"procedure","s1_enb_handover_proc_result"},{"proc_result","failure"}})),
erab_mod_ind_proc_erab_mod_ind_proc(mme_procedures_family.Add({{"procedure","erab_mod_ind_proc"}})),
erab_mod_ind_proc_result_success(mme_procedures_family.Add({{"procedure","erab_mod_ind_proc_result"},{"proc_result","success"}})),
erab_mod_ind_proc_result_failure(mme_procedures_family.Add({{"procedure","erab_mod_ind_proc_result"},{"proc_result","failure"}}))
{
}


mme_procedures_counters::~mme_procedures_counters()
{
}




void mmeStats::increment(mmeStatsCounter name,std::map<std::string,std::string> labels)
{
	switch(name) {
	case mmeStatsCounter::MME_NUM_UE_SUB_STATE_ACTIVE:
	{
		mme_num_ue_m->current_sub_state_Active.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_ue_DynamicMetricObject *obj = static_cast<mme_num_ue_DynamicMetricObject *>(it1->second);
		    obj->gauge.Increment();
		} else {
		    mme_num_ue_DynamicMetricObject *obj = mme_num_ue_m->add_dynamic("sub_state","Active",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_NUM_UE_SUB_STATE_IDLE:
	{
		mme_num_ue_m->current_sub_state_Idle.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_ue_DynamicMetricObject *obj = static_cast<mme_num_ue_DynamicMetricObject *>(it1->second);
		    obj->gauge.Increment();
		} else {
		    mme_num_ue_DynamicMetricObject *obj = mme_num_ue_m->add_dynamic("sub_state","Idle",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_NAS_ATTACH_REQUEST:
	{
		mme_msg_rx_m->attach_request_attach_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","nas","msg_type","attach_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_NAS_IDENTITY_RESPONSE:
	{
		mme_msg_rx_m->nas_identity_response_identity_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","nas","msg_type","identity_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_NAS_AUTHENTICATION_RESPONSE:
	{
		mme_msg_rx_m->nas_authentication_response_authentication_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","nas","msg_type","authentication_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE:
	{
		mme_msg_rx_m->nas_security_mode_response_security_mode_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","nas","msg_type","security_mode_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_NAS_ESM_RESPONSE:
	{
		mme_msg_rx_m->nas_esm_response_esm_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","nas","msg_type","esm_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_INIT_CONTEXT_RESPONSE:
	{
		mme_msg_rx_m->s1ap_init_context_response_init_context_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","init_context_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_NAS_ATTACH_COMPLETE:
	{
		mme_msg_rx_m->nas_attach_complete_attach_complete.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","nas","msg_type","attach_complete",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_NAS_DETACH_REQUEST:
	{
		mme_msg_rx_m->nas_detach_request_detach_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","nas","msg_type","detach_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_RELEASE_REQUEST:
	{
		mme_msg_rx_m->s1ap_release_request_release_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","release_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_RELEASE_COMPLETE:
	{
		mme_msg_rx_m->s1ap_release_complete_release_complete.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","release_complete",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_DETACH_ACCEPT:
	{
		mme_msg_rx_m->s1ap_detach_accept_detach_accept.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","detach_accept",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_SERVICE_REQUEST:
	{
		mme_msg_rx_m->s1ap_service_request_service_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","service_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_TAU_REQUEST:
	{
		mme_msg_rx_m->s1ap_tau_request_tau_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","tau_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_REQUEST_ACK:
	{
		mme_msg_rx_m->s1ap_handover_request_ack_handover_request_ack.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","handover_request_ack",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_NOTIFY:
	{
		mme_msg_rx_m->s1ap_handover_notify_handover_notify.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","handover_notify",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_REQUIRED:
	{
		mme_msg_rx_m->s1ap_handover_required_handover_required.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","handover_required",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_ENB_STATUS_TRANSFER:
	{
		mme_msg_rx_m->s1ap_enb_status_transfer_enb_status_transfer.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","enb_status_transfer",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_CANCEL:
	{
		mme_msg_rx_m->s1ap_handover_cancel_handover_cancel.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","handover_cancel",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_HANDOVER_FAILURE:
	{
		mme_msg_rx_m->s1ap_handover_failure_handover_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","handover_failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S1AP_ERAB_MODIFICATION_INDICATION:
	{
		mme_msg_rx_m->s1ap_erab_modification_indication_erab_modification_indication.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s1ap","msg_type","erab_modification_indication",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S6A_AUTHENTICATION_INFORMATION_ANSWER:
	{
		mme_msg_rx_m->s6a_authentication_information_answer_authentication_information_answer.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s6a","msg_type","authentication_information_answer",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S6A_UPDATE_LOCATION_ANSWER:
	{
		mme_msg_rx_m->s6a_update_location_answer_update_location_answer.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s6a","msg_type","update_location_answer",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S6A_PURGE_ANSWER:
	{
		mme_msg_rx_m->s6a_purge_answer_purge_answer.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s6a","msg_type","purge_answer",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S6A_CANCEL_LOCATION_REQUEST:
	{
		mme_msg_rx_m->s6a_cancel_location_request_cancel_location_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s6a","msg_type","cancel_location_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S11_CREATE_SESSION_RESPONSE:
	{
		mme_msg_rx_m->s11_create_session_response_create_session_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s11","msg_type","create_session_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S11_MODIFY_BEARER_RESPONSE:
	{
		mme_msg_rx_m->s11_modify_bearer_response_modify_bearer_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s11","msg_type","modify_bearer_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S11_DELETE_SESSION_RESPONSE:
	{
		mme_msg_rx_m->s11_delete_session_response_delete_session_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s11","msg_type","delete_session_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S11_RELEASE_BEARER_RESPONSE:
	{
		mme_msg_rx_m->s11_release_bearer_response_release_bearer_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s11","msg_type","release_bearer_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_S11_DOWNLINK_NOTIFICATION_INDICATION:
	{
		mme_msg_rx_m->s11_downlink_notification_indication_downlink_notification_indication.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_rx_DynamicMetricObject *obj = static_cast<mme_msg_rx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_rx_DynamicMetricObject *obj = mme_msg_rx_m->add_dynamic("interface","s11","msg_type","downlink_notification_indication",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_IDENTITY_REQUEST:
	{
		mme_msg_tx_m->nas_identity_request_identity_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","identity_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_AUTHENTICATION_REQUEST:
	{
		mme_msg_tx_m->nas_authentication_request_authentication_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","authentication_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_SECURITY_MODE_COMMAND:
	{
		mme_msg_tx_m->nas_security_mode_command_security_mode_command.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","security_mode_command",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_ESM_INFORMATION_REQUEST:
	{
		mme_msg_tx_m->nas_esm_information_request_esm_information_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","esm_information_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_INITIAL_CONTEXT_REQUEST:
	{
		mme_msg_tx_m->nas_initial_context_request_initial_context_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","initial_context_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_EMM_INFORMATION_REQ:
	{
		mme_msg_tx_m->nas_emm_information_req_emm_information_req.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","emm_information_req",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_ATTACH_REJECT:
	{
		mme_msg_tx_m->nas_attach_reject_attach_reject.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","attach_reject",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_SERVICE_REJECT:
	{
		mme_msg_tx_m->nas_service_reject_service_reject.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","service_reject",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_TAU_RESPONSE:
	{
		mme_msg_tx_m->nas_tau_response_tau_response.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","tau_response",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_ERAB_MODIFICATION_INDICATION:
	{
		mme_msg_tx_m->s1ap_erab_modification_erab_modification_indication.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","erab_modification_indication",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_NAS_NETWORK_INITIATED_DETACH:
	{
		mme_msg_tx_m->nas_network_initiated_detach_network_initiated_detach.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","nas","msg_type","network_initiated_detach",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_S1_RELEASE_COMMAND:
	{
		mme_msg_tx_m->s1ap_s1_release_command_s1_release_command.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","s1_release_command",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_HANDOVER_REQUEST:
	{
		mme_msg_tx_m->s1ap_handover_request_handover_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","handover_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_HANDOVER_COMMAND:
	{
		mme_msg_tx_m->s1ap_handover_command_handover_command.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","handover_command",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_MME_STATUS_TRANSFER:
	{
		mme_msg_tx_m->s1ap_mme_status_transfer_mme_status_transfer.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","mme_status_transfer",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_HANDOVER_PREPARATION_FAILURE:
	{
		mme_msg_tx_m->s1ap_handover_preparation_failure_handover_preparation_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","handover_preparation_failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_HANDOVER_CANCEL_ACK:
	{
		mme_msg_tx_m->s1ap_handover_cancel_ack_handover_cancel_ack.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","handover_cancel_ack",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_PAGING_REQUEST:
	{
		mme_msg_tx_m->s1ap_paging_request_paging_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","paging_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_ICS_REQUEST_PAGING:
	{
		mme_msg_tx_m->s1ap_ics_request_paging_ics_request_paging.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","ics_request_paging",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S1AP_DETACH_ACCEPT:
	{
		mme_msg_tx_m->s1ap_detach_accept_detach_accept.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s1ap","msg_type","detach_accept",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S11_CREATE_SESSION_REQUEST:
	{
		mme_msg_tx_m->s11_create_session_request_create_session_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s11","msg_type","create_session_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S11_MODIFY_BEARER_REQUEST:
	{
		mme_msg_tx_m->s11_modify_bearer_request_modify_bearer_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s11","msg_type","modify_bearer_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S11_DELETE_SESSION_REQUEST:
	{
		mme_msg_tx_m->s11_delete_session_request_delete_session_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s11","msg_type","delete_session_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S11_DOWNLINK_DATA_NOTIFICATION_ACK:
	{
		mme_msg_tx_m->s11_downlink_data_notification_ack_downlink_data_notification_ack.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s11","msg_type","downlink_data_notification_ack",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S11_RELEASE_BEARER_REQUEST:
	{
		mme_msg_tx_m->s11_release_bearer_request_release_bearer_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s11","msg_type","release_bearer_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S6A_AUTHENTICATION_INFO_REQUEST:
	{
		mme_msg_tx_m->s6a_authentication_info_request_authentication_info_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s6a","msg_type","authentication_info_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S6A_UPDATE_LOCATION_REQUEST:
	{
		mme_msg_tx_m->s6a_update_location_request_update_location_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s6a","msg_type","update_location_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_TX_S6A_PURGE_REQUEST:
	{
		mme_msg_tx_m->s6a_purge_request_purge_request.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_msg_tx_DynamicMetricObject *obj = static_cast<mme_msg_tx_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_msg_tx_DynamicMetricObject *obj = mme_msg_tx_m->add_dynamic("interface","s6a","msg_type","purge_request",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_IMSI:
	{
		mme_procedures_m->attach_ue_proc_imsi.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","attach_proc","attach_type","imsi",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_GUTI:
	{
		mme_procedures_m->attach_ue_proc_guti.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","attach_proc","attach_type","guti",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_SUCCESS:
	{
		mme_procedures_m->attach_ue_proc_result_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","attach_proc_result","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_FAILURE:
	{
		mme_procedures_m->attach_ue_proc_result_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","attach_proc_result","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_NETWORK_INIT:
	{
		mme_procedures_m->detach_ue_proc_network_init.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","detach_proc","detach_type","network_init",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_UE_INIT:
	{
		mme_procedures_m->detach_ue_proc_ue_init.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","detach_proc","detach_type","ue_init",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_RESULT_SUCCESS:
	{
		mme_procedures_m->detach_ue_proc_result_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","detach_proc_result","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_RESULT_FAILURE:
	{
		mme_procedures_m->detach_ue_proc_result_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","detach_proc_result","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_RELEASE:
	{
		mme_procedures_m->s1_release_proc_s1_release.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","s1_release",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_RELEASE_PROC_RESULT_SUCCESS:
	{
		mme_procedures_m->s1_release_proc_result_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","s1_release_proc_result","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_RELEASE_PROC_RESULT_FAILURE:
	{
		mme_procedures_m->s1_release_proc_result_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","s1_release_proc_result","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_DDN_INIT:
	{
		mme_procedures_m->service_request_proc_ddn_init.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","service_request_proc","init_by","ddn_init",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_UE_INIT:
	{
		mme_procedures_m->service_request_proc_ue_init.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","service_request_proc","init_by","ue_init",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_RESULT_SUCCESS:
	{
		mme_procedures_m->service_request_proc_result_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","service_request_proc_result","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_RESULT_FAILURE:
	{
		mme_procedures_m->service_request_proc_result_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","service_request_proc_result","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_TAU_PROC:
	{
		mme_procedures_m->tau_proc_tau_proc.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","tau_proc",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_TAU_PROC_RESULT_SUCCESS:
	{
		mme_procedures_m->tau_proc_result_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","tau_proc_result","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_TAU_PROC_RESULT_FAILURE:
	{
		mme_procedures_m->tau_proc_result_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","tau_proc_result","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_ENB_HANDIVER_PROC:
	{
		mme_procedures_m->s1_enb_handover_proc_s1_enb_handiver_proc.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","s1_enb_handiver_proc",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_ENB_HANDOVER_PROC_RESULT_SUCCESS:
	{
		mme_procedures_m->s1_enb_handover_proc_result_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","s1_enb_handover_proc_result","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_ENB_HANDOVER_PROC_RESULT_FAILURE:
	{
		mme_procedures_m->s1_enb_handover_proc_result_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","s1_enb_handover_proc_result","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ERAB_MOD_IND_PROC:
	{
		mme_procedures_m->erab_mod_ind_proc_erab_mod_ind_proc.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","erab_mod_ind_proc",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ERAB_MOD_IND_PROC_RESULT_SUCCESS:
	{
		mme_procedures_m->erab_mod_ind_proc_result_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","erab_mod_ind_proc_result","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ERAB_MOD_IND_PROC_RESULT_FAILURE:
	{
		mme_procedures_m->erab_mod_ind_proc_result_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","erab_mod_ind_proc_result","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	default:
		break;
	}
}




void mmeStats::decrement(mmeStatsCounter name,std::map<std::string,std::string> labels)
{
	switch(name) {
	case mmeStatsCounter::MME_NUM_UE_SUB_STATE_ACTIVE:
	{
		mme_num_ue_m->current_sub_state_Active.Decrement();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_ue_DynamicMetricObject *obj = static_cast<mme_num_ue_DynamicMetricObject *>(it1->second);
		    obj->gauge.Decrement();
		} else {
		    mme_num_ue_DynamicMetricObject *obj = mme_num_ue_m->add_dynamic("sub_state","Active",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Decrement();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_NUM_UE_SUB_STATE_IDLE:
	{
		mme_num_ue_m->current_sub_state_Idle.Decrement();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_ue_DynamicMetricObject *obj = static_cast<mme_num_ue_DynamicMetricObject *>(it1->second);
		    obj->gauge.Decrement();
		} else {
		    mme_num_ue_DynamicMetricObject *obj = mme_num_ue_m->add_dynamic("sub_state","Idle",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Decrement();
		}
		}
		break;
	}
	default:
		break;
	}
}


#ifdef MME_TEST_PROMETHEUS 
#include <unistd.h>
int main() {
	std::thread prom(mmeStatsSetupPrometheusThread);
	prom.detach();
	while(1) {
	mmeStats::Instance()->increment(mmeStatsCounter::MME_NUM_UE_SUB_STATE_ACTIVE);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_NUM_UE_SUB_STATE_IDLE);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE, {{"enb","1.1.1.2"}});
	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_AUTHENTICATION_RESPONSE);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_SUCCESS);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_SUCCESS);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_FAILURE);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_FAILURE, {{"failure_reason", "CSRsp_fail"}});
	sleep(1);
	}
}
#endif

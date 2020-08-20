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
	 mme_num_m = new mme_num_gauges;
	 mme_msg_rx_m = new mme_msg_rx_counters;
	 mme_msg_tx_m = new mme_msg_tx_counters;
	 mme_procedures_m = new mme_procedures_counters;
}
mmeStats* mmeStats::Instance() 
{
	static mmeStats object;
	return &object; 
}


mme_num_gauges::mme_num_gauges():
mme_num_family(BuildGauge().Name("mme_number_of_ue_attached").Help("Number of UE attached at MME").Labels({{"mme","num_ue"}}).Register(*registry)),
current__Active_subscribers(mme_num_family.Add({{"sub_state","Active"},{"level","subscribers"}})),
current__Idle_subscribers(mme_num_family.Add({{"sub_state","Idle"},{"level","subscribers"}})),
current__pdns(mme_num_family.Add({{"level","pdns"}})),
current__bearers(mme_num_family.Add({{"level","bearers"}}))
{
}


mme_num_gauges::~mme_num_gauges()
{
}




mme_msg_rx_counters::mme_msg_rx_counters():
mme_msg_rx_family(BuildCounter().Name("number_of_messages_received").Help("Number of messages recceived by mme ").Labels({{"direction","incoming"}}).Register(*registry)),
mme_msg_rx_nas_attach_request(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","attach_request"}})),
mme_msg_rx_nas_identity_response(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","identity_response"}})),
mme_msg_rx_nas_authentication_response(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","authentication_response"}})),
mme_msg_rx_nas_security_mode_response(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","security_mode_response"}})),
mme_msg_rx_nas_esm_response(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","esm_response"}})),
mme_msg_rx_s1ap_init_context_response(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","init_context_response"}})),
mme_msg_rx_nas_attach_complete(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","attach_complete"}})),
mme_msg_rx_nas_detach_request(mme_msg_rx_family.Add({{"interface","nas"},{"msg_type","detach_request"}})),
mme_msg_rx_s1ap_release_request(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","release_request"}})),
mme_msg_rx_s1ap_release_complete(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","release_complete"}})),
mme_msg_rx_s1ap_detach_accept(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","detach_accept"}})),
mme_msg_rx_s1ap_service_request(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","service_request"}})),
mme_msg_rx_s1ap_tau_request(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","tau_request"}})),
mme_msg_rx_s1ap_handover_request_ack(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_request_ack"}})),
mme_msg_rx_s1ap_handover_notify(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_notify"}})),
mme_msg_rx_s1ap_handover_required(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_required"}})),
mme_msg_rx_s1ap_enb_status_transfer(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","enb_status_transfer"}})),
mme_msg_rx_s1ap_handover_cancel(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_cancel"}})),
mme_msg_rx_s1ap_handover_failure(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","handover_failure"}})),
mme_msg_rx_s1ap_erab_modification_indication(mme_msg_rx_family.Add({{"interface","s1ap"},{"msg_type","erab_modification_indication"}})),
mme_msg_rx_s6a_authentication_information_answer(mme_msg_rx_family.Add({{"interface","s6a"},{"msg_type","authentication_information_answer"}})),
mme_msg_rx_s6a_update_location_answer(mme_msg_rx_family.Add({{"interface","s6a"},{"msg_type","update_location_answer"}})),
mme_msg_rx_s6a_purge_answer(mme_msg_rx_family.Add({{"interface","s6a"},{"msg_type","purge_answer"}})),
mme_msg_rx_s6a_cancel_location_request(mme_msg_rx_family.Add({{"interface","s6a"},{"msg_type","cancel_location_request"}})),
mme_msg_rx_s11_create_session_response(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","create_session_response"}})),
mme_msg_rx_s11_modify_bearer_response(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","modify_bearer_response"}})),
mme_msg_rx_s11_delete_session_response(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","delete_session_response"}})),
mme_msg_rx_s11_release_bearer_response(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","release_bearer_response"}})),
mme_msg_rx_s11_downlink_notification_indication(mme_msg_rx_family.Add({{"interface","s11"},{"msg_type","downlink_notification_indication"}}))
{
}


mme_msg_rx_counters::~mme_msg_rx_counters()
{
}




mme_msg_tx_counters::mme_msg_tx_counters():
mme_msg_tx_family(BuildCounter().Name("number_of_messages_sent").Help("Number of messages sent by mme ").Labels({{"direction","outgoing"}}).Register(*registry)),
mme_msg_tx_nas_identity_request(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","identity_request"}})),
mme_msg_tx_nas_authentication_request(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","authentication_request"}})),
mme_msg_tx_nas_security_mode_command(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","security_mode_command"}})),
mme_msg_tx_nas_esm_information_request(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","esm_information_request"}})),
mme_msg_tx_nas_initial_context_request(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","initial_context_request"}})),
mme_msg_tx_nas_emm_information_req(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","emm_information_req"}})),
mme_msg_tx_nas_attach_reject(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","attach_reject"}})),
mme_msg_tx_nas_service_reject(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","service_reject"}})),
mme_msg_tx_nas_tau_response(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","tau_response"}})),
mme_msg_tx_s1ap_erab_modification_indication(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","erab_modification_indication"}})),
mme_msg_tx_nas_network_initiated_detach(mme_msg_tx_family.Add({{"interface","nas"},{"msg_type","network_initiated_detach"}})),
mme_msg_tx_s1ap_s1_release_command(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","s1_release_command"}})),
mme_msg_tx_s1ap_handover_request(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","handover_request"}})),
mme_msg_tx_s1ap_handover_command(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","handover_command"}})),
mme_msg_tx_s1ap_mme_status_transfer(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","mme_status_transfer"}})),
mme_msg_tx_s1ap_handover_preparation_failure(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","handover_preparation_failure"}})),
mme_msg_tx_s1ap_handover_cancel_ack(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","handover_cancel_ack"}})),
mme_msg_tx_s1ap_paging_request(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","paging_request"}})),
mme_msg_tx_s1ap_ics_request_paging(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","ics_request_paging"}})),
mme_msg_tx_s1ap_detach_accept(mme_msg_tx_family.Add({{"interface","s1ap"},{"msg_type","detach_accept"}})),
mme_msg_tx_s11_create_session_request(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","create_session_request"}})),
mme_msg_tx_s11_modify_bearer_request(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","modify_bearer_request"}})),
mme_msg_tx_s11_delete_session_request(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","delete_session_request"}})),
mme_msg_tx_s11_downlink_data_notification_ack(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","downlink_data_notification_ack"}})),
mme_msg_tx_s11_release_bearer_request(mme_msg_tx_family.Add({{"interface","s11"},{"msg_type","release_bearer_request"}})),
mme_msg_tx_s6a_authentication_info_request(mme_msg_tx_family.Add({{"interface","s6a"},{"msg_type","authentication_info_request"}})),
mme_msg_tx_s6a_update_location_request(mme_msg_tx_family.Add({{"interface","s6a"},{"msg_type","update_location_request"}})),
mme_msg_tx_s6a_purge_request(mme_msg_tx_family.Add({{"interface","s6a"},{"msg_type","purge_request"}}))
{
}


mme_msg_tx_counters::~mme_msg_tx_counters()
{
}




mme_procedures_counters::mme_procedures_counters():
mme_procedures_family(BuildCounter().Name("number_of_procedures").Help("Number of procedures executed/started by mme ").Labels({{"mme-app","procedure"}}).Register(*registry)),
mme_procedures_attach_proc_imsi(mme_procedures_family.Add({{"procedure","ATTACH_PROC"},{"attach_type","imsi"}})),
mme_procedures_attach_proc_guti(mme_procedures_family.Add({{"procedure","ATTACH_PROC"},{"attach_type","guti"}})),
mme_procedures_attach_proc_success(mme_procedures_family.Add({{"procedure","ATTACH_PROC"},{"proc_result","success"}})),
mme_procedures_attach_proc_failure(mme_procedures_family.Add({{"procedure","ATTACH_PROC"},{"proc_result","failure"}})),
mme_procedures_detach_proc_network_init(mme_procedures_family.Add({{"procedure","DETACH_PROC"},{"detach_type","network_init"}})),
mme_procedures_detach_proc_ue_init(mme_procedures_family.Add({{"procedure","DETACH_PROC"},{"detach_type","ue_init"}})),
mme_procedures_detach_proc_success(mme_procedures_family.Add({{"procedure","DETACH_PROC"},{"proc_result","success"}})),
mme_procedures_detach_proc_failure(mme_procedures_family.Add({{"procedure","DETACH_PROC"},{"proc_result","failure"}})),
mme_procedures_s1_release_proc(mme_procedures_family.Add({{"procedure","S1_RELEASE_PROC"}})),
mme_procedures_s1_release_proc_success(mme_procedures_family.Add({{"procedure","S1_RELEASE_PROC"},{"proc_result","success"}})),
mme_procedures_s1_release_proc_failure(mme_procedures_family.Add({{"procedure","S1_RELEASE_PROC"},{"proc_result","failure"}})),
mme_procedures_service_request_proc_ddn_init(mme_procedures_family.Add({{"procedure","SERVICE_REQUEST_PROC"},{"init_by","ddn_init"}})),
mme_procedures_service_request_proc_ue_init(mme_procedures_family.Add({{"procedure","SERVICE_REQUEST_PROC"},{"init_by","ue_init"}})),
mme_procedures_service_request_proc_success(mme_procedures_family.Add({{"procedure","SERVICE_REQUEST_PROC"},{"proc_result","success"}})),
mme_procedures_service_request_proc_failure(mme_procedures_family.Add({{"procedure","SERVICE_REQUEST_PROC"},{"proc_result","failure"}})),
mme_procedures_tau_proc(mme_procedures_family.Add({{"procedure","TAU_PROC"}})),
mme_procedures_tau_proc_success(mme_procedures_family.Add({{"procedure","TAU_PROC"},{"proc_result","success"}})),
mme_procedures_tau_proc_failure(mme_procedures_family.Add({{"procedure","TAU_PROC"},{"proc_result","failure"}})),
mme_procedures_s1_enb_handover_proc(mme_procedures_family.Add({{"procedure","S1_ENB_HANDOVER_PROC"}})),
mme_procedures_s1_enb_handover_proc_success(mme_procedures_family.Add({{"procedure","S1_ENB_HANDOVER_PROC"},{"proc_result","success"}})),
mme_procedures_s1_enb_handover_proc_failure(mme_procedures_family.Add({{"procedure","S1_ENB_HANDOVER_PROC"},{"proc_result","failure"}})),
mme_procedures_erab_mod_ind_proc(mme_procedures_family.Add({{"procedure","ERAB_MOD_IND_PROC"}})),
mme_procedures_erab_mod_ind_proc_success(mme_procedures_family.Add({{"procedure","ERAB_MOD_IND_PROC"},{"proc_result","success"}})),
mme_procedures_erab_mod_ind_proc_failure(mme_procedures_family.Add({{"procedure","ERAB_MOD_IND_PROC"},{"proc_result","failure"}}))
{
}


mme_procedures_counters::~mme_procedures_counters()
{
}




void mmeStats::increment(mmeStatsCounter name,std::map<std::string,std::string> labels)
{
	switch(name) {
	case mmeStatsCounter::MME_NUM_ACTIVE_SUBSCRIBERS:
	{
		mme_num_m->current__Active_subscribers.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_DynamicMetricObject *obj = static_cast<mme_num_DynamicMetricObject *>(it1->second);
		    obj->gauge.Increment();
		} else {
		    mme_num_DynamicMetricObject *obj = mme_num_m->add_dynamic("sub_state","Active","level","subscribers",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_NUM_IDLE_SUBSCRIBERS:
	{
		mme_num_m->current__Idle_subscribers.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_DynamicMetricObject *obj = static_cast<mme_num_DynamicMetricObject *>(it1->second);
		    obj->gauge.Increment();
		} else {
		    mme_num_DynamicMetricObject *obj = mme_num_m->add_dynamic("sub_state","Idle","level","subscribers",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_NUM_PDNS:
	{
		mme_num_m->current__pdns.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_DynamicMetricObject *obj = static_cast<mme_num_DynamicMetricObject *>(it1->second);
		    obj->gauge.Increment();
		} else {
		    mme_num_DynamicMetricObject *obj = mme_num_m->add_dynamic("level","pdns",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_NUM_BEARERS:
	{
		mme_num_m->current__bearers.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_DynamicMetricObject *obj = static_cast<mme_num_DynamicMetricObject *>(it1->second);
		    obj->gauge.Increment();
		} else {
		    mme_num_DynamicMetricObject *obj = mme_num_m->add_dynamic("level","bearers",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_MSG_RX_NAS_ATTACH_REQUEST:
	{
		mme_msg_rx_m->mme_msg_rx_nas_attach_request.Increment();
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
		mme_msg_rx_m->mme_msg_rx_nas_identity_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_nas_authentication_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_nas_security_mode_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_nas_esm_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_init_context_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_nas_attach_complete.Increment();
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
		mme_msg_rx_m->mme_msg_rx_nas_detach_request.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_release_request.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_release_complete.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_detach_accept.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_service_request.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_tau_request.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_handover_request_ack.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_handover_notify.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_handover_required.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_enb_status_transfer.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_handover_cancel.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_handover_failure.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s1ap_erab_modification_indication.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s6a_authentication_information_answer.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s6a_update_location_answer.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s6a_purge_answer.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s6a_cancel_location_request.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s11_create_session_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s11_modify_bearer_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s11_delete_session_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s11_release_bearer_response.Increment();
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
		mme_msg_rx_m->mme_msg_rx_s11_downlink_notification_indication.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_identity_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_authentication_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_security_mode_command.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_esm_information_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_initial_context_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_emm_information_req.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_attach_reject.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_service_reject.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_tau_response.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_erab_modification_indication.Increment();
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
		mme_msg_tx_m->mme_msg_tx_nas_network_initiated_detach.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_s1_release_command.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_handover_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_handover_command.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_mme_status_transfer.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_handover_preparation_failure.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_handover_cancel_ack.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_paging_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_ics_request_paging.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s1ap_detach_accept.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s11_create_session_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s11_modify_bearer_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s11_delete_session_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s11_downlink_data_notification_ack.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s11_release_bearer_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s6a_authentication_info_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s6a_update_location_request.Increment();
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
		mme_msg_tx_m->mme_msg_tx_s6a_purge_request.Increment();
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
		mme_procedures_m->mme_procedures_attach_proc_imsi.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","ATTACH_PROC","attach_type","imsi",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_GUTI:
	{
		mme_procedures_m->mme_procedures_attach_proc_guti.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","ATTACH_PROC","attach_type","guti",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_SUCCESS:
	{
		mme_procedures_m->mme_procedures_attach_proc_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","ATTACH_PROC","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_FAILURE:
	{
		mme_procedures_m->mme_procedures_attach_proc_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","ATTACH_PROC","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_NETWORK_INIT:
	{
		mme_procedures_m->mme_procedures_detach_proc_network_init.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","DETACH_PROC","detach_type","network_init",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_UE_INIT:
	{
		mme_procedures_m->mme_procedures_detach_proc_ue_init.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","DETACH_PROC","detach_type","ue_init",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_SUCCESS:
	{
		mme_procedures_m->mme_procedures_detach_proc_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","DETACH_PROC","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_DETACH_PROC_FAILURE:
	{
		mme_procedures_m->mme_procedures_detach_proc_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","DETACH_PROC","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_RELEASE_PROC:
	{
		mme_procedures_m->mme_procedures_s1_release_proc.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","S1_RELEASE_PROC",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_RELEASE_PROC_SUCCESS:
	{
		mme_procedures_m->mme_procedures_s1_release_proc_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","S1_RELEASE_PROC","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_RELEASE_PROC_FAILURE:
	{
		mme_procedures_m->mme_procedures_s1_release_proc_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","S1_RELEASE_PROC","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_DDN_INIT:
	{
		mme_procedures_m->mme_procedures_service_request_proc_ddn_init.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","SERVICE_REQUEST_PROC","init_by","ddn_init",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_UE_INIT:
	{
		mme_procedures_m->mme_procedures_service_request_proc_ue_init.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","SERVICE_REQUEST_PROC","init_by","ue_init",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_SUCCESS:
	{
		mme_procedures_m->mme_procedures_service_request_proc_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","SERVICE_REQUEST_PROC","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_SERVICE_REQUEST_PROC_FAILURE:
	{
		mme_procedures_m->mme_procedures_service_request_proc_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","SERVICE_REQUEST_PROC","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_TAU_PROC:
	{
		mme_procedures_m->mme_procedures_tau_proc.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","TAU_PROC",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_TAU_PROC_SUCCESS:
	{
		mme_procedures_m->mme_procedures_tau_proc_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","TAU_PROC","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_TAU_PROC_FAILURE:
	{
		mme_procedures_m->mme_procedures_tau_proc_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","TAU_PROC","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_ENB_HANDOVER_PROC:
	{
		mme_procedures_m->mme_procedures_s1_enb_handover_proc.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","S1_ENB_HANDOVER_PROC",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_ENB_HANDOVER_PROC_SUCCESS:
	{
		mme_procedures_m->mme_procedures_s1_enb_handover_proc_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","S1_ENB_HANDOVER_PROC","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_S1_ENB_HANDOVER_PROC_FAILURE:
	{
		mme_procedures_m->mme_procedures_s1_enb_handover_proc_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","S1_ENB_HANDOVER_PROC","proc_result","failure",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ERAB_MOD_IND_PROC:
	{
		mme_procedures_m->mme_procedures_erab_mod_ind_proc.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","ERAB_MOD_IND_PROC",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ERAB_MOD_IND_PROC_SUCCESS:
	{
		mme_procedures_m->mme_procedures_erab_mod_ind_proc_success.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","ERAB_MOD_IND_PROC","proc_result","success",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->counter.Increment();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_PROCEDURES_ERAB_MOD_IND_PROC_FAILURE:
	{
		mme_procedures_m->mme_procedures_erab_mod_ind_proc_failure.Increment();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_procedures_DynamicMetricObject *obj = static_cast<mme_procedures_DynamicMetricObject *>(it1->second);
		    obj->counter.Increment();
		} else {
		    mme_procedures_DynamicMetricObject *obj = mme_procedures_m->add_dynamic("procedure","ERAB_MOD_IND_PROC","proc_result","failure",it->first, it->second);
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
	case mmeStatsCounter::MME_NUM_ACTIVE_SUBSCRIBERS:
	{
		mme_num_m->current__Active_subscribers.Decrement();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_DynamicMetricObject *obj = static_cast<mme_num_DynamicMetricObject *>(it1->second);
		    obj->gauge.Decrement();
		} else {
		    mme_num_DynamicMetricObject *obj = mme_num_m->add_dynamic("sub_state","Active","level","subscribers",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Decrement();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_NUM_IDLE_SUBSCRIBERS:
	{
		mme_num_m->current__Idle_subscribers.Decrement();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_DynamicMetricObject *obj = static_cast<mme_num_DynamicMetricObject *>(it1->second);
		    obj->gauge.Decrement();
		} else {
		    mme_num_DynamicMetricObject *obj = mme_num_m->add_dynamic("sub_state","Idle","level","subscribers",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Decrement();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_NUM_PDNS:
	{
		mme_num_m->current__pdns.Decrement();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_DynamicMetricObject *obj = static_cast<mme_num_DynamicMetricObject *>(it1->second);
		    obj->gauge.Decrement();
		} else {
		    mme_num_DynamicMetricObject *obj = mme_num_m->add_dynamic("level","pdns",it->first, it->second);
		    auto p1 = std::make_pair(s1, obj);
		    metrics_map.insert(p1);
		    obj->gauge.Decrement();
		}
		}
		break;
	}
	case mmeStatsCounter::MME_NUM_BEARERS:
	{
		mme_num_m->current__bearers.Decrement();
		for(auto it = labels.begin(); it != labels.end(); it++) {
		std::cout<<"label - ("<<it->first<<","<<it->second<<")"<<std::endl;
		struct Node s1 = {name, it->first, it->second};
		auto it1 = metrics_map.find(s1);
		if(it1 != metrics_map.end()) {
		    mme_num_DynamicMetricObject *obj = static_cast<mme_num_DynamicMetricObject *>(it1->second);
		    obj->gauge.Decrement();
		} else {
		    mme_num_DynamicMetricObject *obj = mme_num_m->add_dynamic("level","bearers",it->first, it->second);
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


#ifdef TEST_PROMETHEUS 
#include <unistd.h>
int main() {
	std::thread prom(mmeStatsSetupPrometheusThread);
	prom.detach();
	while(1) {
	mmeStats::Instance()->increment(mmeStatsCounter::MME_NUM_ACTIVE_SUBSCRIBERS);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_NUM_IDLE_SUBSCRIBERS);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE, {{"enb","1.1.1.2"}});
	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_AUTHENTICATION_RESPONSE);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_SUCCESS);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_SUCCESS);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_FAILURE);
	mmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_FAILURE, {{"failure_reason", "CSRsp_fail"}});
	sleep(1);
	}
}
#endif

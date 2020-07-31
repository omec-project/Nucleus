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
#include "promClient.h"

using namespace prometheus;

std::shared_ptr<Registry> registry;

void promThreadSetup(void)
{
    registry = std::make_shared<Registry>();

    /* Create single instance */
    statistics::Instance();

    Exposer exposer{"0.0.0.0:3081", 1};
    std::string debugMetrics("/metrics");
    exposer.RegisterCollectable(registry, debugMetrics);
    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

statistics::statistics()
{
    ue_gauge_m = new UE_Gauge;
    rx_s11_msg_m = new Rx_S11_Message_counters;
    rx_s6_msg_m = new Rx_S6_Message_counters;
    rx_s1ap_msg_m = new Rx_S1ap_Message_counters;
    tx_s11_msg_m = new Tx_S11_Message_counters;
    tx_s6_msg_m = new Tx_S6_Message_counters;
    tx_s1ap_msg_m = new Tx_S1ap_Message_counters;
}

statistics* statistics::Instance()
{
    static statistics object;
    return &object;
}

void statistics::ue_detached(mme::UEContext *ue)
{
    ue_gauge_m->current_sub.Decrement();
}

void statistics::ue_attached(mme::UEContext *ue)
{
   ue_gauge_m->current_sub.Increment();
}

void statistics::Increment_s11_msg_rx_stats(msg_type_t msg_type)
{
    rx_s11_msg_m->Increment(msg_type);
}

void statistics::Increment_s6_msg_rx_stats(msg_type_t msg_type)
{
    rx_s6_msg_m->Increment(msg_type);
}

void statistics::Increment_s1ap_msg_rx_stats(msg_type_t msg_type)
{
    rx_s1ap_msg_m->Increment(msg_type);
}

void statistics::Increment_s11_msg_tx_stats(msg_type_t msg_type)
{
    tx_s11_msg_m->Increment(msg_type);
}

void statistics::Increment_s6_msg_tx_stats(msg_type_t msg_type)
{
    tx_s6_msg_m->Increment(msg_type);
}

void statistics::Increment_s1ap_msg_tx_stats(msg_type_t msg_type)
{
    tx_s1ap_msg_m->Increment(msg_type);
}

UE_Gauge::UE_Gauge():
            ue_info(BuildGauge().Name("number_of_ue_attached").Help("Number of UE attached").Labels({{"current_imsis", "current_subscribers"}}).Register(*registry)),
            current_sub(ue_info.Add({{"imsi", "value"}}))
{
}

UE_Gauge::~UE_Gauge()
{
}

Rx_S1ap_Message_counters::Rx_S1ap_Message_counters():
            rx_s1ap_msg_family(BuildCounter().Name("number_of_s1ap_messages_received").Help("Number of message received by mme-app ").Labels({{"rx_messages", "tipc"}}).Register(*registry)),
            attach_req_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "attach_request"}})),
            id_rsp_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "identity_response"}})),
            auth_rsp_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "auth_response"}})),
            sec_mod_complete_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "sec_mode_complete"}})),
            esm_rsp_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "esm_response"}})),
            init_ctxt_rsp_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "init_context_rsp_response"}})),
            attach_complete_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "attach_complete"}})),
            detach_req_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "detach_request"}})),
            s1_release_req_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "s1_release_request"}})),
            s1_release_comp_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "s1_release_complete"}})),
            detach_accept_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "detach_accept"}})),
            service_req_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "service_request"}})),
            tau_req_counter(rx_s1ap_msg_family.Add({{"interface","nas"},{"message_type", "tau_request"}})),
            handover_req_ack_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "handover_req_ack"}})),
            handover_notify_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "handover_notify"}})),
            handover_required_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "handover_required"}})),
            enb_status_transfer_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "enb_status_transfer"}})),
            handover_cancel_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "handover_cancel"}})),
            handover_failure_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "handover_failure"}})),
            erab_mod_ind_counter(rx_s1ap_msg_family.Add({{"interface","s1-app"},{"message_type", "erab_mod_ind"}})),
            unknown_s1ap_msg_counter(rx_s1ap_msg_family.Add({{"interface","s6a"},{"message_type", "unknown_message"}}))
{
}

Rx_S1ap_Message_counters::~Rx_S1ap_Message_counters()
{
}

void 
Rx_S1ap_Message_counters::Increment(msg_type_t msg_type)
{
    switch(msg_type)
    {
		case msg_type_t::attach_request:
            attach_req_counter.Increment();
			break;

		case msg_type_t::id_response:
            id_rsp_counter.Increment();
			break;

		case msg_type_t::auth_response:
            auth_rsp_counter.Increment();
			break;

		case msg_type_t::sec_mode_complete:
            sec_mod_complete_counter.Increment();
			break;

		case msg_type_t::esm_info_response:
            esm_rsp_counter.Increment();
			break;

		case msg_type_t::init_ctxt_response:
            init_ctxt_rsp_counter.Increment();
			break;

		case msg_type_t::attach_complete:
            attach_complete_counter.Increment();
			break;
                
		case msg_type_t::detach_request:
            detach_req_counter.Increment();
			break;
					
		case msg_type_t::s1_release_request:
            s1_release_req_counter.Increment();
			break;
			
		case msg_type_t::s1_release_complete:
            s1_release_comp_counter.Increment();
			break;
		
		case msg_type_t::detach_accept_from_ue:
            detach_accept_counter.Increment();
			break;

		case  msg_type_t::service_request:
            service_req_counter.Increment();
		    break;
					
		case msg_type_t::tau_request:
            tau_req_counter.Increment();
			break;

		case msg_type_t::handover_request_acknowledge:
            handover_req_ack_counter.Increment();
		    break;

		case msg_type_t::handover_notify:
            handover_notify_counter.Increment();
		    break;

		case msg_type_t::handover_required:
            handover_required_counter.Increment();
		    break;
		
		case msg_type_t::enb_status_transfer:
            enb_status_transfer_counter.Increment();
		    break;

		case msg_type_t::handover_cancel:
            handover_cancel_counter.Increment();
		    break;

		case msg_type_t::handover_failure:
            handover_failure_counter.Increment();
		    break;
            
		case msg_type_t::erab_mod_indication:
            erab_mod_ind_counter.Increment();
			break;

        default:
            unknown_s1ap_msg_counter.Increment();
            break;
    }
}

Tx_S1ap_Message_counters::Tx_S1ap_Message_counters():
tx_S1ap_msg_family(BuildCounter().Name("number_of_s1ap_messages_sent").Help("Number of s1ap message sent by mme-app ").Labels({{"tx_messages","tipc"},{"interface","s1ap"}}).Register(*registry)),
   identity_req_counter(tx_S1ap_msg_family.Add({{"message_type", "identity_request"}})),
   authentication_req_counter(tx_S1ap_msg_family.Add({{"message_type", "authentication_request"}})),
   sec_mode_command_counter(tx_S1ap_msg_family.Add({{"message_type", "security_mode_command"}})),
   esm_info_req_counter(tx_S1ap_msg_family.Add({{"message_type", "esm_information_request"}})),
   initial_ctx_req_counter(tx_S1ap_msg_family.Add({{"message_type", "initial_context_request"}})),
   emm_info_req_counter(tx_S1ap_msg_family.Add({{"message_type", "emm_information_req"}})),
   attach_reject_counter(tx_S1ap_msg_family.Add({{"message_type", "attach_reject"}})),
   service_reject_counter(tx_S1ap_msg_family.Add({{"message_type", "service_reject"}})),
   tau_response_counter(tx_S1ap_msg_family.Add({{"message_type", "tau_response"}})),
   erab_mod_confirm_counter(tx_S1ap_msg_family.Add({{"message_type", "erab_modification"}})),
   ni_init_detach_req_counter(tx_S1ap_msg_family.Add({{"message_type", "network_initiated_detach"}})),
   s1_release_command_counter(tx_S1ap_msg_family.Add({{"message_type", "s1_release_command"}})),
   handover_req_counter(tx_S1ap_msg_family.Add({{"message_type", "handover_request"}})),
   handover_command_counter(tx_S1ap_msg_family.Add({{"message_type", "handover_command"}})),
   mme_status_transfer_counter(tx_S1ap_msg_family.Add({{"message_type", "mme_status_transfer"}})),
   handover_preparation_failure_counter(tx_S1ap_msg_family.Add({{"message_type", "handover_preparation_failure"}})),
   handover_cancel_ack_counter(tx_S1ap_msg_family.Add({{"message_type", "handover_cancel_ack"}})),
   paging_req_counter(tx_S1ap_msg_family.Add({{"message_type", "paging_request"}})),
   ics_req_paging_counter(tx_S1ap_msg_family.Add({{"message_type", "ics_request_paging"}})),
   detach_accept_counter(tx_S1ap_msg_family.Add({{"message_type", "detach_accept"}}))
{
}

Tx_S1ap_Message_counters::~Tx_S1ap_Message_counters()
{
}

void
Tx_S1ap_Message_counters::Increment(msg_type_t msg_type)
{
    switch(msg_type)
    {
        case msg_type_t::id_request:
            identity_req_counter.Increment();
            break;
        case msg_type_t::auth_request:
            authentication_req_counter.Increment();
            break;
        case msg_type_t::sec_mode_command:
            sec_mode_command_counter.Increment();
            break;
        case msg_type_t::esm_info_request:
            esm_info_req_counter.Increment();
            break;
        case msg_type_t::init_ctxt_request:
            initial_ctx_req_counter.Increment();
            break;
        case msg_type_t::emm_info_request:
            emm_info_req_counter.Increment();
            break;
        case msg_type_t::attach_reject:
            attach_reject_counter.Increment();
            break;
        case msg_type_t::service_reject:
            service_reject_counter.Increment();
            break;
        case msg_type_t::tau_response:
            tau_response_counter.Increment();
            break;
        case msg_type_t::erab_mod_confirmation:
            erab_mod_confirm_counter.Increment();
            break;
        case msg_type_t::ni_detach_request:
            ni_init_detach_req_counter.Increment();
            break;
        case msg_type_t::s1_release_command:
            s1_release_command_counter.Increment();
            break;
        case msg_type_t::handover_request:
            handover_req_counter.Increment();
            break;
        case msg_type_t::handover_command:
            handover_command_counter.Increment();
            break;
        case msg_type_t::mme_status_transfer:
            mme_status_transfer_counter.Increment();
            break;
        case msg_type_t::handover_preparation_failure:
            handover_preparation_failure_counter.Increment();
            break;
        case msg_type_t::handover_cancel_ack:
            handover_cancel_ack_counter.Increment();
            break;
        case msg_type_t::paging_request:
            paging_req_counter.Increment();
            break;
        case msg_type_t::ics_req_paging:
            ics_req_paging_counter.Increment();
            break;
        case msg_type_t::detach_accept:
            detach_accept_counter.Increment();
            break;
        default:
            break;
    }
}

Rx_S6_Message_counters::Rx_S6_Message_counters():
            rx_s6_msg_family(BuildCounter().Name("number_of_s6_messages_received").Help("Number of message received by mme-app ").Labels({{"rx_messages", "tipc"}}).Register(*registry)),
            auth_info_answer_counter(rx_s6_msg_family.Add({{"interface","s6a"},{"message_type", "authentication_information_answer"}})),
            update_location_answer_counter(rx_s6_msg_family.Add({{"interface","s6a"},{"message_type", "update_location_answer"}})),
            purge_answer_counter(rx_s6_msg_family.Add({{"interface","s6a"},{"message_type", "purge_answer"}})),
            cancel_location_req_counter(rx_s6_msg_family.Add({{"interface","s6a"},{"message_type", "cancel_location_request"}})),
            unknown_s6_msg_counter(rx_s6_msg_family.Add({{"interface","s6a"},{"message_type", "unknown_message"}}))
{
}

Rx_S6_Message_counters::~Rx_S6_Message_counters()
{
}

void
Rx_S6_Message_counters::Increment(msg_type_t msg_type)
{
    switch(msg_type)
    {
		case msg_type_t::auth_info_answer:
            auth_info_answer_counter.Increment();
			break;

		case msg_type_t::update_loc_answer:
            update_location_answer_counter.Increment();
			break;

		case msg_type_t::purge_answser:
            purge_answer_counter.Increment();
			break;
		
		case msg_type_t::cancel_location_request:
            cancel_location_req_counter.Increment();
			break;

        default:
            unknown_s6_msg_counter.Increment();
            break;
    }
}

Tx_S6_Message_counters::Tx_S6_Message_counters():
 tx_S6_msg_family(BuildCounter().Name("number_of_s6_messages_sent").Help("Number of s6 message sent by mme-app ").Labels({{"tx_messages", "tipc"}, {"interface", "s6"}}).Register(*registry)),
 auth_info_req_counter(tx_S6_msg_family.Add({{"message_type", "authentication_info_request"}})),
 update_loc_req_counter(tx_S6_msg_family.Add({{"message_type", "update_location_request"}})),
 purge_request_counter(tx_S6_msg_family.Add({{"message_type", "purge_request"}}))
{
}

Tx_S6_Message_counters::~Tx_S6_Message_counters()
{
}

void
Tx_S6_Message_counters::Increment(msg_type_t msg_type)
{
    switch(msg_type)
    {
        case msg_type_t::auth_info_request:
            auth_info_req_counter.Increment();
            break;
        case msg_type_t::update_loc_request:
            update_loc_req_counter.Increment();
            break;
        case msg_type_t::purge_request:
            purge_request_counter.Increment();
            break;
        default:
            break;
    }
}

Rx_S11_Message_counters::Rx_S11_Message_counters():
            rx_s11_msg_family(BuildCounter().Name("number_of_s11_messages_received").Help("Number of message received by mme-app ").Labels({{"rx_messages", "tipc"}}).Register(*registry)),
            create_session_rsp_counter(rx_s11_msg_family.Add({{"interface", "s11"},{"message_type", "create_session_response"}})),
            modify_bearer_rsp_counter(rx_s11_msg_family.Add({{"interface","s11"},{"message_type", "modify_bearer_response"}})),
            delete_session_rsp_counter(rx_s11_msg_family.Add({{"interface","s11"},{"message_type", "delete_session_response"}})),
            release_bearer_rsp_counter(rx_s11_msg_family.Add({{"interface","s11"},{"message_type", "release_bearer_response"}})),
            downlink_data_notification_counter(rx_s11_msg_family.Add({{"interface","s11"},{"message_type", "downlink_notification_indication"}})),
            unknown_s11_msg_counter(rx_s11_msg_family.Add({{"interface","s11"},{"message_type", "unknown_message"}}))
{
}

Rx_S11_Message_counters::~Rx_S11_Message_counters()
{
}

void
Rx_S11_Message_counters::Increment(msg_type_t msg_type)
{
    switch(msg_type)
    {
		case msg_type_t::create_session_response:
            create_session_rsp_counter.Increment();
			break;

		case msg_type_t::modify_bearer_response:
            modify_bearer_rsp_counter.Increment();
			break;

		case msg_type_t::delete_session_response:
            delete_session_rsp_counter.Increment();
			break;
			
		case msg_type_t::release_bearer_response:
            release_bearer_rsp_counter.Increment();
			break;
		
		case msg_type_t::downlink_data_notification:
            downlink_data_notification_counter.Increment();
			break;

		default:
            unknown_s11_msg_counter.Increment();
            break;
    }
}

Tx_S11_Message_counters::Tx_S11_Message_counters():
 tx_S11_msg_family(BuildCounter().Name("number_of_s11_messages_sent").Help("Number of s11 message sent by mme-app ").Labels({{"tx_messages", "tipc"},{"interface","s11"}}).Register(*registry)),
 create_session_req_counter(tx_S11_msg_family.Add({{"message_type", "create_session_request"}})),
 modify_bearer_req_counter(tx_S11_msg_family.Add({{"message_type", "modify_bearer_request"}})),
 delete_session_req_counter(tx_S11_msg_family.Add({{"message_type", "delete_session_request"}})),
 ddn_ack_counter(tx_S11_msg_family.Add({{"message_type", "downlink_data_notification_ack"}})),
 release_bearer_req_counter(tx_S11_msg_family.Add({{"message_type", "release_bearer_request"}}))

{
}

Tx_S11_Message_counters::~Tx_S11_Message_counters()
{
}

void
Tx_S11_Message_counters::Increment(msg_type_t msg_type)
{
    switch(msg_type)
    {
        case msg_type_t::create_session_request:
            create_session_req_counter.Increment();
            break;
        case msg_type_t::modify_bearer_request:
            modify_bearer_req_counter.Increment();
            break;
        case msg_type_t::delete_session_request:
            delete_session_req_counter.Increment();
            break;
        case msg_type_t::ddn_acknowledgement:
            ddn_ack_counter.Increment();
            break;
        case msg_type_t::release_bearer_request:
            release_bearer_req_counter.Increment();
            break;
        default:
            break;
    }
}



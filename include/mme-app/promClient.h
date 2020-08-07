/*
 * Copyright 2010-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _INCLUDE_MME_APP_PROM_CLIENT_H
#define _INCLUDE_MME_APP_PROM_CLIENT_H

#include <prometheus/gauge.h>
#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include "contextManager/dataBlocks.h"
#include "msgType.h"

using namespace prometheus;
extern std::shared_ptr<Registry> registry;


void promThreadSetup(void);

class UE_Gauge {
    public:
        UE_Gauge();
        ~UE_Gauge();
        void Increment(void);
        void Decrement(void);
        Family<Gauge> &ue_info;
        Gauge &current_sub;
};

class Rx_S1ap_Message_counters {
    public:
        Rx_S1ap_Message_counters();
        ~Rx_S1ap_Message_counters();
        void Increment(msg_type_t msg_type);
        Family<Counter> &rx_s1ap_msg_family;
        Counter &attach_req_counter;
        Counter &id_rsp_counter;
        Counter &auth_rsp_counter;
        Counter &sec_mod_complete_counter;
        Counter &esm_rsp_counter;
        Counter &init_ctxt_rsp_counter;
        Counter &attach_complete_counter;
        Counter &detach_req_counter;
        Counter &s1_release_req_counter;
        Counter &s1_release_comp_counter;
        Counter &detach_accept_counter;
        Counter &service_req_counter;
        Counter &tau_req_counter;
        Counter &handover_req_ack_counter;
        Counter &handover_notify_counter;
        Counter &handover_required_counter;
        Counter &enb_status_transfer_counter;
        Counter &handover_cancel_counter;
        Counter &handover_failure_counter;
        Counter &erab_mod_ind_counter;
        Counter &unknown_s1ap_msg_counter;
};

class Rx_S6_Message_counters {
    public:
        Rx_S6_Message_counters();
        ~Rx_S6_Message_counters();
        void Increment(msg_type_t msg_type);
        Family<Counter> &rx_s6_msg_family;

        Counter &auth_info_answer_counter;
        Counter &update_location_answer_counter;
        Counter &purge_answer_counter;
        Counter &cancel_location_req_counter;
        Counter &unknown_s6_msg_counter;
};

class Rx_S11_Message_counters {
    public:
        Rx_S11_Message_counters();
        ~Rx_S11_Message_counters();
        void Increment(msg_type_t msg_type);
        Family<Counter> &rx_s11_msg_family;

        Counter &create_session_rsp_counter; 
        Counter &modify_bearer_rsp_counter; 
        Counter &delete_session_rsp_counter;
        Counter &release_bearer_rsp_counter;
        Counter &downlink_data_notification_counter;
        Counter &unknown_s11_msg_counter;
};

class Tx_S11_Message_counters {
    public:
        Tx_S11_Message_counters();
        ~Tx_S11_Message_counters();
        Family<Counter> &tx_S11_msg_family;
        void Increment(msg_type_t msg_type);
        Counter &create_session_req_counter;
        Counter &modify_bearer_req_counter;
        Counter &delete_session_req_counter;
        Counter &ddn_ack_counter;
        Counter &release_bearer_req_counter;
};

class Tx_S6_Message_counters {
    public:
        Tx_S6_Message_counters();
        ~Tx_S6_Message_counters();
        Family<Counter> &tx_S6_msg_family;
        void Increment(msg_type_t msg_type);
        Counter &auth_info_req_counter;
        Counter &update_loc_req_counter;
        Counter &purge_request_counter;
};

class Tx_S1ap_Message_counters {
    public:
        Tx_S1ap_Message_counters();
        ~Tx_S1ap_Message_counters();
        Family<Counter> &tx_S1ap_msg_family;
        void Increment(msg_type_t msg_type);
        Counter &identity_req_counter;
        Counter &authentication_req_counter;
        Counter &sec_mode_command_counter;
        Counter &esm_info_req_counter;
        Counter &initial_ctx_req_counter;
        Counter &emm_info_req_counter;
        Counter &attach_reject_counter;
        Counter &service_reject_counter;
        Counter &tau_response_counter;
        Counter &erab_mod_confirm_counter;
        Counter &ni_init_detach_req_counter;
        Counter &s1_release_command_counter;
        Counter &handover_req_counter;
        Counter &handover_command_counter;
        Counter &mme_status_transfer_counter; 
        Counter &handover_preparation_failure_counter;
        Counter &handover_cancel_ack_counter;
        Counter &paging_req_counter;
        Counter &ics_req_paging_counter;
        Counter &detach_accept_counter;

};

class statistics {
    public:
        statistics();
        ~statistics() {}
        static statistics* Instance();
        void promThreadSetup(void);
        void ue_attached(mme::UEContext *ue);
        void ue_detached(mme::UEContext *ue);
        void Increment_s11_msg_rx_stats(msg_type_t msg);
        void Increment_s6_msg_rx_stats(msg_type_t msg);
        void Increment_s1ap_msg_rx_stats(msg_type_t msg);
        void Increment_s11_msg_tx_stats(msg_type_t msg);
        void Increment_s6_msg_tx_stats(msg_type_t msg);
        void Increment_s1ap_msg_tx_stats(msg_type_t msg);
    public:
        UE_Gauge                    *ue_gauge_m;
        Rx_S11_Message_counters     *rx_s11_msg_m;
        Rx_S6_Message_counters      *rx_s6_msg_m;
        Rx_S1ap_Message_counters    *rx_s1ap_msg_m;
        Tx_S11_Message_counters     *tx_s11_msg_m;
        Tx_S6_Message_counters      *tx_s6_msg_m;
        Tx_S1ap_Message_counters    *tx_s1ap_msg_m;
};

#endif

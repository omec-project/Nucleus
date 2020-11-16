/*
 * Copyright 2019-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
#ifndef PROCEDURE_STATS_H
#define PROCEDURE_STATS_H
namespace mme
{
        class ProcedureStats
        {
        public:
                static int num_of_air_sent;
                static int num_of_ulr_sent;
                static int num_of_processed_aia;
                static int num_of_processed_ula;
                static int num_of_auth_req_to_ue_sent;
                static int num_of_processed_auth_response;
                static int num_of_sec_mode_cmd_to_ue_sent;
                static int num_of_processed_sec_mode_resp;
                static int num_of_esm_info_req_to_ue_sent;
                static int num_of_handled_esm_info_resp;
                static int num_of_cs_req_to_sgw_sent;
                static int num_of_processed_cs_resp;
                static int num_of_init_ctxt_req_to_ue_sent;
                static int num_of_processed_init_ctxt_resp;
                static int num_of_mb_req_to_sgw_sent;
                static int num_of_processed_attach_cmp_from_ue;
                static int num_of_processed_mb_resp;
                static int num_of_attach_done;
                static int num_of_del_session_req_sent;
                static int num_of_purge_req_sent;
                static int num_of_processed_del_session_resp;
                static int num_of_processed_pur_resp;
                static int num_of_detach_accept_to_ue_sent;
                static int num_of_processed_detach_accept;
                static int num_of_ue_ctxt_release;
                static int num_of_processed_ctxt_rel_resp;
                static int num_of_subscribers_attached;
                static int num_of_rel_access_bearer_req_sent;
                static int num_of_rel_access_bearer_resp_received;
                static int num_of_s1_rel_req_received;
                static int num_of_s1_rel_cmd_sent;
                static int num_of_s1_rel_comp_received;
                static int num_of_clr_received;
                static int num_of_cla_sent;
                static int num_of_detach_req_to_ue_sent;
                static int num_of_detach_accept_from_ue;
                static int total_num_of_subscribers;
                static int num_of_subscribers_detached;
                static int num_of_ddn_received;
                static int num_of_service_request_received;
                static int num_of_ddn_ack_sent;
                static int num_of_tau_response_to_ue_sent;
                static int num_of_service_reject_sent;
                static int num_of_attach_reject_sent;
                static int num_of_emm_info_sent;
                static int num_of_attach_req_received;
                static int num_of_detach_req_received;
                static int num_of_tau_req_received;
                static int num_of_paging_request_sent;
                static int num_of_id_req_sent;
                static int num_of_id_resp_received;
                static int num_of_auth_reject_sent;
                static int num_of_ho_required_received;
                static int num_of_ho_request_to_target_enb_sent;
                static int num_of_ho_request_ack_received;
                static int num_of_ho_command_to_src_enb_sent;
                static int num_of_ho_notify_received;
                static int num_of_ho_complete;
                static int num_of_ho_failure_received;
                static int num_of_ho_prep_failure_sent;
                static int num_of_ho_cancel_received;
                static int num_of_ho_cancel_ack_sent;
                static int num_of_erab_mod_ind_received;
                static int num_of_erab_mod_conf_sent;	
                static int num_of_erab_mod_ind_timeout;
                static int num_of_abort_erab_mod_indication;
                static int num_of_erab_setup_req_sent;
                static int num_of_erab_setup_resp_received;
                static int num_of_create_bearer_req_received;
                static int num_of_create_bearer_resp_sent;
                static int num_of_act_ded_br_ctxt_req_sent;
                static int num_of_ded_bearer_ctxt_acpt_received;
                static int num_of_ded_bearer_ctxt_reject_received;
                static int num_of_abort_ded_activation;
                static int num_of_ded_act_complete;
                static int num_of_create_bearer_proc_complete;
        };
};
#endif

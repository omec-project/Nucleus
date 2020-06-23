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

#include "procedureStats.h"

using namespace mme;

int ProcedureStats::num_of_air_sent = 0;
int ProcedureStats::num_of_ulr_sent = 0;
int ProcedureStats::num_of_processed_aia = 0;
int ProcedureStats::num_of_processed_ula = 0;
int ProcedureStats::num_of_auth_req_to_ue_sent = 0;
int ProcedureStats::num_of_processed_auth_response = 0;
int ProcedureStats::num_of_sec_mode_cmd_to_ue_sent = 0;
int ProcedureStats::num_of_processed_sec_mode_resp = 0;
int ProcedureStats::num_of_esm_info_req_to_ue_sent = 0;
int ProcedureStats::num_of_handled_esm_info_resp = 0;
int ProcedureStats::num_of_cs_req_to_sgw_sent = 0;
int ProcedureStats::num_of_processed_cs_resp = 0;
int ProcedureStats::num_of_init_ctxt_req_to_ue_sent = 0;
int ProcedureStats::num_of_processed_init_ctxt_resp = 0;
int ProcedureStats::num_of_mb_req_to_sgw_sent = 0;
int ProcedureStats::num_of_processed_attach_cmp_from_ue = 0;
int ProcedureStats::num_of_processed_mb_resp = 0;
int ProcedureStats::num_of_attach_done = 0;
int ProcedureStats::num_of_del_session_req_sent = 0;
int ProcedureStats::num_of_purge_req_sent = 0;
int ProcedureStats::num_of_processed_del_session_resp = 0;
int ProcedureStats::num_of_processed_pur_resp = 0;
int ProcedureStats::num_of_detach_accept_to_ue_sent = 0;
int ProcedureStats::num_of_processed_detach_accept = 0;
int ProcedureStats::num_of_ue_ctxt_release = 0;
int ProcedureStats::num_of_processed_ctxt_rel_resp = 0;
int ProcedureStats::num_of_subscribers_attached = 0;
int ProcedureStats::num_of_rel_access_bearer_req_sent = 0;
int ProcedureStats::num_of_rel_access_bearer_resp_received = 0;
int ProcedureStats::num_of_s1_rel_req_received = 0;
int ProcedureStats::num_of_s1_rel_cmd_sent = 0;
int ProcedureStats::num_of_s1_rel_comp_received = 0;
int ProcedureStats::num_of_clr_received = 0;
int ProcedureStats::num_of_cla_sent = 0;
int ProcedureStats::num_of_detach_req_to_ue_sent = 0;
int ProcedureStats::num_of_detach_accept_from_ue = 0;
int ProcedureStats::total_num_of_subscribers = 0;
int ProcedureStats::num_of_subscribers_detached = 0;
int ProcedureStats::num_of_ddn_received = 0;
int ProcedureStats::num_of_service_request_received = 0;
int ProcedureStats::num_of_ddn_ack_sent = 0;
int ProcedureStats::num_of_tau_response_to_ue_sent = 0;
int ProcedureStats::num_of_service_reject_sent = 0;
int ProcedureStats::num_of_attach_reject_sent = 0;
int ProcedureStats::num_of_emm_info_sent = 0;
int ProcedureStats::num_of_attach_req_received = 0;
int ProcedureStats::num_of_detach_req_received = 0;
int ProcedureStats::num_of_tau_req_received = 0;
int ProcedureStats::num_of_paging_request_sent = 0;
int ProcedureStats::num_of_id_req_sent = 0;
int ProcedureStats::num_of_id_resp_received = 0;
int ProcedureStats::num_of_auth_reject_sent = 0;
int ProcedureStats::num_of_ho_required_received = 0;
int ProcedureStats::num_of_ho_request_to_target_enb_sent = 0;
int ProcedureStats::num_of_ho_request_ack_received = 0;
int ProcedureStats::num_of_ho_command_to_src_enb_sent = 0;
int ProcedureStats::num_of_ho_notify_received = 0;
int ProcedureStats::num_of_ho_complete = 0;
int ProcedureStats::num_of_ho_failure_received = 0;
int ProcedureStats::num_of_ho_prep_failure_sent = 0;
int ProcedureStats::num_of_ho_cancel_received = 0;
int ProcedureStats::num_of_ho_cancel_ack_sent = 0;


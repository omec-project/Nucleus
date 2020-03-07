/*
 * Copyright 2019-present, Infosys Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

        };
};
#endif

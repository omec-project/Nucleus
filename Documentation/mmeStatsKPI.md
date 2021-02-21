#License & Copyright

#SPDX-FileCopyrightText: 2020 Open Networking Foundation <info@opennetworking.org>

#SPDX-License-Identifier: LicenseRef-ONF-Member-Only-1.0



# mme_num

|Family type|Family Name|Counter Name| Counter Help|Static Labels|Dynamic Labels|
|----------:|:--------------:|:------------:|:-----:|:-------:|-------:|
|Gauge|mme_number_of_ue_attached|number_of_subscribers|number of subscribers in mme |[{'sub_state': 'Active'}, {'level': 'subscribers'}]|['tac']|
|Gauge|mme_number_of_ue_attached|number_of_subscribers|number of subscribers in mme |[{'sub_state': 'Idle'}, {'level': 'subscribers'}]|['tac']|
|Gauge|mme_number_of_ue_attached|number_of_pdns|number of pdns in mme |[{'level': 'pdns'}]|['tac']|
|Gauge|mme_number_of_ue_attached|number_of_bearers|number of bearers in mme |[{'level': 'bearers'}]|['tac']|


# enb_num

|Family type|Family Name|Counter Name| Counter Help|Static Labels|Dynamic Labels|
|----------:|:--------------:|:------------:|:-----:|:-------:|-------:|
|Gauge|mme_number_of_enb_attached|number_of_enbs|number of enodeb in mme |[{'enb_state': 'Active'}]|['tac']|
|Gauge|mme_number_of_enb_attached|number_of_enbs|number of enodeb in mme |[{'enb_state': 'Active'}]|['enb-name']|
|Gauge|mme_number_of_enb_attached|number_of_enbs|number of enodeb in mme |[{'enb_state': 'Active'}]|['enb-id']|


# mme_msg_rx

|Family type|Family Name|Counter Name| Counter Help|Static Labels|Dynamic Labels|
|----------:|:--------------:|:------------:|:-----:|:-------:|-------:|
|Counter|number_of_messages_received|attach_request|number of attach request received from UE|[{'interface': 'nas'}, {'msg_type': 'attach_request'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_identity_response|number of identity response received from UE|[{'interface': 'nas'}, {'msg_type': 'identity_response'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_authentication_response|number of authentication response received from UE|[{'interface': 'nas'}, {'msg_type': 'authentication_response'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_security_mode_response|number of security mode complete response received from UE|[{'interface': 'nas'}, {'msg_type': 'security_mode_response'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_esm_response|number of ESM response messaged received by MME|[{'interface': 'nas'}, {'msg_type': 'esm_response'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_init_context_response|number of NAS init context response messaged received by MME|[{'interface': 's1ap'}, {'msg_type': 'init_context_response'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_attach_complete|number of NAS attach complete message received by MME|[{'interface': 'nas'}, {'msg_type': 'attach_complete'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_detach_request|number of NAS detach message received by MME|[{'interface': 'nas'}, {'msg_type': 'detach_request'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_act_ded_br_ctxt_acpt|number of NAS act ded bearer accept message received by MME|[{'interface': 'nas'}, {'msg_type': 'act_ded_br_ctxt_acpt'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_act_ded_br_ctxt_rjct|number of NAS act ded bearer reject message received by MME|[{'interface': 'nas'}, {'msg_type': 'act_ded_br_ctxt_rjct'}]|['enb_addr']|
|Counter|number_of_messages_received|nas_deact_eps_br_ctxt_acpt|number of NAS deact eps bearer accept message received by MME|[{'interface': 'nas'}, {'msg_type': 'deact_eps_br_ctxt_acpt'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_release_request|number of s1ap release request received by MME|[{'interface': 's1ap'}, {'msg_type': 'release_request'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_release_complete|number of s1ap release complete received by MME|[{'interface': 's1ap'}, {'msg_type': 'release_complete'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_detach_accept|number of s1ap detach accept received by MME|[{'interface': 's1ap'}, {'msg_type': 'detach_accept'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_service_request|number of s1ap service request received by MME|[{'interface': 's1ap'}, {'msg_type': 'service_request'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_tau_request|number of s1ap TAU request received by MME|[{'interface': 's1ap'}, {'msg_type': 'tau_request'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_handover_request_ack|number of s1ap Handover request ack received by MME|[{'interface': 's1ap'}, {'msg_type': 'handover_request_ack'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_handover_notify|number of s1ap Handover notify received by MME|[{'interface': 's1ap'}, {'msg_type': 'handover_notify'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_handover_required|number of s1ap Handover required received by MME|[{'interface': 's1ap'}, {'msg_type': 'handover_required'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_enb_status_transfer|number of s1ap enb status transfer received by MME|[{'interface': 's1ap'}, {'msg_type': 'enb_status_transfer'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_handover_cancel|number of s1ap Handover cancel received by MME|[{'interface': 's1ap'}, {'msg_type': 'handover_cancel'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_handover_failure|number of s1ap Handover failure received by MME|[{'interface': 's1ap'}, {'msg_type': 'handover_failure'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_erab_modification_indication|number of s1ap Erab modification indication received by MME|[{'interface': 's1ap'}, {'msg_type': 'erab_modification_indication'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_erab_setup_response|number of s1ap Erab setup response received by MME|[{'interface': 's1ap'}, {'msg_type': 'erab_setup_response'}]|['enb_addr']|
|Counter|number_of_messages_received|s1ap_erab_release_response|number of s1ap Erab release response received by MME|[{'interface': 's1ap'}, {'msg_type': 'erab_release_response'}]|['enb_addr']|
|Counter|number_of_messages_received|s6a_authentication_information_answer|number of authentication answer received from HSS|[{'interface': 's6a'}, {'msg_type': 'authentication_information_answer'}]|['hss_addr']|
|Counter|number_of_messages_received|s6a_update_location_answer|number of update location answer received from HSS|[{'interface': 's6a'}, {'msg_type': 'update_location_answer'}]|['hss_addr']|
|Counter|number_of_messages_received|s6a_purge_answer|number of purge answer received from HSS|[{'interface': 's6a'}, {'msg_type': 'purge_answer'}]|['hss_addr']|
|Counter|number_of_messages_received|s6a_cancel_location_request|number of cancel location request received by MME|[{'interface': 's6a'}, {'msg_type': 'cancel_location_request'}]|['hss_addr']|
|Counter|number_of_messages_received|s11_create_session_response|number of CSRsp received by MME|[{'interface': 's11'}, {'msg_type': 'create_session_response'}]|['sgw_addr']|
|Counter|number_of_messages_received|s11_modify_bearer_response|number of MBRsp received by MME|[{'interface': 's11'}, {'msg_type': 'modify_bearer_response'}]|['sgw_addr']|
|Counter|number_of_messages_received|s11_delete_session_response|number of DSRsp received by MME|[{'interface': 's11'}, {'msg_type': 'delete_session_response'}]|['sgw_addr']|
|Counter|number_of_messages_received|s11_release_bearer_response|number of RAB Rsp received by MME|[{'interface': 's11'}, {'msg_type': 'release_bearer_response'}]|['sgw_addr']|
|Counter|number_of_messages_received|s11_downlink_notification_indication|number of DDN Ind received by MME|[{'interface': 's11'}, {'msg_type': 'downlink_notification_indication'}]|['sgw_addr']|
|Counter|number_of_messages_received|s11_create_bearer_request|number of create bearer request received by MME|[{'interface': 's11'}, {'msg_type': 'create_bearer_request'}]|['sgw_addr']|
|Counter|number_of_messages_received|s11_delete_bearer_request|number of delete bearer request received by MME|[{'interface': 's11'}, {'msg_type': 'delete_bearer_request'}]|['sgw_addr']|


# mme_msg_tx

|Family type|Family Name|Counter Name| Counter Help|Static Labels|Dynamic Labels|
|----------:|:--------------:|:------------:|:-----:|:-------:|-------:|
|Counter|number_of_messages_sent|nas_identity_request|number of identity request sent by MME|[{'interface': 'nas'}, {'msg_type': 'identity_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_authentication_request|number of authentication request sent by MME|[{'interface': 'nas'}, {'msg_type': 'authentication_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_security_mode_command|number of security mode command sent to UE|[{'interface': 'nas'}, {'msg_type': 'security_mode_command'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_esm_information_request|number of ESM information request sent to UE|[{'interface': 'nas'}, {'msg_type': 'esm_information_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_initial_context_request|number of NAS initial context request sent to UE|[{'interface': 'nas'}, {'msg_type': 'initial_context_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_emm_information_req|number of EMM information request sent to UE|[{'interface': 'nas'}, {'msg_type': 'emm_information_req'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_attach_reject|number of attach reject sent to UE|[{'interface': 'nas'}, {'msg_type': 'attach_reject'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_service_reject|number of service request reject sent to UE|[{'interface': 'nas'}, {'msg_type': 'service_reject'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_tau_response|number of TAU response sent to UE|[{'interface': 'nas'}, {'msg_type': 'tau_response'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_act_ded_br_ctxt_request|number of act ded bearer request sent to UE|[{'interface': 'nas'}, {'msg_type': 'act_ded_br_ctxt_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_deact_eps_br_ctxt_request|number of deact eps bearer request sent to UE|[{'interface': 'nas'}, {'msg_type': 'deact_eps_br_ctxt_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_erab_modification|number of ERAB modification message sent by MME|[{'interface': 's1ap'}, {'msg_type': 'erab_modification_indication'}]|['enb_addr']|
|Counter|number_of_messages_sent|nas_network_initiated_detach|number of network initiated detach sent to UE|[{'interface': 'nas'}, {'msg_type': 'network_initiated_detach'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_s1_release_command|number of s1 Release command sent to eNB|[{'interface': 's1ap'}, {'msg_type': 's1_release_command'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_handover_request|number of handover request sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'handover_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_handover_command|number of s1 handover command sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'handover_command'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_mme_status_transfer|number of MME status transfer sent to eNB |[{'interface': 's1ap'}, {'msg_type': 'mme_status_transfer'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_handover_preparation_failure|number of handover preparation failure sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'handover_preparation_failure'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_handover_cancel_ack|number of handover cancel ack sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'handover_cancel_ack'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_paging_request|number of paging request sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'paging_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_ics_request_paging|number of ics request paging sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'ics_request_paging'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_detach_accept|number of detach accept sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'detach_accept'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_erab_setup_request|number of erab setup request sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'erab_setup_request'}]|['enb_addr']|
|Counter|number_of_messages_sent|s1ap_erab_release_command|number of erab release command sent to eNB|[{'interface': 's1ap'}, {'msg_type': 'erab_release_command'}]|['enb_addr']|
|Counter|number_of_messages_sent|s11_create_session_request|number of CSReq sent by MME|[{'interface': 's11'}, {'msg_type': 'create_session_request'}]|['sgw_addr']|
|Counter|number_of_messages_sent|s11_modify_bearer_request|number of MBReq sent by MME|[{'interface': 's11'}, {'msg_type': 'modify_bearer_request'}]|['sgw_addr']|
|Counter|number_of_messages_sent|s11_delete_session_request|number of DSReq sent by MME|[{'interface': 's11'}, {'msg_type': 'delete_session_request'}]|['sgw_addr']|
|Counter|number_of_messages_sent|s11_downlink_data_notification_ack|number of DDN Ack sent by MME|[{'interface': 's11'}, {'msg_type': 'downlink_data_notification_ack'}]|['sgw_addr']|
|Counter|number_of_messages_sent|s11_release_bearer_request|number of RAB request sent by MME|[{'interface': 's11'}, {'msg_type': 'release_bearer_request'}]|['sgw_addr']|
|Counter|number_of_messages_sent|s11_create_bearer_response|number of create bearer response sent by MME|[{'interface': 's11'}, {'msg_type': 'create_bearer_response'}]|['sgw_addr']|
|Counter|number_of_messages_sent|s11_delete_bearer_response|number of delete bearer response sent by MME|[{'interface': 's11'}, {'msg_type': 'delete_bearer_response'}]|['sgw_addr']|
|Counter|number_of_messages_sent|s6a_authentication_info_request|number of authentication information request sent by MME|[{'interface': 's6a'}, {'msg_type': 'authentication_info_request'}]|['hss_addr']|
|Counter|number_of_messages_sent|s6a_update_location_request|number of ULR request sent by MME |[{'interface': 's6a'}, {'msg_type': 'update_location_request'}]|['hss_addr']|
|Counter|number_of_messages_sent|s6a_purge_request|number of Purge request sent by MME|[{'interface': 's6a'}, {'msg_type': 'purge_request'}]|['hss_addr']|


# mme_procedures

|Family type|Family Name|Counter Name| Counter Help|Static Labels|Dynamic Labels|
|----------:|:--------------:|:------------:|:-----:|:-------:|-------:|
|Counter|number_of_procedures|attach_ue_proc|number of UE attach procedure started by MME|[{'procedure': 'ATTACH_PROC'}, {'attach_type': 'imsi'}]|['enb_addr']|
|Counter|number_of_procedures|attach_ue_proc|number of UE attach procedure started by MME|[{'procedure': 'ATTACH_PROC'}, {'attach_type': 'guti'}]|['enb_addr']|
|Counter|number_of_procedures|attach_ue_proc_result|UE attach procedure results|[{'procedure': 'ATTACH_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|attach_ue_proc_result|UE attach procedure results|[{'procedure': 'ATTACH_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|detach_ue_proc|number of UE detach procedure started by MME|[{'procedure': 'DETACH_PROC'}, {'detach_type': 'network_init'}]|['enb_addr']|
|Counter|number_of_procedures|detach_ue_proc|number of UE detach procedure started by MME|[{'procedure': 'DETACH_PROC'}, {'detach_type': 'network_init'}]|['sgw_addr']|
|Counter|number_of_procedures|detach_ue_proc|number of UE detach procedure started by MME|[{'procedure': 'DETACH_PROC'}, {'detach_type': 'ue_init'}]|['enb_addr']|
|Counter|number_of_procedures|detach_ue_proc|number of UE detach procedure started by MME|[{'procedure': 'DETACH_PROC'}, {'detach_type': 'ue_init'}]|['sgw_addr']|
|Counter|number_of_procedures|detach_ue_proc_result|UE detach procedure results|[{'procedure': 'DETACH_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|detach_ue_proc_result|UE detach procedure results|[{'procedure': 'DETACH_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|s1_release_proc|number of S1 Release procedure started by MME|[{'procedure': 'S1_RELEASE_PROC'}]|['enb_addr']|
|Counter|number_of_procedures|s1_release_proc_result|s1 release procedure results|[{'procedure': 'S1_RELEASE_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|s1_release_proc_result|s1 release procedure results|[{'procedure': 'S1_RELEASE_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|service_request_proc|number of service request procedure started by MME|[{'procedure': 'SERVICE_REQUEST_PROC'}, {'init_by': 'ddn_init'}]|['enb_addr']|
|Counter|number_of_procedures|service_request_proc|number of service request procedure started by MME|[{'procedure': 'SERVICE_REQUEST_PROC'}, {'init_by': 'pgw_init'}]|['enb_addr']|
|Counter|number_of_procedures|service_request_proc|number of service request procedure started by MME|[{'procedure': 'SERVICE_REQUEST_PROC'}, {'init_by': 'ue_init'}]|['enb_addr']|
|Counter|number_of_procedures|service_request_proc_result|service request procedure results|[{'procedure': 'SERVICE_REQUEST_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|service_request_proc_result|service request procedure results|[{'procedure': 'SERVICE_REQUEST_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|tau_proc|number of TAU procedure started by MME|[{'procedure': 'TAU_PROC'}]|['enb_addr']|
|Counter|number_of_procedures|tau_proc_result|TAU procedure results|[{'procedure': 'TAU_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|tau_proc_result|TAU procedure results|[{'procedure': 'TAU_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|s1_enb_handover_proc|number of s1 enb handover procedure started by MME|[{'procedure': 'S1_ENB_HANDOVER_PROC'}]|['enb_addr']|
|Counter|number_of_procedures|s1_enb_handover_proc_result|s1 enb handover procedure results|[{'procedure': 'S1_ENB_HANDOVER_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|s1_enb_handover_proc_result|s1 enb handover procedure results|[{'procedure': 'S1_ENB_HANDOVER_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|erab_mod_ind_proc|number of erab modification indication procedure started by MME|[{'procedure': 'ERAB_MOD_IND_PROC'}]|['enb_addr']|
|Counter|number_of_procedures|erab_mod_ind_proc_result|erab modification indication indication procedure results|[{'procedure': 'ERAB_MOD_IND_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|erab_mod_ind_proc_result|erab modification indication indication procedure results|[{'procedure': 'ERAB_MOD_IND_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|create_bearer_proc|number of create bearer request procedure started by MME|[{'procedure': 'CREATE_BEARER_PROC'}]|['enb_addr']|
|Counter|number_of_procedures|create_bearer_proc_result|create bearer request indication procedure results|[{'procedure': 'CREATE_BEARER_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|create_bearer_proc_result|create bearer request indication procedure results|[{'procedure': 'CREATE_BEARER_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|delete_bearer_proc|number of delete bearer request procedure started by MME|[{'procedure': 'DELETE_BEARER_PROC'}]|['enb_addr']|
|Counter|number_of_procedures|delete_bearer_proc_result|delete bearer request indication procedure results|[{'procedure': 'DELETE_BEARER_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|delete_bearer_proc_result|delete bearer request indication procedure results|[{'procedure': 'DELETE_BEARER_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|ded_bearer_activation_proc|number of ded bearer activation procedure started by MME|[{'procedure': 'DED_BEARER_ACTIVATION_PROC'}]|['enb_addr']|
|Counter|number_of_procedures|ded_bearer_activation_proc_result|ded_bearer_activation procedure results|[{'procedure': 'DED_BEARER_ACTIVATION_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|ded_bearer_activation_proc_result|ded_bearer_activation procedure results|[{'procedure': 'DED_BEARER_ACTIVATION_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|ded_bearer_deactivation_proc|number of ded bearer deactivation procedure started by MME|[{'procedure': 'DED_BEARER_DEACTIVATION_PROC'}]|['enb_addr']|
|Counter|number_of_procedures|ded_bearer_deactivation_proc_result|ded_bearer_deactivation procedure results|[{'procedure': 'DED_BEARER_DEACTIVATION_PROC'}, {'proc_result': 'success'}]|['enb_addr', 'failure_reason']|
|Counter|number_of_procedures|ded_bearer_deactivation_proc_result|ded_bearer_deactivation procedure results|[{'procedure': 'DED_BEARER_DEACTIVATION_PROC'}, {'proc_result': 'failure'}]|['enb_addr', 'failure_reason']|

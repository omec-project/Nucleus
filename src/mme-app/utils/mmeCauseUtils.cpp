
/*
 * Copyright 2020-present Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <controlBlock.h>
#include <s1ap_structs.h>
#include <gtpCauseTypes.h>
#include <contextManager/subsDataGroupManager.h>
#include <log.h>
#include <utils/mmeCauseUtils.h>

using namespace mme;

uint32_t MmeCauseUtils::convertToNasEmmCause(ERROR_CODES mmeErrorCause)
{
    log_msg(LOG_DEBUG, "MME Error Cause %d", mmeErrorCause);

	uint32_t nasEmmCause = 0;

	switch(mmeErrorCause)
	{
		case S1AP_SECMODE_FAILED:
		case S6A_AIA_FAILED:
			nasEmmCause = emmCause_ue_id_not_derived_by_network;
			break;
		case MAC_MISMATCH:
			nasEmmCause = emmCause_mac_failure;
			break;
		default:
			nasEmmCause = emmCause_network_failure;
			break;
	}
	return nasEmmCause;
}

uint32_t MmeCauseUtils::convertToGtpCause(ERROR_CODES mmeErrorCause)
{
    log_msg(LOG_DEBUG, "MME Error Cause %d", mmeErrorCause);

	uint32_t gtpCause = 0;

	switch (mmeErrorCause)
	{
		case NETWORK_TIMEOUT:
			gtpCause = GTPV2C_CAUSE_UE_NOT_RESPONDING;
			break;
		case BEARER_CONTEXT_NOT_FOUND:
		case SESSION_CONTEXT_NOT_FOUND:
			gtpCause = GTPV2C_CAUSE_CONTEXT_NOT_FOUND;
			break;
		case PAGING_FAILED:
			gtpCause = GTPV2C_CAUSE_UNABLE_TO_PAGE_UE;
			break;
		default:
			gtpCause = GTPV2C_CAUSE_NETWORK_FAILURE;
			break;
	}
	return gtpCause;
}

S1apCause MmeCauseUtils::convertToS1apCause(ERROR_CODES mmeErrorCause)
{
    log_msg(LOG_DEBUG, "MME Error Cause %d", mmeErrorCause);

    S1apCause s1apCause;

    switch (mmeErrorCause)
    {
         case SUCCESS:
        {
           s1apCause.s1apCause_m.present = s1apCause_PR_radioNetwork;
           s1apCause.s1apCause_m.choice.radioNetwork = s1apCauseRadioNetwork_user_inactivity;
           break;
        }
        case SESSION_CONTAINER_EMPTY:
        case SESSION_CONTEXT_NOT_FOUND:
        case BEARER_CONTAINER_EMPTY:
        case BEARER_CONTEXT_NOT_FOUND:
        case NETWORK_TIMEOUT:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_misc;
            s1apCause.s1apCause_m.choice.misc = s1apCauseMisc_unspecified;
            break;
        }
        case S1AP_SECMODE_FAILED:
        case S6A_AIA_FAILED:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_misc;
            s1apCause.s1apCause_m.choice.misc = s1apCauseMisc_unknown_PLMN;
            break;
        }
        case S1AP_HOREQACK_FAILED:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_radioNetwork;
            s1apCause.s1apCause_m.choice.radioNetwork = s1apCauseRadioNetwork_ho_failure_in_target_EPC_eNB_or_target_system;
            break;
        }
		case S11_MODIFY_BEARER_RESP_FAILURE:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_radioNetwork;
            s1apCause.s1apCause_m.choice.radioNetwork = s1apCauseRadioNetwork_unknown_E_RAB_ID;
            break;
        }
        case MAC_MISMATCH:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_nas;
            s1apCause.s1apCause_m.choice.nas = s1apCauseNas_authentication_failure;
            break;
        }
        default:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_nas;
            s1apCause.s1apCause_m.choice.nas = s1apCauseNas_normal_release;
            break;
        }
    }
    return s1apCause;
}

uint32_t MmeCauseUtils::convertToGtpCause(s1apCause_t s1apCause)
{
	uint32_t gtpCause = 0;

    switch (s1apCause.present)
    {
        case s1apCause_PR_radioNetwork:
        {
            switch (s1apCause.choice.radioNetwork)
            {
                case s1apCauseRadioNetwork_multiple_E_RAB_ID_instances:
                case s1apCauseRadioNetwork_unknown_E_RAB_ID:
                case s1apCauseRadioNetwork_not_supported_QCI_value:
                {
                	gtpCause = GTPV2C_CAUSE_REQUEST_REJECTED;
                    break;
                }
                case s1apCauseRadioNetwork_s1_intra_system_handover_triggered:
                case s1apCauseRadioNetwork_s1_inter_system_handover_triggered:
                case s1apCauseRadioNetwork_x2_handover_triggered:
                {
                	gtpCause = GTPV2C_CAUSE_TEMPORARILY_REJECTED_DUE_TO_HANDOVER_TAU_RAU_PROCEDURE_IN_PROGRESS;
                    break;
                }
            }
        }break;
        default:
        {
        	gtpCause = GTPV2C_CAUSE_NETWORK_FAILURE;
	    break;
        }
    }

    return gtpCause;
}

uint32_t MmeCauseUtils::convertToGtpCause(esm_cause_t esmCause)
{
	uint32_t gtpCause = 0;

    switch (esmCause)
    {
	case INSUFFICIENT_RESOURCES:
	{
		gtpCause = GTPV2C_CAUSE_NO_RESOURCES_AVAILABLE;
	    break;
	}
	case SERVICE_OPTION_NOT_SUPPORTED:
	{
		gtpCause = GTPV2C_CAUSE_SERVICE_NOT_SUPPORTED;
	    break;
	}
	case SEMANTIC_ERROR_IN_THE_TFT_OPERATION:
	{
		gtpCause = GTPV2C_CAUSE_SEMANTIC_ERROR_IN_THE_TFT_OPERATION;
	    break;
	}
	case SYNTACTICAL_ERROR_IN_THE_TFT_OPERATION:
	{
		gtpCause = GTPV2C_CAUSE_SYNTACTIC_ERROR_IN_THE_TFT_OPERATION;
	    break;
	}
	case SEMANTIC_ERROR_IN_PACKET_FILTERS:
	{
		gtpCause = GTPV2C_CAUSE_SEMANTIC_ERRORS_IN_PACKET_FILTERS;
	    break;
	}
	case SYNTACTICAL_ERROR_IN_PACKET_FILTERS:
	{
		gtpCause = GTPV2C_CAUSE_SYNTACTIC_ERRORS_IN_PACKET_FILTERS;
	    break;
	}
	default:
	{
		gtpCause = GTPV2C_CAUSE_NETWORK_FAILURE;
	    break;
	}
    }
    return gtpCause;
}


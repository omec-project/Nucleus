
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
    log_msg(LOG_DEBUG, "MME Error Cause %d\n", mmeErrorCause);

	uint32_t nasEmmCause = 0;

	switch(mmeErrorCause)
	{
		case S1AP_SECMODE_FAILED:
		case S6A_AIA_FAILED:
			nasEmmCause = emmCause_ue_id_not_derived_by_network;
			break;

		default:
			nasEmmCause = emmCause_network_failure;
			break;
	}
	return nasEmmCause;
}

uint32_t MmeCauseUtils::convertToGtpCause(ERROR_CODES mmeErrorCause)
{
    log_msg(LOG_DEBUG, "MME Error Cause %d\n", mmeErrorCause);

	uint32_t gtpCause = 0;

	switch (mmeErrorCause)
	{
		case NETWORK_TIMEOUT:
			gtpCause = GTPV2C_CAUSE_UE_NOT_RESPONDING;
			break;
		default:
			gtpCause = GTPV2C_CAUSE_NETWORK_FAILURE;
			break;
	}
	return gtpCause;
}

S1apCause MmeCauseUtils::convertToS1apCause(ERROR_CODES mmeErrorCause)
{
    log_msg(LOG_DEBUG, "MME Error Cause %d\n", mmeErrorCause);

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
        case BEARERC_CONTAINER_EMPTY:
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
        default:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_nas;
            s1apCause.s1apCause_m.choice.nas = s1apCauseNas_normal_release;
            break;
        }
    }
    return s1apCause;
}




/*
 * Copyright 2020-present Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <controlBlock.h>
#include <s1ap_structs.h>
#include <contextManager/subsDataGroupManager.h>
#include <log.h>
#include <utils/mmeCauseUtils.h>
#include <utils/mmeCauseTypes.h>

using namespace mme;

uint32_t MmeCauseUtils::convertToNasEmmCause(MmeErrorCause mmeErrorCause)
{
    log_msg(LOG_DEBUG, "MME Error Cause %d\n", mmeErrorCause);

	uint32_t nasEmmCause = 0;

	switch(mmeErrorCause)
	{
		case secModeRespFailure_c:
		case s6AiaFailure_c:
			nasEmmCause = emmCause_ue_id_not_derived_by_network;
			break;

		default:
			nasEmmCause = emmCause_network_failure;
			break;
	}
	return nasEmmCause;
}

uint32_t MmeCauseUtils::convertToGtpCause(MmeErrorCause mmeErrorCause)
{
    log_msg(LOG_DEBUG, "MME Error Cause %d\n", mmeErrorCause);

	uint32_t gtpCause = 0;

	switch (mmeErrorCause)
	{
		case networkTimeout_c:
			gtpCause = 0x87; //UE not responding
			break;
		default:
			gtpCause = 0x13; //Network Failure
			break;
	}
	return gtpCause;
}

S1apCause MmeCauseUtils::convertToS1apCause(MmeErrorCause mmeErrorCause)
{
    log_msg(LOG_DEBUG, "MME Error Cause %d\n", mmeErrorCause);

    S1apCause s1apCause;

    switch (mmeErrorCause)
    {
        case networkTimeout_c:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_misc;
            s1apCause.s1apCause_m.choice.misc = s1apCauseMisc_unspecified;
            break;
        }
        case secModeRespFailure_c:
        case s6AiaFailure_c:
        {
            s1apCause.s1apCause_m.present = s1apCause_PR_misc;
            s1apCause.s1apCause_m.choice.misc = s1apCauseMisc_unknown_PLMN;
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



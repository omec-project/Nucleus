/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <err_codes.h>
#include <gtpCauseTypes.h>
#include <log.h>
#include <mmeSmDefs.h>
#include <state.h>
#include <timeoutManager.h>
#include <timerQueue.h>
#include <utils/mmeContextManagerUtils.h>
#include <utils/mmeStatesUtils.h>
#include <utils/mmeTimerTypes.h>
#include <utils/mmeTimerUtils.h>

#include <state.h>
#include <tempDataBlock.h>
#include <vector>

using namespace mme;
using namespace SM;
using namespace cmn;
using namespace std;

/***************************************
* MmeStatesUtils : on_state_entry
***************************************/
ActStatus MmeStatesUtils::on_state_entry(ControlBlock& cb)
{
    // Check and start state guard timers
    MmeProcedureCtxt *procedure_p =
            dynamic_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p == NULL)
    {
        return ActStatus::HALT;
    }

    if (procedure_p->getCtxtType() != defaultMmeProcedure_c)
    {
        SM::State *currentState_p = procedure_p->getCurrentState();

        if (currentState_p != NULL)
        {
            uint32_t timeoutVal =
                    currentState_p->getStateGuardTimeoutDuration();
            if (timeoutVal > 0)
            {
                MmeUeTimerContext *timerCtxt =
                        static_cast<MmeUeTimerContext*>(MmeTimerUtils::startTimer(
                                timeoutVal,
                                cb.getCBIndex(),
                                stateGuardTimer_c,  // timer type
                                currentState_p->getStateId())); // treat state id as timerId

                if (timerCtxt != NULL)
                {
                    timerCtxt->setProcType(procedure_p->getCtxtType());
                    timerCtxt->setBearerId(procedure_p->getBearerId());
                    procedure_p->setStateGuardTimerCtxt(timerCtxt);
                }
            }
        }
    }
    return ActStatus::PROCEED;
}

/***************************************
* MmeStatesUtils : on_state_exit
***************************************/
ActStatus MmeStatesUtils::on_state_exit(ControlBlock &cb)
{
    // Check and stop state guard timers
    MmeProcedureCtxt *procedure_p =
            static_cast<MmeProcedureCtxt*>(cb.getTempDataBlock());
    if (procedure_p == NULL)
    {
        return ActStatus::HALT;
    }

    if (procedure_p->getCtxtType() != defaultMmeProcedure_c)
    {
        MmeTimerUtils::stopTimer(procedure_p->getStateGuardTimerCtxt());
        procedure_p->setStateGuardTimerCtxt(NULL);
    }
    return ActStatus::PROCEED;
}

/***************************************
 * MmeStatesUtils : validate_event
 ***************************************/
EventStatus MmeStatesUtils::validate_event(ControlBlock &cb,
        TempDataBlock *tempDataBlock, SM::Event &event)
{
    EventStatus rc = EventStatus::FORWARD;

    SM::State* currentState = tempDataBlock->getCurrentState();
    if(currentState->isEventHandled(event.getEventId()) == false)
    {
        return rc;
    }

    MmeProcedureCtxt *smProc_p =
            static_cast<MmeProcedureCtxt*>(tempDataBlock);

    switch (event.getEventId())
    {
    case STATE_GUARD_TIMEOUT:
    {
        cmn::TimeoutEMsgShPtr eMsg = std::dynamic_pointer_cast<cmn::TimeoutMessage>(
                event.getEventData());

        if (eMsg)
        {
            MmeUeTimerContext *timerCtxt =
                    static_cast<MmeUeTimerContext*>(eMsg->getTimerContext());

            if (timerCtxt != NULL)
            {
                uint16_t timerId =
                        smProc_p->getCurrentState()->getStateId();

                // Check if we are in same state or state has already changed.
                // This happens when an event for the state was received and before
                // it got processed, the timeout event was fired by the timer thread.
                // If so, we need to ignore the guard timer expiry event.
                if (smProc_p->getCtxtType() == timerCtxt->getProcType() &&
						smProc_p->getBearerId() == timerCtxt->getBearerId())
                {
                    if (timerCtxt->getTimerId() == timerId)
                        rc = EventStatus::CONSUME;
                    else
                        rc = EventStatus::IGNORE;
                }
            }
        }
    }break;
    case ERAB_SETUP_RESP_FROM_ENB:
    {
        cmn::IpcEMsgShPtr eMsg =
                std::dynamic_pointer_cast<cmn::IpcEventMessage>(
                        event.getEventData());
        if (eMsg)
        {
            utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
            if (msgBuf != NULL)
            {
                const erabSuResp_Q_msg_t *erabSuResp = static_cast<const erabSuResp_Q_msg_t*>(msgBuf->getDataPointer());

                if (erabSuResp != NULL)
                {
                    if (erabSuResp->erab_su_list.count)
                    {
                        for (int i = 0; i < erabSuResp->erab_su_list.count; i++)
                        {
                            if (erabSuResp->erab_su_list.erab_su_item[i].e_RAB_ID
                                    == smProc_p->getBearerId())
                            {
                                rc = EventStatus::CONSUME_AND_FORWARD;
                                break;
                            }
                        }
                    }

                    if (erabSuResp->erab_fail_list.count)
                    {
                        for (int i = 0; i < erabSuResp->erab_fail_list.count;
                                i++)
                        {
                            if (erabSuResp->erab_fail_list.erab_fail_item[i].e_RAB_ID
                                    == smProc_p->getBearerId())
                            {
                                rc = EventStatus::CONSUME_AND_FORWARD;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
        break;
    case ERAB_REL_RESP_FROM_ENB:
        {
            cmn::IpcEMsgShPtr eMsg =
                    std::dynamic_pointer_cast<cmn::IpcEventMessage>(
                            event.getEventData());
            if (eMsg)
            {
                utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
                if (msgBuf != NULL)
                {
                    const erab_rel_resp_Q_msg_t *erabRelResp = static_cast<const erab_rel_resp_Q_msg_t *>(msgBuf->getDataPointer());
                    if (erabRelResp != NULL)
                    {
                        if (erabRelResp->erab_rel_list.count)
                        {
                            for (int i = 0; i < erabRelResp->erab_rel_list.count; i++)
                            {
                                if (erabRelResp->erab_rel_list.erab_id[i]
                                        == smProc_p->getBearerId())
                                {
                                    rc = EventStatus::CONSUME_AND_FORWARD;
                                    break;
                                }
                            }
                        }

                        if (erabRelResp->erab_failed_to_release_list.count)
                        {
                            for (int i = 0; i < erabRelResp->erab_failed_to_release_list.count;
                                    i++)
                            {
                                if (erabRelResp->erab_failed_to_release_list.erab_item[i].e_RAB_ID
                                        == smProc_p->getBearerId())
                                {
                                    rc = EventStatus::CONSUME_AND_FORWARD;
                                    break;
                                }
                            }
                        }

                    }
                }
            }
        }
            break;
    case ACT_DED_BEARER_ACCEPT_FROM_UE:
    {
        cmn::IpcEMsgShPtr eMsg =
                std::dynamic_pointer_cast<cmn::IpcEventMessage>(
                        event.getEventData());
        if (eMsg)
        {
            utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
            if (msgBuf != NULL)
            {
                const dedicatedBearerContextAccept_Q_msg_t *dedBrAcpt = static_cast<const dedicatedBearerContextAccept_Q_msg_t*>(msgBuf->getDataPointer());
                if (dedBrAcpt != NULL)
                {
                    if (smProc_p->getBearerId() == dedBrAcpt->eps_bearer_id)
                    {
                        rc = EventStatus::CONSUME;
                    }
                }
            }
        }
    }
        break;
    case ACT_DED_BEARER_REJECT_FROM_UE:
    {
        cmn::IpcEMsgShPtr eMsg =
                std::dynamic_pointer_cast<cmn::IpcEventMessage>(
                        event.getEventData());
        if (eMsg)
        {
            utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
            if (msgBuf != NULL)
            {
                const dedicatedBearerContextReject_Q_msg *dedBrRjct = static_cast<const dedicatedBearerContextReject_Q_msg*> (msgBuf->getDataPointer());
                if (dedBrRjct != NULL)
                {
                    if (smProc_p->getBearerId() == dedBrRjct->eps_bearer_id)
                    {
                        rc = EventStatus::CONSUME;
                    }
                }
            }
        }
    }
        break;
    case DEACT_DED_BEARER_ACCEPT_FROM_UE:
        {
            cmn::IpcEMsgShPtr eMsg =
                    std::dynamic_pointer_cast<cmn::IpcEventMessage>(
                            event.getEventData());
            if (eMsg)
            {
                utils::MsgBuffer *msgBuf = eMsg->getMsgBuffer();
                if (msgBuf != NULL)
                {
                    const deactivate_epsbearerctx_accept_Q_msg *deactBrAcpt = static_cast<const deactivate_epsbearerctx_accept_Q_msg_t*>(msgBuf->getDataPointer());
                    if (deactBrAcpt != NULL)
                    {
                        if (smProc_p->getBearerId() == deactBrAcpt->eps_bearer_id)
                        {
                            rc = EventStatus::CONSUME;
                        }
                    }
                }
            }
        }
            break;
    case GW_CP_REQ_INIT_PAGING:
    case GW_INIT_DED_BEARER_AND_SESSION_SETUP:
    case DED_BEARER_DEACT_START:
    case PAGING_FAILURE:
    case DETACH_FAILURE:
    {
        rc = EventStatus::CONSUME_AND_FORWARD;
    }
        break;
    default:
    {
        rc = EventStatus::CONSUME;
    }
    }
    return rc;
}

/**********************************************
* MmeStatesUtils : handle_sm_exception
***********************************************/
void MmeStatesUtils::handle_sm_exception(SM::ControlBlock& cb, uint8_t err_code)
{

    log_msg(LOG_ERROR,"CB with index %d has encountered a "
            "SM Exception with error code : %d", cb.getCBIndex(), err_code);

    //Cleaning up the UE Context
    MmeContextManagerUtils::deleteUEContext(cb.getCBIndex());
}

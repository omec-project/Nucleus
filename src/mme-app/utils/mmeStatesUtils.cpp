/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <err_codes.h>
#include <gtpCauseTypes.h>
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
                const s1_incoming_msg_data_t *msgData_p =
                        (s1_incoming_msg_data_t*) (msgBuf->getDataPointer());

                if (msgData_p != NULL)
                {
                    const struct erabSuResp_Q_msg &erabSuResp =
                            (msgData_p->msg_data.erabSuResp_Q_msg_m);

                    if (erabSuResp.erab_su_list.count)
                    {
                        for (int i = 0; i < erabSuResp.erab_su_list.count; i++)
                        {
                            if (erabSuResp.erab_su_list.erab_su_item[i].e_RAB_ID
                                    == smProc_p->getBearerId())
                            {
                                rc = EventStatus::CONSUME_AND_FORWARD;
                                break;
                            }
                        }
                    }

                    if (erabSuResp.erab_fail_list.count)
                    {
                        for (int i = 0; i < erabSuResp.erab_fail_list.count;
                                i++)
                        {
                            if (erabSuResp.erab_fail_list.erab_fail_item[i].e_RAB_ID
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
                const s1_incoming_msg_data_t *msgData_p =
                        (s1_incoming_msg_data_t*) (msgBuf->getDataPointer());
                if (msgData_p != NULL)
                {
                    const struct dedicatedBearerContextAccept_Q_msg &dedBrAcpt =
                            (msgData_p->msg_data.dedBearerContextAccept_Q_msg_m);

                    if (smProc_p->getBearerId() == dedBrAcpt.eps_bearer_id)
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
                const s1_incoming_msg_data_t *msgData_p =
                        (s1_incoming_msg_data_t*) (msgBuf->getDataPointer());
                if (msgData_p != NULL)
                {
                    const struct dedicatedBearerContextReject_Q_msg &dedBrRjct =
                            (msgData_p->msg_data.dedBearerContextReject_Q_msg_m);

                    if (smProc_p->getBearerId() == dedBrRjct.eps_bearer_id)
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
    case PAGING_FAILURE:
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


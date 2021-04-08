/*
 * Copyright 2019-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "s1apContextManager/s1apContextWrapper.h"

#include "../../../include/s1ap/s1apContextManager/s1apDataBlocks.h"
#include "s1apContextManager/s1apDataGroupManager.h"
#include "log.h"
#include "err_codes.h"

using namespace mme;
using namespace SM;
   
uint32_t createControlBlock_cpp()
{
    SM::ControlBlock *cb = NULL;
    EnbContext* enbCtxt_p = NULL;
    cb = mme::S1apDataGroupManager::Instance()->allocateCB();
    if(cb == NULL) {
        log_msg(LOG_ERROR, "failed to allocate CB ");
        return INVALID_ENTRY;
    }
    enbCtxt_p = static_cast <EnbContext *>(cb->getPermDataBlock());
    if (enbCtxt_p != NULL)
    {
        log_msg(LOG_ERROR, "Enb Context exists in newly allocated CB. Should release and allocate again. : TBD.");
        return INVALID_ENTRY;
    }

    log_msg(LOG_DEBUG, "Allocate Enb context for CB.");
    enbCtxt_p = mme::S1apDataGroupManager::Instance()->getEnbContext();
    if(NULL == enbCtxt_p)
    {
        log_msg(LOG_ERROR, "Enb Context alloc failed.");
        return INVALID_ENTRY;
    }

    enbCtxt_p->setContextID(cb->getCBIndex());
    cb->setPermDataBlock(enbCtxt_p);
    cb->setFastAccessBlock(enbCtxt_p, 1);
    return cb->getCBIndex();
}

uint32_t findControlBlockWithEnbId_cpp(uint32_t enbId)
{
    int cbIndex = mme::S1apDataGroupManager::Instance()->findCBWithenbId(enbId);

    if (cbIndex <= 0)
    {
        log_msg(LOG_ERROR, "Failed to find control block with enbid :%d.",enbId);
        return INVALID_CB_INDEX;
    }

    return cbIndex;

}

bool clearControlBlockDetailsEnbFd_cpp(uint32_t enbFd, struct EnbStruct *enbStructCtx)
{
    int cbIndex = mme::S1apDataGroupManager::Instance()->findCBWithenbFd(enbFd);

    if (cbIndex <= 0)
    {
        log_msg(LOG_ERROR, "Failed to find control block with enbFd :%d.",enbFd);
        return false; 
    }

    SM::ControlBlock *cb = NULL;
    if (cbIndex > 0)
    {
        cb = mme::S1apDataGroupManager::Instance()->findControlBlock(cbIndex);
        if(NULL == cb)
        {
            log_msg(LOG_ERROR,"Control block not found for cb Index %d.", cbIndex);
            return false;
        }

        EnbContext* enbCtxt_p = static_cast <EnbContext *>(cb->getPermDataBlock());
        enbStructCtx->tai_m.tac = enbCtxt_p->getTai().tac; 
        strcpy(enbStructCtx->eNbName, enbCtxt_p->getEnbname());
        int val = mme::S1apDataGroupManager::Instance()->deleteenbIdkey(
                                                                        enbCtxt_p->getEnbId());
        log_msg(LOG_DEBUG,"clearControlBlock : delete enbid2 %d: %d", enbCtxt_p->getEnbId(),val);
        val = mme::S1apDataGroupManager::Instance()->deleteenbFdkey(
                                                                    enbCtxt_p->getEnbFd());
        log_msg(LOG_DEBUG,"clearControlBlock : delete enbfd2 %d: %d", enbCtxt_p->getEnbFd(), val);
        log_msg(LOG_DEBUG,"clearControlBlock : fd map size after delete2 : %d.", 
                mme::S1apDataGroupManager::Instance()->sizeEnbFdKeyMap());
        log_msg(LOG_DEBUG,"clearControlBlock : Id map size after delete2 : %d.", 
                mme::S1apDataGroupManager::Instance()->sizeEnbIdKeyMap());
        log_msg(LOG_DEBUG,"clearControlBlock : deallocate cb for index %d", cbIndex);
        mme::S1apDataGroupManager::Instance()->deleteEnbContext(enbCtxt_p);
        mme::S1apDataGroupManager::Instance()->deAllocateCB(cbIndex);
        return true;
    }
    else
    {
        log_msg(LOG_ERROR, "Failed to find control block with cbIndex :%d.",cbIndex);
        return true;
    }


}

uint32_t findControlBlockWithEnbFd_cpp(uint32_t enbFd)
{
    int cbIndex = mme::S1apDataGroupManager::Instance()->findCBWithenbFd(enbFd);

    if (cbIndex <= 0)
    {
        log_msg(LOG_ERROR, "Failed to find control block with enbFd :%d.",enbFd);
        return INVALID_CB_INDEX;
    }

    return cbIndex;

}

uint32_t getEnbFdWithCbIndex_cpp(uint32_t cbIndex)
{
    SM::ControlBlock *cb = NULL;
    if (cbIndex > 0)
    {
        cb = mme::S1apDataGroupManager::Instance()->findControlBlock(cbIndex);
        if(NULL == cb)
        {
            log_msg(LOG_ERROR,"Control block not found for cb Index %d.", cbIndex);
            return INVALID_CB_INDEX;
        }

        EnbContext* enbCtxt_p = static_cast <EnbContext *>(cb->getPermDataBlock());
        assert(enbCtxt_p != NULL);
        return enbCtxt_p->getEnbFd();
    }
    else
    {
        log_msg(LOG_ERROR, "Failed to find control block with cbIndex :%d.",cbIndex);
        return INVALID_CB_INDEX;
    }

}

uint32_t setValuesForEnbCtx_cpp(uint32_t cbIndex, EnbStruct* enbCtx, bool update)
{
    SM::ControlBlock *cb = NULL;
    if (cbIndex > 0)
    {
        cb = mme::S1apDataGroupManager::Instance()->findControlBlock(cbIndex);
        if(NULL == cb)
        {
            log_msg(LOG_ERROR,"Control block not found for cb Index %d.", cbIndex);
            return INVALID_CB_INDEX;
        }

        EnbContext* enbCbCtx = static_cast <EnbContext *>(cb->getPermDataBlock());
        if(enbCbCtx != NULL)
        {
            if(update && (enbCbCtx->getTai().tac != enbCtx->tai_m.tac))
            {
                log_msg(LOG_ERROR,"Different Enbs accessing same context tacNew : %d, tacOld : %d.", enbCbCtx->getTai().tac, enbCtx->tai_m.tac);
                return INVALID_CB_INDEX;
            }
            else {
                int val = mme::S1apDataGroupManager::Instance()->deleteenbIdkey(
                                                            enbCbCtx->getEnbId());
                log_msg(LOG_DEBUG,"setValuesForEnb : delete enbid %d: %d", 
                                    enbCbCtx->getEnbId(),val);
                val = mme::S1apDataGroupManager::Instance()->deleteenbFdkey(
                                                            enbCbCtx->getEnbFd());
                log_msg(LOG_DEBUG,"setValuesForEnb : delete enbfd %d: %d", 
                                    enbCbCtx->getEnbFd(), val);
                log_msg(LOG_DEBUG,"setValuesForEnb : fd map size after delete : %d.", 
                        mme::S1apDataGroupManager::Instance()->sizeEnbFdKeyMap());
                log_msg(LOG_DEBUG,"setValuesForEnb : Id map size after delete : %d.", 
                mme::S1apDataGroupManager::Instance()->sizeEnbIdKeyMap());
                enbCbCtx->setEnbFd(enbCtx->enbFd_m);
                enbCbCtx->setEnbId(enbCtx->enbId_m);
                enbCbCtx->setS1apEnbUeId(enbCtx->s1apEnbUeId_m);
                enbCbCtx->setTai(enbCtx->tai_m);
                enbCbCtx->setEnbname(enbCtx->eNbName, strlen(enbCtx->eNbName));
                enbCbCtx->setRestartCounter(enbCbCtx->getRestartCounter()+1);
                enbCtx->restart_counter = enbCbCtx->getRestartCounter();
                log_msg(LOG_DEBUG,"setValuesForEnb : Enbs accessing context tacNew : %d, tacOld : %d name = %s .", enbCbCtx->getTai().tac, enbCtx->tai_m.tac, enbCtx->eNbName);
                val = mme::S1apDataGroupManager::Instance()->addenbIdkey(
                                              enbCtx->enbId_m, cbIndex);
                log_msg(LOG_DEBUG,"setValuesForEnb : cbIndex : %d, add enbId %d: %d", cbIndex, enbCbCtx->getEnbId(),val);
                val = mme::S1apDataGroupManager::Instance()->addenbFdkey(
                                              enbCtx->enbFd_m, cbIndex);
                log_msg(LOG_DEBUG,"setValuesForEnb : cbIndex : %d, add enbFd %d: %d", cbIndex, enbCbCtx->getEnbFd(),val);
                log_msg(LOG_DEBUG,"setValuesForEnb : fd map size after add : %d.", 
                        mme::S1apDataGroupManager::Instance()->sizeEnbFdKeyMap());
                log_msg(LOG_DEBUG,"setValuesForEnb : Id map size after add : %d.", 
                        mme::S1apDataGroupManager::Instance()->sizeEnbIdKeyMap());
            }
        }
        else
        {
            log_msg(LOG_ERROR,"No Valid Enb Ctx in Control Block.");
            return INVALID_CB_INDEX;
        }
    }
    else
    {
        log_msg(LOG_ERROR, "Failed to find control block with cbIndex :%d.",cbIndex);
        return INVALID_CB_INDEX;
    }

    return SUCCESS;
}

//}

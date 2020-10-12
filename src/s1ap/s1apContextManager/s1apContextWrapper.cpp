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
        log_msg(LOG_ERROR, "failed to allocate CB \n");
        return INVALID_ENTRY;
    }
    enbCtxt_p = static_cast <EnbContext *>(cb->getPermDataBlock());
    if (enbCtxt_p != NULL)
    {
        log_msg(LOG_ERROR, "Enb Context exists in newly allocated CB. Should release and allocate again. : TBD.\n");
        return INVALID_ENTRY;
    }

    log_msg(LOG_DEBUG, "Allocate Enb context for CB.\n");
    enbCtxt_p = mme::S1apDataGroupManager::Instance()->getEnbContext();
    if(NULL == enbCtxt_p)
    {
        log_msg(LOG_ERROR, "Enb Context alloc failed.\n");
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
        log_msg(LOG_ERROR, "Failed to find control block with enbid :%d.\n",enbId);
        return INVALID_CB_INDEX;
    }

    return cbIndex;

}

bool getControlBlockDetailsEnbFd_cpp(uint32_t enbFd, struct EnbStruct *enbStructCtx)
{
    int cbIndex = mme::S1apDataGroupManager::Instance()->findCBWithenbFd(enbFd);

    if (cbIndex <= 0)
    {
        log_msg(LOG_ERROR, "Failed to find control block with enbFd :%d.\n",enbFd);
        return false; 
    }

    SM::ControlBlock *cb = NULL;
    if (cbIndex > 0)
    {
        cb = mme::S1apDataGroupManager::Instance()->findControlBlock(cbIndex);
        if(NULL == cb)
        {
            log_msg(LOG_ERROR,"Control block not found for cb Index %d.\n", cbIndex);
            return false;
        }

        EnbContext* enbCtxt_p = static_cast <EnbContext *>(cb->getPermDataBlock());
        enbStructCtx->tai_m.tac = enbCtxt_p->getTai().tac; 
        strcpy(enbStructCtx->eNbName, enbCtxt_p->getEnbname());
        int val = mme::S1apDataGroupManager::Instance()->deleteenbIdkey(
                                                                        enbCtxt_p->getEnbId());
        log_msg(LOG_DEBUG,"delete enbid2 %d: %d\n", enbCtxt_p->getEnbId(),val);
        val = mme::S1apDataGroupManager::Instance()->deleteenbFdkey(
                                                                    enbCtxt_p->getEnbFd());
        log_msg(LOG_DEBUG,"delete enbfd2 %d: %d\n", enbCtxt_p->getEnbFd(), val);
        log_msg(LOG_DEBUG,"fd map size after delete2 : %d.\n", 
                mme::S1apDataGroupManager::Instance()->sizeFdkeyMap());
        log_msg(LOG_DEBUG,"Id map size after delete2 : %d.\n", 
                mme::S1apDataGroupManager::Instance()->sizeIdkeyMap());
        log_msg(LOG_DEBUG,"deallocate cb for index %d\n", cbIndex);
        mme::S1apDataGroupManager::Instance()->deleteEnbContext(enbCtxt_p);
        mme::S1apDataGroupManager::Instance()->deAllocateCB(cbIndex);
        return true;
    }
    else
    {
        log_msg(LOG_ERROR, "Failed to find control block with cbIndex :%d.\n",cbIndex);
        return true;
    }


}

uint32_t findControlBlockWithEnbFd_cpp(uint32_t enbFd)
{
    int cbIndex = mme::S1apDataGroupManager::Instance()->findCBWithenbFd(enbFd);

    if (cbIndex <= 0)
    {
        log_msg(LOG_ERROR, "Failed to find control block with enbFd :%d.\n",enbFd);
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
            log_msg(LOG_ERROR,"Control block not found for cb Index %d.\n", cbIndex);
            return INVALID_CB_INDEX;
        }

        EnbContext* enbCtxt_p = static_cast <EnbContext *>(cb->getPermDataBlock());
        log_msg(LOG_DEBUG,"Enb Fd : %d\n",
                enbCtxt_p->getEnbFd());
        return enbCtxt_p->getEnbFd();
    }
    else
    {
        log_msg(LOG_ERROR, "Failed to find control block with cbIndex :%d.\n",cbIndex);
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
            log_msg(LOG_ERROR,"Control block not found for cb Index %d.\n", cbIndex);
            return INVALID_CB_INDEX;
        }

        EnbContext* enbCbCtx = static_cast <EnbContext *>(cb->getPermDataBlock());
        if(enbCbCtx != NULL)
        {
            if(update && (enbCbCtx->getTai().tac != enbCtx->tai_m.tac))
            {
                log_msg(LOG_ERROR,"Different Enbs accessing same context tacNew : %d, tacOld : %d.\n", enbCbCtx->getTai().tac, enbCtx->tai_m.tac);
                return INVALID_CB_INDEX;
            }
            else {
                int val = mme::S1apDataGroupManager::Instance()->deleteenbIdkey(
                                                        enbCbCtx->getEnbId());
                log_msg(LOG_DEBUG,"cbIndex : %d, delete enbid %d: %d\n", cbIndex, enbCbCtx->getEnbId(),val);
                val = mme::S1apDataGroupManager::Instance()->deleteenbFdkey(
                                                        enbCbCtx->getEnbFd());
                log_msg(LOG_DEBUG,"cbIndex : %d, delete enbfd %d: %d\n", cbIndex, enbCbCtx->getEnbFd(), val);
                log_msg(LOG_DEBUG,"fd map size after delete : %d.\n", 
                        mme::S1apDataGroupManager::Instance()->sizeFdkeyMap());
                log_msg(LOG_DEBUG,"Id map size after delete : %d.\n", 
                        mme::S1apDataGroupManager::Instance()->sizeIdkeyMap());
                enbCbCtx->setEnbFd(enbCtx->enbFd_m);
                enbCbCtx->setEnbId(enbCtx->enbId_m);
                enbCbCtx->setS1apEnbUeId(enbCtx->s1apEnbUeId_m);
                enbCbCtx->setTai(enbCtx->tai_m);
                enbCbCtx->setEnbname(enbCtx->eNbName, strlen(enbCtx->eNbName));
                log_msg(LOG_DEBUG,"Enbs accessing context tacNew : %d, tacOld : %d name = %s .\n", enbCbCtx->getTai().tac, enbCtx->tai_m.tac, enbCtx->eNbName);
                val = mme::S1apDataGroupManager::Instance()->addenbIdkey(
                                              enbCtx->enbId_m, cbIndex);
                log_msg(LOG_DEBUG,"cbIndex : %d, add enbId %d: %d\n", cbIndex, enbCbCtx->getEnbId(),val);
                val = mme::S1apDataGroupManager::Instance()->addenbFdkey(
                                              enbCtx->enbFd_m, cbIndex);
                log_msg(LOG_DEBUG,"cbIndex : %d, add enbFd %d: %d\n", cbIndex, enbCbCtx->getEnbFd(),val);
                log_msg(LOG_DEBUG,"fd map size after add : %d.\n", 
                        mme::S1apDataGroupManager::Instance()->sizeFdkeyMap());
                log_msg(LOG_DEBUG,"Id map size after add : %d.\n", 
                        mme::S1apDataGroupManager::Instance()->sizeIdkeyMap());
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
        log_msg(LOG_ERROR, "Failed to find control block with cbIndex :%d.\n",cbIndex);
        return INVALID_CB_INDEX;
    }

    return SUCCESS;
}

//}

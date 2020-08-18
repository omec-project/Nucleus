/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <timeoutManager.h>
#include <timerQueue.h>
#include <utils/s11TimerUtils.h>
#include <utils/s11TimerTypes.h>
#include "s11.h"
#include "gtp_tables.h"
#include "s11_config.h"
#include "assert.h"

using namespace cmn;

extern TimeoutManager* timeoutMgr_g;
extern s11_config_t *s11_cfg;

s11TimerContext* s11TimerUtils::startTimer(uint32_t durationMs,
                                        uint16_t timerType,
                                        uint16_t timerId,
                                        gtpTransData *trans)
{
    s11TimerContext* timerCtxt = NULL;
    
    TimeoutManager &timeoutMgr =
    		static_cast<TimeoutManager&>(
    	    			compDb.getComponent(TimeoutManagerCompId));

    CTime duration(durationMs);
    CTime expiryTime;
    expiryTime = expiryTime + duration;
    
    timerCtxt = new s11TimerContext(
                trans, timerType, timerId, expiryTime);
    
    timeoutMgr.startTimer(timerCtxt);
    
    log_msg(LOG_DEBUG,
                "Timer started. duration %d \n", durationMs);
    return timerCtxt;
}

uint32_t s11TimerUtils::stopTimer(TimerContext* timerCtxt)
{
    uint32_t rc = 0;

    TimeoutManager &timeoutMgr =
    		static_cast<TimeoutManager&>(
    				compDb.getComponent(TimeoutManagerCompId));

    if (timerCtxt != NULL)
    {
        rc = timeoutMgr.cancelTimer(timerCtxt);
        if (rc > 0)
        {
            log_msg(LOG_DEBUG, "Timer deleted\n");
            delete timerCtxt;
        }
    }
    return rc;
}

void s11TimerUtils::onTimeout(TimerContext* timerCtxt)
{
    log_msg(LOG_DEBUG, "transaction timeout event \n");
    s11TimerContext* mmeTimerCtxt = static_cast<s11TimerContext *>(timerCtxt);
    if (mmeTimerCtxt == NULL)
    {
        return;
    }
    gtpTransData *trans = mmeTimerCtxt->getTrans();
    if(trans->tx_count >= s11_cfg->num_retransmission)
    {
        log_msg(LOG_DEBUG, "retransmission count exceeded \n");
        /* Free message and transaction and remove it from table */
        gtpTransData *t1 = gtpTables::Instance()->delSeqKey(trans->key);
        assert(t1 == trans);
        return;
    }
    trans->tx_count++;

    int res = sendto (
            trans->fd,
            trans->buf->getDataPointer(),
            trans->buf->getLength(), 0,
            (struct sockaddr*)(&trans->sgw_addr),
            sizeof(struct sockaddr_in));
    if (res < 0) {
        log_msg(LOG_ERROR,"Error in sendto while retransmitting message \n");
    }
    s11TimerUtils::startTimer(s11_cfg->retransmission_interval, 1, transTimer_c, trans);


}

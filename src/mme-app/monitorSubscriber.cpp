/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include <monitorSubscriber.h>

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <contextManager/subsDataGroupManager.h>
#include <log.h>
#include <mme_app.h>
#include <monSubDefs.h>
#include <thread_pool.h>

extern "C"
{
#include <unix_conn.h>
}


using namespace mme;

/**
* Monitor message Imsi Request processing.
*/
void MonitorSubscriber::handle_monitor_imsi_req(struct monitor_imsi_req *mir, int sock_fd)
{
    struct monitor_imsi_rsp mia = {0};

    DigitRegister15 IMSI;
    IMSI.setImsiDigits((uint8_t *)mir->imsi);

    int ueIdx =  SubsDataGroupManager::Instance()->findCBWithimsi(IMSI);
    if (ueIdx > 0)
    {
        SM::ControlBlock* controlBlk_p = mme::SubsDataGroupManager::Instance()->findControlBlock(ueIdx);
        if (controlBlk_p != NULL)
        {
            UEContext* ueCtxt_p = static_cast<UEContext *>(controlBlk_p->getPermDataBlock());
            if (ueCtxt_p != NULL)
            {
                MmContext* mmCtxt_p = ueCtxt_p->getMmContext();
                if (mmCtxt_p != NULL)
                {
                    if (mmCtxt_p->getMmState() == EpsAttached)
                    {
                        SessionContext* sessCtxt_p = ueCtxt_p->getSessionContext();
                        if (sessCtxt_p != NULL)
                        {
                            mia.result = 1;
                            mia.ue_state = mmCtxt_p->getEcmState();
                            mia.bearer_id = 5;
                            mia.paa = sessCtxt_p->getPdnAddr().paa_m.ip_type.ipv4.s_addr;
                            ueCtxt_p->getImsi().getImsiDigits(mia.imsi);
                            memcpy(&mia.tai, &ueCtxt_p->getTai().tai_m, sizeof(struct TAI));
                            memcpy(&mia.ambr, &ueCtxt_p->getAmbr().ambr_m, sizeof(struct AMBR));
                            log_msg(LOG_ERROR, "IMSI: %s PAA %x \n", mia.imsi, mia.paa);
                        }
                    }
                    else if (mmCtxt_p->getMmState() == EpsDetached)
                    {
                        mia.result = 1;
                        mia.ue_state = mmCtxt_p->getEcmState();
                        ueCtxt_p->getImsi().getImsiDigits(mia.imsi);
                        memcpy(&mia.tai, &ueCtxt_p->getTai().tai_m, sizeof(struct TAI));
                        memcpy(&mia.ambr, &ueCtxt_p->getAmbr().ambr_m, sizeof(struct AMBR));
                    }
                }
            }
        }

    }

    unsigned char buf[BUFFER_SIZE] = {0};
    struct monitor_resp_msg resp;
    resp.hdr = MONITOR_IMSI_RSP;
    memcpy(&resp.data.mia, &mia, sizeof(struct monitor_imsi_rsp));
    memcpy(buf, &mia, sizeof(struct monitor_imsi_rsp));
    send_unix_msg(sock_fd, buf, sizeof(struct monitor_imsi_rsp));

    return;
}

/**
* Monitor message IMSI list processing.
*/
void MonitorSubscriber::handle_imsi_list_req(struct monitor_imsi_req *mir, int sock_fd)
{
    uint32_t numOfUes = 0;

    uint8_t buf[BUFFER_SIZE] = {0};
    uint32_t size = sizeof(uint32_t);
    uint8_t* bufPtr = (uint8_t *)buf + size;

    for (uint32_t i = 1; i <= 8000; i++)
    {
            SM::ControlBlock* controlBlk_p = mme::SubsDataGroupManager::Instance()->findControlBlock(i);
            if (controlBlk_p != NULL && controlBlk_p->getPermDataBlock() != NULL)
            {
                UEContext* ueCtxt_p = static_cast<UEContext *>(controlBlk_p->getPermDataBlock());
                MmContext* mmCtxt = ueCtxt_p->getMmContext();
                if (mmCtxt != NULL && mmCtxt->getMmState() == EpsAttached)
                {
                    ueCtxt_p->getImsi().getImsiDigits(bufPtr);
                    bufPtr += IMSI_STR_LEN;
                    size += IMSI_STR_LEN;
                    numOfUes++;
                }
                if(((size + IMSI_STR_LEN) > BUFFER_SIZE))
                    break;
            }
    }

    memcpy(buf, &numOfUes, sizeof(uint32_t));
    send_unix_msg(sock_fd, buf, size);
    return;
}

/**
* Flush Imsis.
*/
void MonitorSubscriber::handle_imsi_flush_req()
{
    /* TODO: Throttling design in progress for global events that 
    are applicable to large number of UEs. */
}

/**
* Monitor message processing.
*/
void MonitorSubscriber::handle_monitor_processing(void *message)
{
    log_msg(LOG_INFO, "Monitor Message Received");

	int sock_fd = 0;
	memcpy(&sock_fd, (char*)message, sizeof(int));

	char *msg = ((char *) message) + (sizeof(int));
	struct monitor_imsi_req* mir = (struct monitor_imsi_req*)msg;

    switch(mir->req_type)
    {
        case 0:
            {
                log_msg(LOG_DEBUG, "imsi request");
	            handle_monitor_imsi_req(mir, sock_fd);
                break;
            }
        case 1:
            {
                log_msg(LOG_DEBUG, "imsi list request");
	            handle_imsi_list_req(mir, sock_fd);
                break;
            }
        case 2:
            {
                log_msg(LOG_DEBUG, "imsi flush request");
                handle_imsi_flush_req();
                break;
            }
    }
	/*free allocated message buffer*/
	free(message);
	return;
}



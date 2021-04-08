/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "gtpv2c.h"
#include "gtpv2c_ie.h"
#include "msgType.h"
#include "s11_config.h"
#include "s11_options.h"
#include <gtpV2StackWrappers.h>


/****Globals and externs ***/

/*S11 CP communication parameters*/
extern int g_s11_fd;
extern s11_config_t g_s11_cfg;
extern socklen_t g_s11_serv_size;

extern struct GtpV2Stack* gtpStack_gp;
struct thread_pool *g_tpool;

/****Global and externs end***/

/**
* Stage specific message processing.
*/
static int
ddn_failure_ind_processing(struct DDN_FAIL_Q_msg *ddn_fail_msg)
{
    struct MsgBuffer* ddnFailMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
    if(ddnFailMsgBuf_p == NULL)
    {
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
            return -1;
    }
    GtpV2MessageHeader gtpHeader;
    gtpHeader.msgType =  GTP_DOWNLINK_DATA_NOTIFICATION_FAILURE_IND;
    gtpHeader.sequenceNumber = ddn_fail_msg->seq_no;
    gtpHeader.teidPresent = true;
    gtpHeader.teid = ddn_fail_msg->s11_sgw_c_fteid.header.teid_gre;
    struct sockaddr_in sgw_ip = {0};
    create_sock_addr(&sgw_ip, g_s11_cfg.egtp_def_port, ddn_fail_msg->s11_sgw_c_fteid.ip.ipv4.s_addr);

    DownlinkDataNotificationFailureIndicationMsgData msgData;
    memset(&msgData, 0, sizeof(DownlinkDataNotificationFailureIndicationMsgData));


    msgData.cause.causeValue = ddn_fail_msg->cause;


    GtpV2Stack_buildGtpV2Message(gtpStack_gp, ddnFailMsgBuf_p, &gtpHeader, &msgData);


    sendto(g_s11_fd,
            MsgBuffer_getDataPointer(ddnFailMsgBuf_p),
            MsgBuffer_getBufLen(ddnFailMsgBuf_p), 0,
            (struct sockaddr*)&sgw_ip, g_s11_serv_size);

    log_msg(LOG_INFO, "DDN Failure Indication Sent, len - %d bytes.", MsgBuffer_getBufLen(ddnFailMsgBuf_p));
    MsgBuffer_free(ddnFailMsgBuf_p);
    return SUCCESS;
}


/**
* Thread function for stage.
*/
void*
ddn_failure_ind_handler(void *data)
{
    log_msg(LOG_INFO, "DDN Ack handler initialized");
    ddn_failure_ind_processing((struct DDN_FAIL_Q_msg *)data);
    return NULL;
}

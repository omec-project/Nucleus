/*
 * Copyright (c) 2021, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>
#include <freeDiameter/libfdproto.h>

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "s6a_fd.h"
#include "s6a.h"
#include "msgType.h"
#include "hss_message.h"
#include <transactionDb.h>

/** Global and externs**/
extern struct fd_dict_objects g_fd_dict_objs;
extern int g_Q_mme_S6a_fd;
extern void* s6IncReqTransDb_gp;
/**global and externs end**/



/**
 * @brief callback handler for dsr recvd from hss
 * Parse dsr and do cleanup for freediameter
 * @params callback std
 * @return error/success
 */
int
dsr_callback(struct msg **buf, struct avp *avps, struct session *sess,
            void *data, enum disp_action *action)
{
    struct msg *dsd = NULL;
    struct msg_hdr * hdr = NULL;
    struct avp *avp_ptr = NULL;
    dsr_Q_msg_t dsr_msg = {0};
    struct avp_hdr *avp_header = NULL;

    dsd = *buf;

    // Extract e2eId
    CHECK_FCT_DO(fd_msg_hdr(dsd, &hdr), return S6A_FD_ERROR);

    uint32_t e2eId = hdr->msg_eteid;
    if (s6IncReqTransDb_gp != NULL)
    {
        DbItem item = {(void*) dsd};
        bool rc = addTransItem(s6IncReqTransDb_gp, e2eId, &item);
        if (!rc)
        {
            log_msg(LOG_DEBUG,"Unable to add transaction item for eteid %d", e2eId);
        }
    }

    dump_fd_msg(dsd);

    fd_msg_search_avp(dsd, g_fd_dict_objs.user_name,&avp_ptr);
    if(NULL != avp_ptr) {
        fd_msg_avp_hdr(avp_ptr, &avp_header);
        memcpy(dsr_msg.header.IMSI,avp_header->avp_value->os.data,sizeof(dsr_msg.header.IMSI));
    }

    fd_msg_search_avp(dsd, g_fd_dict_objs.DSR_flags,&avp_ptr);
    if(NULL != avp_ptr) {
        fd_msg_avp_hdr(avp_ptr, &avp_header);
        dsr_msg.dsr_flags = avp_header->avp_value->u32;
    }

    fd_msg_search_avp(dsd, g_fd_dict_objs.Context_Identifier,&avp_ptr);
    if(NULL != avp_ptr) {
        fd_msg_avp_hdr(avp_ptr, &avp_header);
        dsr_msg.apn_conf_prof_id = avp_header->avp_value->u32;
    }

    dsr_msg.header.msg_type = delete_subscriber_data_request;
    dsr_msg.eteId = e2eId;

    dsr_msg.header.destInstAddr = htonl(mmeAppInstanceNum_c);
    dsr_msg.header.srcInstAddr = htonl(s6AppInstanceNum_c);

    /*Send to MME-APP*/
    send_tipc_message(g_Q_mme_S6a_fd, mmeAppInstanceNum_c, (char*)&dsr_msg, sizeof(dsr_Q_msg_t));

    *buf = NULL;

    return SUCCESS;
}

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

/****Globals and externs ***/

extern int g_Q_mme_S6a_fd;
extern int g_our_hss_fd;
extern void* s6IncReqTransDb_gp;
/*Making global just to avoid stack passing*/

extern struct fd_dict_objects g_fd_dict_objs;
extern struct fd_dict_data g_fd_dict_data;
/****Global and externs end***/

static int
send_dsa(struct s6a_dsa_Q_msg *dsa_msg)
{
    struct msg *dsr = NULL;
    struct msg *dsa = NULL;
    char* result_code = "DIAMETER_UNABLE_TO_COMPLY";

    if (dsa_msg->res_code == DIAMETER_ERROR_USER_UNKNOWN)
    {
        result_code = "DIAMETER_INVALID_AVP_VALUE";
    }
    else if (dsa_msg->res_code == DIAMETER_SUCCESS)
    {
        result_code = "DIAMETER_SUCCESS";
    }

    if (s6IncReqTransDb_gp != NULL)
    {
        DbItem item = {};
        bool rc = findTransItem(s6IncReqTransDb_gp, dsa_msg->eteId, &item);
        if (!rc)
        {
            log_msg(LOG_DEBUG,"Unable to find transaction item for eteid %d", dsa_msg->eteId);
        }
	dsr = ((struct msg*)item.appData_p);
    }

    CHECK_FCT( fd_msg_new_answer_from_req ( fd_g_config->cnf_dict, &dsr, 0 ) );
    dsa = dsr;

    CHECK_FCT( fd_msg_rescode_set( dsa, result_code, NULL, NULL, 1 ) );

    //Send the answer
    CHECK_FCT( fd_msg_send( &dsr, NULL, NULL ) );

    deleteTransItem(s6IncReqTransDb_gp, dsa_msg->eteId);

    return SUCCESS;
}

void*
dsa_handler(void *data)
{
    log_msg(LOG_INFO, "DSA Q handler ready.");

    send_dsa((struct s6a_dsa_Q_msg *) data);

    return NULL;
}

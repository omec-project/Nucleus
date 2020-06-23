/*
* Copyright 2019-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*
*
*/
extern "C"
{
#include "monitor_config.h"
}

#include "log.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "mme_app.h"

extern mme_config g_mme_cfg;

void mme_config_change_cbk(char *config_file, uint32_t flags)
{
    // Run the script with this file. It generates new config for mme
	log_msg(LOG_INFO, "Received %s . File %s Flags %x \n", __FUNCTION__, config_file, flags);
    system("sh /opt/mme/config/mme-init.sh");
    /* We dont expect quick updates from configmap..One update per interval */
    watch_config_change((char *)("/opt/mme/config/config.json"), mme_config_change_cbk, false);

    /* Parse the config again */
    mme_config new_config;
    mme_parse_config(&new_config); 
    if(strcmp(new_config.logging, g_mme_cfg.logging))
    {
        //Logging level changed 
        set_logging_level(new_config.logging);
		log_msg(LOG_INFO, "g_mme_cfg logging level %s", new_config.logging);
    }
    free(g_mme_cfg.logging); /* Release old logging level string */
    free(g_mme_cfg.mme_name); /* Relese old mme_name */
    g_mme_cfg = new_config; /* shallow copy of the structure */ 
}

void register_config_updates(void)
{
    /* I would prefer a complete path than this relative path.
     * Looks like it may break */
    watch_config_change((char *)("/opt/mme/config/config.json"), mme_config_change_cbk, false);
}


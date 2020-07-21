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

extern mme_config_t *mme_cfg;

void mme_config_change_cbk(char *config_file, uint32_t flags)
{
    // Run the script with this file. It generates new config for mme
	log_msg(LOG_INFO, "Received %s . File %s Flags %x \n", __FUNCTION__, config_file, flags);
    system((char*)("sh /opt/mme/config/mme-init.sh"));
    /* We dont expect quick updates from configmap..One update per interval */
    watch_config_change((char *)("/opt/mme/config/config.json"), mme_config_change_cbk, false);

    /* Parse the config again */
    mme_config_t *new_config = new (mme_config_t);
    assert(new_config != NULL);
    mmeConfig::mme_parse_config_new(new_config);
    if(strcmp(new_config->logging, mme_cfg->logging))
    {
        //Logging level changed 
        set_logging_level(new_config->logging);
		log_msg(LOG_INFO, "new logging level %s", new_config->logging);
    }
    mme_config_t *temp = mme_cfg;
    mme_cfg = new_config; 

    delete temp;
}

void register_config_updates(void)
{
    /* I would prefer a complete path than this relative path.
     * Looks like it may break */
    watch_config_change((char *)("/opt/mme/config/config.json"), mme_config_change_cbk, false);
}


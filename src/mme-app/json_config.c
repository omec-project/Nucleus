/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

extern "C"
{
#include "json_data.h"
}

#include "mme_app.h"
#include "err_codes.h"
#include "log.h"

/**
 * @brief Initialize json parser
 * @param None
 * @return void
 */
void
init_parser(char *path)
{
	load_json(path);
}

/* int_alg: "EIA1, EIA2, EIA0" */
static int
getIntAlgOrder(char *alg_list, uint8_t* alg_order)
{
    char *subString;
    log_msg(LOG_DEBUG, "alg_list : %s\n", alg_list); 
    subString = strtok(alg_list,"[]");
    log_msg(LOG_DEBUG, "substring : %s\n", subString); 
    //subString = strtok(NULL,"]");
    //log_msg(LOG_DEBUG, "substring : %s\n", subString); 

    char token[] = ",";
    char *saved_comma=NULL;
    char *alg[3] = {NULL,NULL,NULL};
    alg[0] = strtok_r(subString, token, &saved_comma);
    alg[1] = strtok_r(NULL, token, &saved_comma);
    alg[2] = strtok_r(NULL, token, &saved_comma);

    for(int i = 0;i < 3; i++)
    {
        log_msg(LOG_DEBUG, "algs : %s\n", alg[i]);
        if(alg[i] != NULL)
        {
            if(!strcmp(alg[i],"EIA0"))
            {
                alg_order[i] = 0;
            }
            else if(!strcmp(alg[i],"EIA1"))
            {
                alg_order[i] = 1;
            }
            else if(!strcmp(alg[i],"EIA2"))
            {
                alg_order[i] = 2;
            }
            else
            {
                alg_order[i] = 0;
            }
        }
        else
        {
            alg_order[i] = 0;
        }
    }
    
    return 0;
}


/* sec_alg: "EEA0, EEA1, EEA2" */
static int
getSecAlgOrder(char *alg_list, uint8_t* alg_order)
{
    char *subString;
    log_msg(LOG_DEBUG, "alg_list : %s\n", alg_list); 
    subString = strtok(alg_list,"[]");
    log_msg(LOG_DEBUG, "substring : %s\n", subString); 

    char token[] = ",";
    char *saved_comma=NULL;
    char *alg[3] = {NULL,NULL,NULL};
    char *temp_token = subString;
    char *temp;
    //while ((temp = strtok_r(temp_token, ",", &temp_token)))
    //    log_msg(LOG_DEBUG, "token : %s\n", temp); 
    
    alg[0] = strtok_r(subString, token, &saved_comma);
    alg[1] = strtok_r(NULL, token, &saved_comma);
    alg[2] = strtok_r(NULL, token, &saved_comma);

    for(int i = 0;i < 3; i++)
    {
        log_msg(LOG_DEBUG, "algs : %s\n", alg[i]);
        if(alg[i] != NULL)
        {
            if(!strcmp(alg[i],"EEA0"))
            {
                alg_order[i] = 0;
            }
            else if(!strcmp(alg[i],"EEA1"))
            {
                alg_order[i] = 1;
            }
            else if(!strcmp(alg[i],"EEA2"))
            {
                alg_order[i] = 2;
            }
            else
            {
                alg_order[i] = 0;
            }
        }
        else
        {
            alg_order[i] = 0;
        }
    }
    
    return 0;
}
/**
 * @brief parser mme-app input json file
 * @param None
 * @return int as SUCCESS or FAIL
 */
int
parse_mme_conf(mme_config *config)
{
	log_msg(LOG_INFO, "Parsing config %s \n", __FUNCTION__);
	/*mme own information*/
	config->mme_name = get_string_scalar((char *)("mme.name"));
	if(NULL == config->mme_name) return E_PARSING_FAILED;

	config->mme_ip_addr = get_ip_scalar((char *)("mme.ip_addr"));
	if(E_PARSING_FAILED == config->mme_ip_addr) return E_PARSING_FAILED;

	config->logging = get_string_scalar((char *)("mme.logging"));
	if(NULL == config->logging) 
    { 
	  log_msg(LOG_INFO, "Missing logging config");
      config->logging = (char *)calloc(1, strlen("debug")+1);
      strncpy(config->logging, "debug", strlen("debug")+1);
    }

	config->mcc_dig1 = get_int_scalar((char *)("mme.mcc.dig1"));
	if(E_PARSING_FAILED == config->mcc_dig1) return E_PARSING_FAILED;
	config->mcc_dig2 = get_int_scalar((char *)("mme.mcc.dig2"));
	if(E_PARSING_FAILED == config->mcc_dig1) return E_PARSING_FAILED;
	config->mcc_dig3 = get_int_scalar((char *)("mme.mcc.dig3"));
	if(E_PARSING_FAILED == config->mcc_dig1) return E_PARSING_FAILED;
	config->mcc_dig1 = get_int_scalar((char *)("mme.mnc.dig1"));
	if(E_PARSING_FAILED == config->mcc_dig1) return E_PARSING_FAILED;
	config->mnc_dig2 = get_int_scalar((char *)("mme.mnc.dig2"));
	if(E_PARSING_FAILED == config->mcc_dig1) return E_PARSING_FAILED;
	config->mnc_dig3 = get_int_scalar((char *)("mme.mnc.dig3"));
	if(E_PARSING_FAILED == config->mcc_dig1) return E_PARSING_FAILED;

	config->mme_group_id = get_int_scalar((char *)("mme.group_id"));
	if(-1 == config->mme_group_id) return -1;

	config->mme_code = get_int_scalar((char *)("mme.code"));
	if(-1 == config->mme_code) return -1;

	char* int_alg_list = get_string_scalar((char *)("mme.security.int_alg_list"));
	if(E_PARSING_FAILED == config->mcc_dig1) return E_PARSING_FAILED;
	char* sec_alg_list = get_string_scalar((char *)("mme.security.sec_alg_list"));
	if(E_PARSING_FAILED == config->mcc_dig1) return E_PARSING_FAILED;
    getIntAlgOrder(int_alg_list, config->integrity_alg_order);
    getSecAlgOrder(sec_alg_list, config->ciphering_alg_order);
	return SUCCESS;
}


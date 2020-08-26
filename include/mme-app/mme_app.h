/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __MME_APP_H_
#define __MME_APP_H_

#include <string>
#include <list>
#include <iostream>
#ifdef __cplusplus
extern "C"{
#endif

#include <stdbool.h>
#include "s1ap_structs.h"
#include "log.h"
#include "s1ap_error.h"
#include "defines.h"

/**
 * MME main application configuration parameters structures.
 * All fields in this will be filled in from input json file.
 */
/*
       security:
        integrity_order : [ EIA1, EIA2, EIA0 ]
        ciphering_order : [ EEA0, EEA1, EEA2 ]
*/
typedef struct mme_config
{
	unsigned int mme_ip_addr;
	unsigned short mme_sctp_port;
	unsigned int s11_sgw_ip;
	unsigned int s11_pgw_ip;
	unsigned short mme_egtp_def_port;
	char  *mme_egtp_def_hostname;
	char  mme_name[128];
    char  logging[16];
    uint8_t integrity_alg_order[3];
    uint8_t ciphering_alg_order[3];
	char  mcc_dig1;
	char  mcc_dig2;
	char  mcc_dig3;
	char  mnc_dig1;
	char  mnc_dig2;
	char  mnc_dig3;
	struct PLMN plmn_id;
	unsigned int mme_s1ap_ip;
	unsigned int mme_egtp_ip;
	unsigned short mme_group_id;
	unsigned char mme_code;
	uint16_t num_plmns;
	struct PLMN plmns[MAX_PLMN];
	struct PLMN_C plmn_mcc_mnc[MAX_PLMN];
    uint16_t prom_port;
} mme_config_t;

class apn_config
{
    private:
        std::string apn_name;
        std::string spgw_srv;
        uint16_t    failure_cnt;
        uint32_t    sgw_addr;
        uint32_t    pgw_addr;
        bool        dns_pending;
    public:
        apn_config(std::string apn, std::string service) 
        {
            apn_name = std::move(apn);
            spgw_srv = std::move(service);
            dns_pending = true;
            sgw_addr = 0; 
            pgw_addr=0; 
            failure_cnt = 0;
        }
        ~apn_config() {}
        uint16_t    get_failure_count() { return failure_cnt; }
        std::string get_apn()           { return apn_name; }
        std::string get_spgw_srv()      { return spgw_srv; }
        bool get_dns_state()            { return dns_pending; }
        uint32_t get_sgw_addr()         { return sgw_addr; }
        uint32_t get_pgw_addr()         { return pgw_addr; }
        void set_pgw_ip(uint32_t pgw_ip) { pgw_addr = pgw_ip; }
        void set_sgw_ip(uint32_t sgw_ip) { sgw_addr = sgw_ip; }
        void set_dns_resolved()         { dns_pending = false; }
        void set_dns_pending()          { dns_pending = true; }
};

class mmeConfig
{
    private:
        std::list<apn_config*> apn_list;
    public:

        void   add_apn(apn_config *t) { apn_list.push_back(t); }

        apn_config* find_apn(const std::string apn) 
        {
            std::list<apn_config*>::iterator it;
            apn_config *temp;
            std::cout<<"Find APN *"<<apn<<"*  length = "<<apn.length()<<std::endl;
            for(it = apn_list.begin(); it != apn_list.end(); it++) {
                temp = *it;
                std::cout<<"Current APN "<<temp->get_apn()<<std::endl;
                if(temp->get_apn().compare(apn) == 0)
                {
                    std::cout<<"Matched APN "<<temp->get_apn()<<std::endl;
                    return temp;
                }
                std::cout<<"Not matched APN "<<temp->get_apn()<<"length "<<temp->get_apn().length()<<std::endl;
            }
            return nullptr;
        }

        static void mme_parse_config_new(mme_config_t *);
        static int get_mcc_mnc(char *plmn, uint16_t *mcc_i, uint16_t *mnc_i, uint16_t *mnc_digits); 
        static int getIntAlgOrder(char *alg_list, uint8_t* alg_order);
        static int getSecAlgOrder(char *alg_list, uint8_t* alg_order);
        void initiate_spgw_resolution();
        void invalidate_dns();
};


const size_t fifoQSize_c = 1000;
const uint16_t MmeIpcInterfaceCompId = 1;
const uint16_t TimeoutManagerCompId = 2;

void stat_init();

/* Register for config change trigger */
void register_config_updates(void);
#ifdef __cplusplus
}
#endif


#endif /*__MME_APP_H_*/

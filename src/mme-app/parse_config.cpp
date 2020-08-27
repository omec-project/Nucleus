/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <list>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "mme_app.h"
#include <utils/mmeTimerUtils.h>
#include <utils/mmeTimerTypes.h>

using namespace mme;

extern mmeConfig *mme_tables;

/* int_alg: "EIA1, EIA2, EIA0" */
int
mmeConfig::getIntAlgOrder(char *alg_list, uint8_t* alg_order)
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
                alg_order[i] = 1;
            }
        }
        else
        {
            alg_order[i] = 1;
        }
    }
    
    return 0;
}


/* sec_alg: "EEA0, EEA1, EEA2" */
int
mmeConfig::getSecAlgOrder(char *alg_list, uint8_t* alg_order)
{
    char *subString;
    log_msg(LOG_DEBUG, "alg_list : %s\n", alg_list); 
    subString = strtok(alg_list,"[]");
    log_msg(LOG_DEBUG, "substring : %s\n", subString); 

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

int
mmeConfig::get_mcc_mnc(char *plmn, uint16_t *mcc_i, uint16_t *mnc_i, uint16_t *mnc_digits)
{
	const char *token = ",";
	char *saved_comma=NULL;
	char *mcc = strtok_r(plmn, token, &saved_comma);
	char *mnc = strtok_r(NULL, token, &saved_comma);

	char *saved_e=NULL;
	const char *token_e = "=";
	char *mcc_f = strtok_r(mcc, token_e, &saved_e);
	mcc_f = strtok_r(NULL, token_e, &saved_e);
	*mcc_i = atoi(mcc_f);

	saved_e=NULL;
	char *mnc_f = strtok_r(mnc, token_e, &saved_e);
	mnc_f = strtok_r(NULL, token_e, &saved_e);
    *mnc_digits = strlen(mnc_f);
	*mnc_i = atoi(mnc_f);
	return 0;
}

void 
mmeConfig::mme_parse_config_new(mme_config_t *config)
{
    FILE* fp = fopen("conf/mme.json", "r");
    if(fp == NULL){
        std::cout << "The json config file specified does not exists" << std::endl;
        return;
    }
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    config->prom_port = 3081;
    if(!doc.IsObject()) {
        std::cout << "Error parsing the json config file" << std::endl;
        return;
    }
    if(doc.HasMember("mme"))
    {
        const rapidjson::Value& mmeSection = doc["mme"];
        if(mmeSection.HasMember("name"))
        {
            strcpy(config->mme_name, mmeSection["name"].GetString());
        }
        if(mmeSection.HasMember("prom_port"))
        {
            config->prom_port = mmeSection["prom_port"].GetInt();
        }
        if(mmeSection.HasMember("group_id"))
        {
            config->mme_group_id = mmeSection["group_id"].GetInt();
        }
        if(mmeSection.HasMember("code"))
        {
            config->mme_code = mmeSection["code"].GetInt();
        }
        if(mmeSection.HasMember("logging"))
        {
            strcpy(config->logging, mmeSection["logging"].GetString());
        }
        if(mmeSection.HasMember("mcc"))
        {
            const rapidjson::Value& mccSection = mmeSection["mcc"];
            if(mccSection.HasMember("dig1"))
            {
                config->mcc_dig1 = mccSection["dig1"].GetInt(); 
            }
            if(mccSection.HasMember("dig2"))
            {
                config->mcc_dig2 = mccSection["dig2"].GetInt(); 
            }
            if(mccSection.HasMember("dig3"))
            {
                config->mcc_dig3 = mccSection["dig3"].GetInt(); 
            }
        }   
        if(mmeSection.HasMember("mnc"))
        {
            const rapidjson::Value& mncSection = mmeSection["mnc"];
            if(mncSection.HasMember("dig1"))
            {
                config->mnc_dig1 = mncSection["dig1"].GetInt(); 
            }
            if(mncSection.HasMember("dig2"))
            {
                config->mnc_dig2 = mncSection["dig2"].GetInt(); 
            }
            if(mncSection.HasMember("dig3"))
            {
                config->mnc_dig3 = mncSection["dig3"].GetInt(); 
            }
        }
        if(mmeSection.HasMember("plmnlist"))
        {
            int count = 1;
            const rapidjson::Value &plmn = mmeSection["plmnlist"];
            for (rapidjson::Value::ConstMemberIterator plmnitr = plmn.MemberBegin(); plmnitr != plmn.MemberEnd(); ++plmnitr)
            {
                rapidjson::StringBuffer sb;
                rapidjson::Writer<rapidjson::StringBuffer> writer( sb );
                plmnitr->value.Accept(writer);
                char plmn[100];
                strcpy(plmn,sb.GetString());
                uint16_t mcc_i, mnc_i, mnc_digits=3;
                mmeConfig::get_mcc_mnc(plmn, &mcc_i, &mnc_i, &mnc_digits);
                config->plmn_mcc_mnc[count-1].mcc = mcc_i;
                config->plmn_mcc_mnc[count-1].mnc = mnc_i;
                log_msg(LOG_INFO, "Parsed plmn mcc - %d mnc - %d \n", mcc_i, mnc_i);
                unsigned char mcc_dig_1 = mcc_i / 100; 
                unsigned char mcc_dig_2 = (mcc_i / 10) % 10; 
                unsigned char mcc_dig_3 = mcc_i % 10; 
                unsigned char mnc_dig_1; 
                unsigned char mnc_dig_2;
                unsigned char mnc_dig_3;
                if(mnc_digits == 2) // 01
                {
                    mnc_dig_1 = 0x0F;
                    mnc_dig_2 = mnc_i / 10;
                }
                else
                {
                    mnc_dig_1 = mnc_i / 100;
                    mnc_dig_2 = (mnc_i / 10) % 10; 
                }
                mnc_dig_3 = mnc_i % 10;
                config->plmns[count-1].idx[0] = (mcc_dig_2 << 4) | (mcc_dig_1);
                config->plmns[count-1].idx[1] = (mnc_dig_1 << 4) | (mcc_dig_3);
                config->plmns[count-1].idx[2] = (mnc_dig_3 << 4) | (mnc_dig_2);
                config->plmns[count-1].mnc_digits = mnc_digits;
                log_msg(LOG_INFO, "Configured plmn %x %x %x \n", config->plmns[count-1].idx[0], config->plmns[count-1].idx[1], config->plmns[count-1].idx[2]); 
            }
        }
        if(mmeSection.HasMember("security"))
        {
            const rapidjson::Value &securitySection = mmeSection["security"];
            if(securitySection.HasMember("int_alg_list"))
            {
                std::string intAlgList = securitySection["int_alg_list"].GetString();
                char alg_list[100];
                strcpy(alg_list, intAlgList.c_str());
                getIntAlgOrder(alg_list, config->integrity_alg_order);
            } else {
                config->integrity_alg_order[0] = 1;
                config->integrity_alg_order[1] = 1;
                config->integrity_alg_order[2] = 1;
            }

            if(securitySection.HasMember("sec_alg_list"))
            {
                std::string secAlgList = securitySection["sec_alg_list"].GetString();
                char alg_list[100];
                strcpy(alg_list, secAlgList.c_str());
                getSecAlgOrder(alg_list, config->ciphering_alg_order);
            } else {
                config->ciphering_alg_order[0] = 0;
                config->ciphering_alg_order[1] = 0;
                config->ciphering_alg_order[2] = 0;
            }
        }
        else
        {
            config->integrity_alg_order[0] = 1;
            config->integrity_alg_order[1] = 1;
            config->integrity_alg_order[2] = 1;
            config->ciphering_alg_order[0] = 0;
            config->ciphering_alg_order[1] = 0;
            config->ciphering_alg_order[2] = 0;
        }

        if(mmeSection.HasMember("apnlist"))
        {
            const rapidjson::Value &apn = mmeSection["apnlist"];
            for (rapidjson::Value::ConstMemberIterator apnitr = apn.MemberBegin(); apnitr != apn.MemberEnd(); ++apnitr)
            {
                std::string apn = apnitr->name.GetString();
                std::string spgw = apnitr->value.GetString();

                log_msg(LOG_INFO, "Configured service %s\n", spgw.c_str());
                log_msg(LOG_INFO, "Configured apn %s \n", apn.c_str()); 
                // add apn==>{ spgw service,  address, failures }  in the map 
                apn_config *ap1 = new apn_config(apn, spgw);
                mme_tables->add_apn(ap1);
            }
        }
    }
    /* Print parsed configuraton */
    log_msg(LOG_DEBUG,"mme_name : %s \n", config->mme_name);
    log_msg(LOG_DEBUG,"mme_groupid : %d \n", config->mme_group_id);
    log_msg(LOG_DEBUG,"mme_code : %d \n", config->mme_code);
    log_msg(LOG_DEBUG,"logging  : %s \n", config->logging);
    log_msg(LOG_DEBUG,"MCC :    %d %d %d \n", config->mcc_dig1, config->mcc_dig2, config->mcc_dig3);
    log_msg(LOG_DEBUG,"MNC :    %d %d %d \n", config->mnc_dig1, config->mnc_dig2, config->mnc_dig3);
    for(int i=0; i<config->num_plmns;i++)
    {
        log_msg(LOG_DEBUG,"PLMN(%d) :    %d %d %d \n", i, config->plmns[i].idx[0], config->plmns[i].idx[1], config->plmns[i].idx[2]);
    }
    mme_tables->initiate_spgw_resolution();
    return ;
}

void mmeConfig::invalidate_dns()
{
    apn_config *temp;
    std::list<apn_config*>::iterator it;

    for(it = apn_list.begin(); it != apn_list.end(); it++) 
    {
        temp = *it;
        temp->set_dns_pending();
    }
    return;
}

/* One resolution at a time with timer retry */
void mmeConfig::initiate_spgw_resolution()
{
    std::list<apn_config*>::iterator it;
    bool started = false;
    apn_config *temp;

    for(it = apn_list.begin(); it != apn_list.end(); it++) 
    {
        temp = *it;
        if(temp->get_dns_state() == false)
        {
            struct in_addr p = {0};
            struct in_addr s = {0};
            p.s_addr = temp->get_pgw_addr();
            s.s_addr = temp->get_sgw_addr();
            log_msg(LOG_DEBUG, "SGW address = %s, PGW address = %s ",inet_ntoa(p), inet_ntoa(s));
            continue;
        }
        struct addrinfo hints;
        struct addrinfo *result=NULL, *rp=NULL; 
        int err;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
        hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        hints.ai_protocol = 0;          /* Any protocol */
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;
        err = getaddrinfo(temp->get_spgw_srv().c_str(), NULL, &hints, &result);
        if (err != 0) 
        {
            // Keep trying ...May be SGW is not yet deployed 
            // We shall be doing this once timer library is integrated 
            log_msg(LOG_ERROR, "%s - getaddr info failed %s\n",temp->get_spgw_srv().c_str(), gai_strerror(err));
            if(started == false)
              MmeTimerUtils::startTimer(10000, 1 /* ue_index */, mmeConfigDnsResolve_c, 1 /*timer type*/);
            started = true;
        }
        else 
        {
            for (rp = result; rp != NULL; rp = rp->ai_next) 
            {
                if(rp->ai_family == AF_INET)
                {
                    struct sockaddr_in *addrV4 = (struct sockaddr_in *)rp->ai_addr;
                    log_msg(LOG_INFO, "gw address received from DNS response %s\n", inet_ntoa(addrV4->sin_addr));
                    temp->set_pgw_ip(addrV4->sin_addr.s_addr);
                    temp->set_sgw_ip(addrV4->sin_addr.s_addr);
                    temp->set_dns_resolved();
                    break;
                }
            }
        }
    }
}


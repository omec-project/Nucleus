#include <iostream>
#include <cstdio>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "mme_app.h"

using namespace rapidjson;

static int
get_mcc_mnc(char *plmn, uint16_t *mcc_i, uint16_t *mnc_i, uint16_t *mnc_digits)
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
mme_parse_config_new(mme_config_t *config)
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
                get_mcc_mnc(plmn, &mcc_i, &mnc_i, &mnc_digits);
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
                log_msg(LOG_INFO, "Configured plmn %x %x %x", config->plmns[count-1].idx[0], config->plmns[count-1].idx[1], config->plmns[count-1].idx[2]); 
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
    return ;
}


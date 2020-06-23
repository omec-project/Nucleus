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
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    Document d;
    d.ParseStream(is);
    std::cout<<"key mme "<< d.HasMember("mme")<<std::endl;
    const rapidjson::Value& itemn = d["mme"];

    for (rapidjson::Value::ConstMemberIterator itr = itemn.MemberBegin(); itr != itemn.MemberEnd(); ++itr)
    {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer( sb );
        itr->value.Accept(writer);
        std::cout <<"Object = "<<itr->value.IsObject()<<" "<<itr->name.GetString()<< " "<< sb.GetString()<<std::endl;
        if(strcmp(itr->name.GetString(), "name") == 0)
        {
            strcpy(config->mme_name, sb.GetString());
        }
        else if(strcmp(itr->name.GetString(), "group_id") == 0)
        {
            config->mme_group_id = std::stoi(sb.GetString());
        }
        else if(strcmp(itr->name.GetString(), "code") == 0)
        {
            config->mme_code = std::stoi(sb.GetString());
        }
        else if(strcmp(itr->name.GetString(), "logging") == 0)
        {
            strcpy(config->logging, sb.GetString());
        }	
        else if(strcmp(itr->name.GetString(),"mcc") == 0)
        {
            std::cout<<"Found mcc list "<<std::endl;
            const rapidjson::Value &mcc = itr->value;
            for (rapidjson::Value::ConstMemberIterator mccitr = mcc.MemberBegin(); mccitr != mcc.MemberEnd(); ++mccitr)
            {
                rapidjson::StringBuffer sb;
                rapidjson::Writer<rapidjson::StringBuffer> writer( sb );
                mccitr->value.Accept(writer);
                if(strcmp(mccitr->name.GetString(),"dig1") == 0)
                    config->mcc_dig1 = std::stoi(sb.GetString()); 
                else if(strcmp(mccitr->name.GetString(),"dig2") == 0)
                    config->mcc_dig2 = std::stoi(sb.GetString()); 
                else if(strcmp(mccitr->name.GetString(),"dig3") == 0)
                    config->mcc_dig3 = std::stoi(sb.GetString()); 
                std::cout <<"	Object = "<<mccitr->value.IsObject()<<" "<<mccitr->name.GetString()<< " "<< sb.GetString()<<std::endl;
            }
        }
        else if(strcmp(itr->name.GetString(),"mnc") == 0)
        {
            std::cout<<"Found mnc list "<<std::endl;
            const rapidjson::Value &mnc = itr->value;
            for (rapidjson::Value::ConstMemberIterator mncitr = mnc.MemberBegin(); mncitr != mnc.MemberEnd(); ++mncitr)
            {
                rapidjson::StringBuffer sb;
                rapidjson::Writer<rapidjson::StringBuffer> writer( sb );
                mncitr->value.Accept(writer);
                if(strcmp(mncitr->name.GetString(),"dig1") == 0)
                    config->mnc_dig1 = std::stoi(sb.GetString()); 
                else if(strcmp(mncitr->name.GetString(),"dig2") == 0)
                    config->mnc_dig2 = std::stoi(sb.GetString()); 
                else if(strcmp(mncitr->name.GetString(),"dig3") == 0)
                    config->mnc_dig3 = std::stoi(sb.GetString()); 
                std::cout <<"	Object = "<<mncitr->value.IsObject()<<" "<<mncitr->name.GetString()<< " "<< sb.GetString()<<std::endl;
            }
        }
        else if(strcmp(itr->name.GetString(),"plmnlist") == 0)
        {
            std::cout<<"Found plmn list "<<std::endl;
            int count = 1;
            const rapidjson::Value &plmn = itr->value;
            for (rapidjson::Value::ConstMemberIterator plmnitr = plmn.MemberBegin(); plmnitr != plmn.MemberEnd(); ++plmnitr)
            {
                rapidjson::StringBuffer sb;
                rapidjson::Writer<rapidjson::StringBuffer> writer( sb );
                plmnitr->value.Accept(writer);
                std::cout <<"	Object = "<<plmnitr->value.IsObject()<<" "<<plmnitr->name.GetString()<< " "<< sb.GetString()<<std::endl;
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
    fclose(fp);
    return ;
}


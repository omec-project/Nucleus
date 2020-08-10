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
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "s11_config.h"
extern "C"
{
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "monitor_config.h"
#include "log.h"
}

void get_addr_from_str(const char *name, uint32_t *addr);

extern s11_config_t *s11_cfg;
void 
s11Config::s11_parse_config(s11_config_t *config)
{
    FILE* fp = fopen("conf/s11.json", "r");
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
        if(mmeSection.HasMember("logging"))
        {
            strcpy(config->logging, mmeSection["logging"].GetString());
        }
    }    
    if(doc.HasMember("s11"))
    {
        const rapidjson::Value& s11Section = doc["s11"];
        if(s11Section.HasMember("egtp_local_addr"))
        {
            std::string addr;
            addr = s11Section["egtp_local_addr"].GetString();
            config->local_egtp_ip = 0;
            struct in_addr temp = {0};
            inet_aton(addr.c_str(), &temp);
            config->local_egtp_ip = ntohl(temp.s_addr); 
        }
        if(s11Section.HasMember("egtp_default_port"))
        {
            config->egtp_def_port = s11Section["egtp_default_port"].GetInt();
            config->egtp_def_port = config->egtp_def_port;
        }
        if(s11Section.HasMember("sgw_addr"))
        {
            std::string addr;
            addr = s11Section["sgw_addr"].GetString();
            config->sgw_ip = 0;
            get_addr_from_str(addr.c_str(), &config->sgw_ip);
        }
        if(s11Section.HasMember("pgw_addr"))
        {
            std::string addr;
            addr = s11Section["pgw_addr"].GetString();
            config->pgw_ip = 0;
            get_addr_from_str(addr.c_str(), &config->pgw_ip);
        }
    }
    /* Print parsed configuraton */
    //log_msg(LOG_DEBUG,"logging  : %s \n", config->logging);
    
    config->num_retransmission = 3;
    config->retransmission_interval = 3000; // 3 sec 
    return ;
}

void s11_config_change_cbk(char *config_file, uint32_t flags)
{
    // Run the script with this file. It generates new config for mme
	log_msg(LOG_INFO, "Received %s . File %s Flags %x \n", __FUNCTION__, config_file, flags);
    system((char*)("sh /opt/mme/config/mme-init.sh"));
    /* We dont expect quick updates from configmap..One update per interval */
    watch_config_change((char *)("/opt/mme/config/config.json"), s11_config_change_cbk, false);

    /* Parse the config again */
    s11_config_t *new_config = new (s11_config_t);
    assert(new_config != NULL);
    s11Config::s11_parse_config(new_config);
    if(strcmp(new_config->logging, new_config->logging))
    {
        //Logging level changed 
        set_logging_level(new_config->logging);
		log_msg(LOG_INFO, "new logging level %s", new_config->logging);
    }
    s11_config_t *temp = s11_cfg;

    // switch to new config 
    s11_cfg = new_config; 

    delete temp;
}

void s11Config::register_config_updates(void)
{
    /* I would prefer a complete path than this relative path.
     * Looks like it may break */
    watch_config_change((char *)("/opt/mme/config/config.json"), s11_config_change_cbk, false);
}

void
get_addr_from_str(const char *name, uint32_t *addr)
{
    if(name != NULL)
    {
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
        err = getaddrinfo(name, NULL, &hints, &result);
        if (err != 0) 
        {
            log_msg(LOG_INFO, "getaddr info failed for %s with error %s\n",name, gai_strerror(err));
        }
        else 
        {
            for (rp = result; rp != NULL; rp = rp->ai_next) 
            {
                if(rp->ai_family == AF_INET)
                {
                    struct sockaddr_in *addrV4 = (struct sockaddr_in *)rp->ai_addr;
                    log_msg(LOG_INFO, "gw name (%s) resolved  to %s\n", name, inet_ntoa(addrV4->sin_addr));
                    *addr = addrV4->sin_addr.s_addr;
                    return;
                }
            }
        }
    }
    log_msg(LOG_INFO, "failed to resolve configured sgw address %s \n", name);
    return;
}

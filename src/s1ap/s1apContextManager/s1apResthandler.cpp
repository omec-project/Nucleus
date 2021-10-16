/*
* Copyright 2021-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*/


#include "s1apResthandler.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include <pistache/endpoint.h>

int configVersion;
char *plmns_cpp[1000];
int num_plmns_cpp;

void RestHandler::onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response)
{
    static bool needConfig = true;

    if (request.resource() == "/v1/config-check") {
      if (needConfig == true) {
         response.send(Pistache::Http::Code::Not_Found);
         needConfig = false;
      } else {
         response.send(Pistache::Http::Code::Ok);
      }
    } else if (request.resource() == "/v1/config/slice") {
        rapidjson::Document doc;
        doc.Parse(request.body().c_str());
        if(doc.HasParseError())
        {
                std::stringstream ss;
                ss << "Body parsing error offset="
                        << doc.GetErrorOffset()
                        << " error=" << rapidjson::GetParseError_En(doc.GetParseError());
                response.send(Pistache::Http::Code::Bad_Request, ss.str());
                return;
        }
        if(doc.HasMember("plmnlist"))
        {
            for(uint32_t i=0; i< doc["plmnlist"].Size();i++)
            {
                const rapidjson::Value& plmnName = doc["plmnlist"][i];
                std::string key = plmnName.GetString();
                std::cout<<"plmn "<< key.c_str()<<std::endl;
                if (plmns_cpp[i] == NULL) {
                    plmns_cpp[i] = (char *)malloc(32);
                }
                strncpy(plmns_cpp[i],key.c_str(), strlen(key.c_str())); 
            }
            num_plmns_cpp = doc["plmnlist"].Size();
            std::cout<<"Number of plmns "<< num_plmns_cpp<<std::endl;
        }
        needConfig = false;
        configVersion++;
        response.send(Pistache::Http::Code::Ok);
    } else {
      std::stringstream ss;
      ss << "Unrecognized resource [" << request.resource() << "]";
      response.send(Pistache::Http::Code::Bad_Request, ss.str() );
    }

    return;
}

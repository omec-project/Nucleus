/*
* Copyright 2019-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*/


#include "mmeappResthandler.h"
#include "rapidjson/error/en.h"
#include <pistache/endpoint.h>

void RestHandler::onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response)
{
    std::cout<<"Received message on http interface\n";
    std::cout << request.resource() << std::endl;
    std::cout << request.method() << std::endl;
    std::cout << request.body() << std::endl;
    std::stringstream ss;
    ss << "Unrecognized resource [" << request.resource() << "]";
    response.send(Pistache::Http::Code::Bad_Request, ss.str() );
}

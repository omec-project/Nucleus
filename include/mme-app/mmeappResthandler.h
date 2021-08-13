/*
* Copyright 2019-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*/



#ifndef _MME_RESTHANDLER_H_
#define _MME_RESTHANDLER_H_ 

#include <memory>
#include <pistache/endpoint.h>
#include <pistache/router.h>

class RestHandler : public Pistache::Http::Handler {

public:
    HTTP_PROTOTYPE(RestHandler)
    void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response);
};

#endif 

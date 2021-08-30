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

extern int configVersion;
extern int num_plmns_cpp;
extern char *plmns_cpp[1000];

class RestHandler : public Pistache::Http::Handler {

public:
    HTTP_PROTOTYPE(RestHandler)
    void onRequest(const Pistache::Http::Request& request, Pistache::Http::ResponseWriter response);
};

#endif 

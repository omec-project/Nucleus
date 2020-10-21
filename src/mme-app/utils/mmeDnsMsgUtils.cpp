/*
* Copyright 2020-present, Infosys Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "utils/mmeDnsMsgUtils.h"

#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <mme_app.h>
#include "epc/epcdns.h"

using namespace mme;

extern mme_config_t *mme_cfg;


void MmeDnsMsgUtils::Ipv4_gateway_ip( EPCDNS::StringVector &ipv4_ip, unsigned long * sgw_ip)
{
        EPCDNS::StringVector::const_iterator it = ipv4_ip.begin();
        char * result = (char*)ipv4_ip[0].c_str();
        unsigned long tmp = htonl(inet_addr(result));
        memcpy(sgw_ip,&tmp, sizeof(unsigned long));
}
void MmeDnsMsgUtils::Extract_IPs( EPCDNS::NodeSelectorResultList &at, unsigned long  *sgw_ip)
{
        EPCDNS::NodeSelectorResultList::const_iterator it = at.begin();
        Ipv4_gateway_ip((*it)->getIPv4Hosts(), sgw_ip);
}

void MmeDnsMsgUtils::SetDNSConfiguration()
{
        DNS::Cache::setRefreshConcurrent( mme_cfg->dns_config.concurrent);
        DNS::Cache::setRefreshPercent( mme_cfg->dns_config.percentage);
        DNS::Cache::setRefreshInterval( mme_cfg->dns_config.interval_seconds );
	std::cout<< "address of dns server" << mme_cfg->dns_config.dns1_ip;
        DNS::Cache::getInstance().addNamedServer(mme_cfg->dns_config.dns1_ip);
        DNS::Cache::getInstance().applyNamedServers();

}



/*
* Copyright (c) 2020  Great Software Laboratory Pvt. Ltd.
*
* SPDX-License-Identifier: Apache-2.0
*/
#ifndef INCLUDE_MME_APP_UTILS_MMEDNSMSGUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMEDNSMSGUTILS_H_



#include <stdint.h>
#include <msgType.h>
namespace mme
{
	class MmeDnsMsgUtils
	{
	public:

		static void Ipv4_gateway_ip( EPCDNS::StringVector &ipv4_ip, unsigned long * sgw_ip);

		static void Extract_IPs( EPCDNS::NodeSelectorResultList &at, unsigned long  *sgw_ip);
		
		static void SetDNSConfiguration();	
	private:
		MmeDnsMsgUtils();
		~MmeDnsMsgUtils();
	};
}

#endif /* INCLUDE_MME_APP_UTILS_MMEDNSMSGUTILS_H_ */

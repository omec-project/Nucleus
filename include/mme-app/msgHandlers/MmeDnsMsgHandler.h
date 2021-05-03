/*
 * Copyright (c) 2020  Great Software Laboratory Pvt. Ltd.  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef INCLUDE_MME_APP_MSGHANDLERS_MMEDNSMSGHANDLER_H_
#define INCLUDE_MME_APP_MSGHANDLERS_MMESNDMSGHANDLER_H_

#include <stdint.h>

class MmeDnsMsgHandler {
public:
	static MmeDnsMsgHandler* Instance();
	virtual ~MmeDnsMsgHandler();

	void handleDnsResponse(uint64_t sgw_ip, uint32_t ueIdx);

private:
	MmeDnsMsgHandler();


};

#endif /* INCLUDE_MME_APP_MSGHANDLERS_MMEDNSMSGHANDLER_H_ */

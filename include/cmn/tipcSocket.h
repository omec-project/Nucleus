 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_CMN_TIPCSOCKET_H_
#define INCLUDE_CMN_TIPCSOCKET_H_

#include "ipcChannel.h"

namespace cmn {
namespace ipc {

class TipcSocket: public IpcChannel {
public:
	TipcSocket();
	virtual ~TipcSocket();

	bool bindTipcSocket(IpcAddress myAddress);

    virtual void sendMsgTo(void * buffer, uint32_t len, IpcAddress destAddr);
    virtual uint32_t recvMsgFrom(void * buffer, uint32_t len, IpcAddress& srcAddr);

private:
    void initialize();

};

} /* namespace ipc */
} /* namespace cmn */

#endif /* INCLUDE_CMN_TIPCSOCKET_H_ */

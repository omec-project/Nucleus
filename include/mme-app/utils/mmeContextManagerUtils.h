/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INCLUDE_MME_APP_UTILS_MMECONTEXTMANAGERUTILS_H_
#define INCLUDE_MME_APP_UTILS_MMECONTEXTMANAGERUTILS_H_

#include <utils/mmeProcedureTypes.h>
#include <stdint.h>

namespace SM
{
class ControlBlock;
}

namespace mme
{

class MmeProcedureCtxt;
class S1HandoverProcedureContext;
class MmeContextManagerUtils
{
public:

    static MmeSvcReqProcedureCtxt*
    allocateServiceRequestProcedureCtxt( SM::ControlBlock& cb_r, PagingTrigger pagingTrigger);

    static MmeTauProcedureCtxt*
    allocateTauProcedureCtxt(SM::ControlBlock& cb_r);

    static MmeProcedureCtxt* findProcedureCtxt(SM::ControlBlock& cb_r, ProcedureType procType);

	static bool deleteProcedureCtxt(MmeProcedureCtxt* procedure_p);
	static bool deallocateProcedureCtxt(SM::ControlBlock& cb_r, ProcedureType procType);
	static bool deallocateAllProcedureCtxts(SM::ControlBlock& cb_r);

	static void deleteUEContext(uint32_t cbIndex);
	static void deleteSessionContext(SM::ControlBlock& cb_r);

	static S1HandoverProcedureContext* allocateHoContext(SM::ControlBlock& cb_r);

private:
	MmeContextManagerUtils();
	~MmeContextManagerUtils();
};
}



#endif /* INCLUDE_MME_APP_UTILS_MMECONTEXTMANAGERUTILS_H_ */

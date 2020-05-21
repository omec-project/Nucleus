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

#include <controlBlock.h>
#include <contextManager/subsDataGroupManager.h>
#include <log.h>
#include <mmeStates/pagingStart.h>
#include <mmeStates/serviceRequestStart.h>
#include <mmeStates/tauStart.h>
#include <utils/mmeContextManagerUtils.h>
#include <mmeStates/intraS1HoStart.h>

using namespace mme;

MmeSvcReqProcedureCtxt*
MmeContextManagerUtils::allocateServiceRequestProcedureCtxt(SM::ControlBlock& cb_r, PagingTrigger pagingTrigger)
{
    log_msg(LOG_DEBUG, "allocateServiceRequestProcedureCtxt: Entry");

    MmeSvcReqProcedureCtxt *prcdCtxt_p =
            SubsDataGroupManager::Instance()->getMmeSvcReqProcedureCtxt();
    if (prcdCtxt_p != NULL)
    {
        prcdCtxt_p->setCtxtType(ProcedureType::serviceRequest_c);
        prcdCtxt_p->setPagingTrigger(pagingTrigger);

        if (pagingTrigger == ddnInit_c)
        {
            prcdCtxt_p->setNextState(PagingStart::Instance());
        }
        else
        {
            prcdCtxt_p->setNextState(ServiceRequestStart::Instance());
        }

        cb_r.setCurrentTempDataBlock(prcdCtxt_p);

    }
    return prcdCtxt_p;
}

MmeTauProcedureCtxt*
MmeContextManagerUtils::allocateTauProcedureCtxt(SM::ControlBlock& cb_r)
{
    log_msg(LOG_DEBUG, "allocateTauRequestProcedureCtxt: Entry");

    MmeTauProcedureCtxt *prcdCtxt_p =
            SubsDataGroupManager::Instance()->getMmeTauProcedureCtxt();
    if (prcdCtxt_p != NULL)
    {
        prcdCtxt_p->setCtxtType(ProcedureType::tau_c);
        prcdCtxt_p->setNextState(TauStart::Instance());

        cb_r.setCurrentTempDataBlock(prcdCtxt_p);
    }

    return prcdCtxt_p;
}

bool MmeContextManagerUtils::deleteProcedureCtxt(MmeProcedureCtxt* procedure_p)
{
	log_msg(LOG_DEBUG, "deleteProcedureCtxt: Entry");

	if (procedure_p == NULL)
	{
		log_msg(LOG_INFO, "Procedure Context is NULL");

		return false;
	}

	SubsDataGroupManager* subsDgMgr_p = SubsDataGroupManager::Instance();

	log_msg(LOG_INFO, "Procedure Type is %d", procedure_p->getCtxtType());

	bool rc = true;
	switch (procedure_p->getCtxtType())
	{
		case attach_c:
                {
                        MmeAttachProcedureCtxt* atchProc_p =
                                static_cast<MmeAttachProcedureCtxt *>(procedure_p);

                        subsDgMgr_p->deleteMmeAttachProcedureCtxt(atchProc_p);
                        break;
                }
                case s1Release_c:
                {
                        MmeS1RelProcedureCtxt* s1RelProc_p =
                                static_cast<MmeS1RelProcedureCtxt *>(procedure_p);

                        subsDgMgr_p->deleteMmeS1RelProcedureCtxt(s1RelProc_p);
                        break;
                }	
		case detach_c:
		{
			MmeDetachProcedureCtxt* detachProc_p = 
				static_cast<MmeDetachProcedureCtxt *>(procedure_p);

			subsDgMgr_p->deleteMmeDetachProcedureCtxt(detachProc_p);

			break;
		}
		case serviceRequest_c:
		{
			MmeSvcReqProcedureCtxt* svcReqProc_p =
					static_cast<MmeSvcReqProcedureCtxt*>(procedure_p);

			subsDgMgr_p->deleteMmeSvcReqProcedureCtxt(svcReqProc_p);

			break;
		}
		case tau_c:
		{
			MmeTauProcedureCtxt* tauProc_p =
					static_cast<MmeTauProcedureCtxt*>(procedure_p);

			subsDgMgr_p->deleteMmeTauProcedureCtxt(tauProc_p);

			break;
		}
		case s1Handover_c:
		{
			S1HandoverProcedureContext* s1HoProc_p =
					static_cast<S1HandoverProcedureContext*>(procedure_p);

			subsDgMgr_p->deleteS1HandoverProcedureContext(s1HoProc_p);

			break;
		}
		default:
		{
			log_msg(LOG_INFO, "Unsupported procedure type %d\n", procedure_p->getCtxtType());
			rc = false;
		}
	}
	return rc;
}

bool MmeContextManagerUtils::deallocateProcedureCtxt(SM::ControlBlock& cb_r, ProcedureType procType)
{
    bool rc = false;

	MmeProcedureCtxt* procedure_p =
			static_cast<MmeProcedureCtxt*>(cb_r.getTempDataBlock());

	MmeProcedureCtxt* prevProcedure_p = NULL;
	MmeProcedureCtxt* nextProcedure_p = NULL;

    while (procedure_p != NULL)
    {
        nextProcedure_p =
            static_cast<MmeProcedureCtxt*>(procedure_p->getNextTempDataBlock());
        
        ProcedureType procedureType = procedure_p->getCtxtType();
        if (procType == procedureType)
        {
            log_msg(LOG_INFO, "Procedure type %d\n", procedureType);
            
            rc = deleteProcedureCtxt(procedure_p);
            
            if (rc == true)
            {
                if (prevProcedure_p != NULL)
                {
                    if (nextProcedure_p != NULL)
                    {
                        prevProcedure_p->setNextTempDataBlock(nextProcedure_p);
                    }
                }
                else
                {
                    cb_r.setTempDataBlock(nextProcedure_p);
                }
            }
            // break out of while loop
            break;
        }
        prevProcedure_p = procedure_p;
        procedure_p = nextProcedure_p;		
    }

    return rc;
}

bool MmeContextManagerUtils::deallocateAllProcedureCtxts(SM::ControlBlock& cb_r)
{
    bool rc = false;

    MmeProcedureCtxt* procedure_p =
    		static_cast<MmeProcedureCtxt*>(cb_r.getTempDataBlock());
    
    MmeProcedureCtxt* nextProcedure_p = NULL;
    
    while (procedure_p != NULL)
    {
        nextProcedure_p =
    		static_cast<MmeProcedureCtxt*>(procedure_p->getNextTempDataBlock());
    
        if (procedure_p->getCtxtType() != defaultMmeProcedure_c)
        {
            rc = deleteProcedureCtxt(procedure_p);
        }

        procedure_p = nextProcedure_p;
    }
    return rc;
}

MmeProcedureCtxt* MmeContextManagerUtils::findProcedureCtxt(SM::ControlBlock& cb_r, ProcedureType procType)
{
    MmeProcedureCtxt* mmeProcCtxt_p = NULL;

    MmeProcedureCtxt* currentProcedure_p =
                static_cast<MmeProcedureCtxt*>(cb_r.getTempDataBlock());

    MmeProcedureCtxt* nextProcedure_p = NULL;

    while (currentProcedure_p != NULL)
    {
        nextProcedure_p = static_cast<MmeProcedureCtxt*>(
                currentProcedure_p->getNextTempDataBlock());

        if (currentProcedure_p->getCtxtType() == procType)
        {
            mmeProcCtxt_p = currentProcedure_p;
            break;
        }
        currentProcedure_p = nextProcedure_p;
    }

    return mmeProcCtxt_p;
}

void MmeContextManagerUtils::deleteSessionContext(SM::ControlBlock& cb_r)
{
    UEContext* ueCtxt_p = static_cast<UEContext *>(cb_r.getPermDataBlock());
    if (ueCtxt_p == NULL)
    {
        log_msg(LOG_DEBUG, "Failed to retrieve UEContext from control block %u", cb_r.getCBIndex());
        return;
    }

    SessionContext* sessCtxt_p = ueCtxt_p->getSessionContext();
    if (sessCtxt_p == NULL)
    {
        log_msg(LOG_DEBUG, "Failed to retrieve SessionContext from UEContext %u", cb_r.getCBIndex());
        return;
    }

    BearerContext* bearerCtxt_p = sessCtxt_p->getBearerContext();
    if(bearerCtxt_p != NULL)
    {
        log_msg(LOG_INFO, "Deallocating bearer context for UE block %u", cb_r.getCBIndex());

        SubsDataGroupManager::Instance()->deleteBearerContext(bearerCtxt_p);
        sessCtxt_p->setBearerContext(NULL);
    }

    log_msg(LOG_INFO, "Deallocating session context for UE block %u", cb_r.getCBIndex());

    SubsDataGroupManager::Instance()->deleteSessionContext(sessCtxt_p);
    ueCtxt_p->setSessionContext(NULL);
}

void MmeContextManagerUtils::deleteUEContext(uint32_t cbIndex)
{
    SM::ControlBlock* cb_p = SubsDataGroupManager::Instance()->findControlBlock(cbIndex);
    if (cb_p == NULL)
    {
        log_msg(LOG_DEBUG, "Failed to find control block for index %u", cbIndex);
        return;
    }

    deallocateAllProcedureCtxts(*cb_p);

    deleteSessionContext(*cb_p);

    UEContext* ueCtxt_p = static_cast<UEContext *>(cb_p->getPermDataBlock());
    if (ueCtxt_p == NULL)
    {
        log_msg(LOG_DEBUG, "Failed to retrieve UEContext from control block %u", cbIndex);
    }
    else
    {
        MmContext* mmContext_p = ueCtxt_p->getMmContext();
        if (mmContext_p != NULL)
        {
            SubsDataGroupManager::Instance()->deleteMmContext(mmContext_p);
            ueCtxt_p->setMmContext(NULL);
        }

        // Remove IMSI -> CBIndex key mapping
        const DigitRegister15& ue_imsi = ueCtxt_p->getImsi();
        SubsDataGroupManager::Instance()->deleteimsikey(ue_imsi);

        // Remove mTMSI -> CBIndex mapping
        SubsDataGroupManager::Instance()->deletemTmsikey(ueCtxt_p->getMTmsi());

        SubsDataGroupManager::Instance()->deleteUEContext(ueCtxt_p);
    }

    SubsDataGroupManager::Instance()->deAllocateCB(cb_p->getCBIndex());
}

S1HandoverProcedureContext* MmeContextManagerUtils::allocateHoContext(SM::ControlBlock& cb_r)
{
    log_msg(LOG_DEBUG, "allocateHoProcedureCtxt: Entry");

    S1HandoverProcedureContext *prcdCtxt_p =
            SubsDataGroupManager::Instance()->getS1HandoverProcedureContext();
    if (prcdCtxt_p != NULL)
    {
        prcdCtxt_p->setCtxtType(ProcedureType::s1Handover_c);
        prcdCtxt_p->setNextState(IntraS1HoStart::Instance());
        prcdCtxt_p->setHoType(intraMmeS1Ho_c);
        cb_r.setCurrentTempDataBlock(prcdCtxt_p);
    }

    return prcdCtxt_p;
}

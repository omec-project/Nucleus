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

#include <iostream>
#include <string>
#include "controlBlock.h"
#include "event.h"
#include "eventMessage.h"
#include "log.h"
#include "tempDataBlock.h"
#include <stateMachineEngine.h>

using namespace std;
using namespace cmn;

namespace SM
{
	uint32_t ControlBlock::controlBlockArrayIdx = 0;

	ControlBlock::ControlBlock():mutex_m(), cbIndex_m(++controlBlockArrayIdx),
	        cbState_m(FREE), smCtxt_m(), pdb_mp(NULL),
	        tdb_mp(NULL), inProcQueue_m(false)
	{
        for (int i = 0; i < MAX_FAST_BLOCK_IDX; i++)
            fadb_mpa[i] = NULL;

        std::queue<Event> internalEmptyQ;
        std::swap( internalEventQ, internalEmptyQ );

        std::queue<Event> extEmptymptyQ;
        std::swap( externalEventQ, extEmptymptyQ );

        std::deque<debugEventInfo> emptyDQ;
        std::swap(debugEventInfoQ, emptyDQ);
	}

	void ControlBlock::reset()
	{
		std::lock_guard<std::mutex> lock(mutex_m);

		cbState_m = FREE;

		smCtxt_m.clear();

		pdb_mp = NULL;

		for (int i = 0; i < MAX_FAST_BLOCK_IDX; i++)
			fadb_mpa[i] = NULL;

		tdb_mp = NULL;

		inProcQueue_m = false;

		std::queue<Event> internalEmptyQ;
		std::swap( internalEventQ, internalEmptyQ );

        std::queue<Event> extEmptymptyQ;
        std::swap( externalEventQ, extEmptymptyQ );

		std::deque<debugEventInfo> emptyDQ;
		std::swap(debugEventInfoQ, emptyDQ);
	}

	ControlBlock::~ControlBlock()
	{
	}

	bool ControlBlock::getNextEvent(Event &evt)
	{
		std::lock_guard<std::mutex> lock(mutex_m);

		bool rc = true;
        if(!internalEventQ.empty())
        {
            evt = internalEventQ.front();
            internalEventQ.pop();
        }
        else if(!externalEventQ.empty())
        {
            evt = externalEventQ.front();
            externalEventQ.pop();
        }
        else
        {
            rc = false;
        }
		return rc;
	}
      
	void ControlBlock::display()
	{
		log_msg(LOG_DEBUG,"Displaying control block");

		pdb_mp->display();

		tdb_mp->display();

	}

	void ControlBlock::addEventToProcQ(Event &event)
	{
		std::lock_guard<std::mutex> lock(mutex_m);

		if (cbState_m == ALLOCATED)
		{
		    externalEventQ.push(event);
		    if (inProcQueue_m == false)
		    {
		        inProcQueue_m = true;
		        if (!SM::StateMachineEngine::Instance()->addCBToProcQ(this))
		        {
		            inProcQueue_m = false;
		        }
		    }
		}
	}

    void ControlBlock::qInternalEvent(Event &event)
    {
        if (cbState_m == ALLOCATED)
        {
            internalEventQ.push(event);
        }
    }

	PermDataBlock* ControlBlock::getFastAccessBlock(unsigned short idx) const
	{
		PermDataBlock* permBlock_p = NULL;

		if (idx < ControlBlock::MAX_FAST_BLOCK_IDX)
			permBlock_p = fadb_mpa[idx];

		return permBlock_p;
	}

	void ControlBlock::setFastAccessBlock(
		PermDataBlock* pdb_p, unsigned short idx)
	{
		if (idx < ControlBlock::MAX_FAST_BLOCK_IDX)
			fadb_mpa[idx] = pdb_p;
	}

	PermDataBlock* ControlBlock::getPermDataBlock() const
	{
		return pdb_mp;
	}

	void ControlBlock::setPermDataBlock(PermDataBlock* pdb_p)
	{
		pdb_mp = pdb_p;
	}

    void ControlBlock::addTempDataBlock(TempDataBlock* tdb_p)
    {
        if (tdb_p != NULL)
        {
        	tdb_p->setNextTempDataBlock(tdb_mp);
        }

    	tdb_mp = tdb_p;
    }

    TempDataBlock* ControlBlock::getFirstTempDataBlock()
    {
        return tdb_mp;
    }

    void ControlBlock::removeTempDataBlock(TempDataBlock* tdb_p)
    {
        TempDataBlock* currTdb_p = tdb_mp;

        TempDataBlock* nextTdb_p = NULL;
        TempDataBlock* prevTdb_p = NULL;

        while (currTdb_p != NULL)
        {
            nextTdb_p = currTdb_p->getNextTempDataBlock();
            if (currTdb_p == tdb_p)
            {
                if (prevTdb_p != NULL)
                {
                    prevTdb_p->setNextTempDataBlock(nextTdb_p);
                }
                else
                {
                    tdb_mp = nextTdb_p;
                }
                break;
            }

            prevTdb_p = currTdb_p;
            currTdb_p = nextTdb_p;
        }

        if (smCtxt_m.tempDataBlock_mp == tdb_p)
        {
            smCtxt_m.tempDataBlock_mp = NULL;
        }
    }
	
	void ControlBlock::addDebugInfo(debugEventInfo& eventInfo)
	{
		if(debugEventInfoQ.size()>=10)
		{
			debugEventInfoQ.pop_front();
		}
		debugEventInfoQ.push_back(eventInfo);
	}

	uint32_t ControlBlock::getCBIndex()
	{
		return cbIndex_m;
	}

    void ControlBlock::setControlBlockState(ControlBlockState state)
    {
        std::lock_guard<std::mutex> lock(mutex_m);
        
        log_msg(LOG_DEBUG, "CB state transition from %d to %d", cbState_m, state);

        cbState_m = state;
    }

    ControlBlockState ControlBlock::getControlBlockState()
    {
        std::lock_guard<std::mutex> lock(mutex_m);

        return cbState_m;
    }

    void ControlBlock::setProcQueueFlag(bool flag)
    {
        std::lock_guard<std::mutex> lock(mutex_m);
        inProcQueue_m = flag;
    }

    void ControlBlock::resetProcQueueFlag()
    {
        std::lock_guard<std::mutex> lock(mutex_m);
        inProcQueue_m = false;
    }

    bool ControlBlock::isInProcQueue()
    {
        std::lock_guard<std::mutex> lock(mutex_m);
        return inProcQueue_m;
    }

    void* ControlBlock::getMsgData()
    {
        IpcEMsgShPtr eMsg = std::dynamic_pointer_cast<cmn::IpcEventMessage>(
                smCtxt_m.evt.getEventData());
        void * msg = NULL;
        if (eMsg)
        {
            msg =  eMsg->getMsgBuffer();
        }
        return msg;
    }

    uint16_t ControlBlock::getEventId()
    {
        return smCtxt_m.evt.getEventId();
    }

    cmn::EventMsgShPtr ControlBlock::getEventMessage()
    {
        return smCtxt_m.evt.getEventData();
    }

    TempDataBlock* ControlBlock::getTempDataBlock() const
    {
        return smCtxt_m.tempDataBlock_mp;
    }

    void ControlBlock::setNextState(State* state)
    {
        if (smCtxt_m.tempDataBlock_mp != NULL)
        {
            smCtxt_m.tempDataBlock_mp->setNextState(state);
        }
    }

    StateMachineContext& ControlBlock::getStateMachineContext()
    {
        return smCtxt_m;
    }

}


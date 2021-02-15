/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CONTROLBLOCK_H_
#define CONTROLBLOCK_H_

#include <mutex>
#include <queue>
#include <deque>
#include <stdint.h>
#include <time.h>
#include "permDataBlock.h"
#include <smTypes.h>
#include "tempDataBlock.h"
#include <event.h>

class Event;
class State;

using namespace std;

namespace SM
{
	typedef struct debugEventInfo
	{
		uint16_t event;
		std::string state;
        	time_t evt_time;

		debugEventInfo(uint16_t evt, std::string st, time_t t)
        	{
        		event = evt;
        		state = st;
        		evt_time = t;
        	}

	}debugEventInfo;

	class Event;
	class State;
	class ControlBlock
	{
   	public:
		static const unsigned short MAX_FAST_BLOCK_IDX = 5;
		static uint32_t controlBlockArrayIdx;

      		ControlBlock();
      		virtual ~ControlBlock();

      		// Invoked when the control block
      		// is freed. Resets the individual data members of
      		// control block
      		void reset();

      		uint32_t getCBIndex();

      		// Invoked when external events are to be queued.
      		// The event gets queued to the external event q and
      		// the control block is placed on the State Machine
      		// procedure queue
      		void addEventToProcQ(Event &event);

      		// Queue internal events to the internal event queue.
      		// SM process the internal events first and then
      		// checks for the external event queue
      		void qInternalEvent(Event &event);

      		// Fetches the next event to be processed
      		// from the control block's event queue.
	      	bool getNextEvent(Event &evt);

	      	// Returns the event that is being
	      	// processed by the state machine
	      	uint16_t getEventId();

	      	// Returns the event message that is being
	      	// processed by the state machine
	      	cmn::EventMsgShPtr getEventMessage();

	      	// Returns the ipc event message for the
	      	// current event being processed
	      	void* getMsgData();

	      	// Returns the tempDataBlock selected by
	      	// the event validator
	      	TempDataBlock* getTempDataBlock() const;

	      	// Sets the next state for the temp
	      	// data block being processed by
	      	// the state machine
      		void setNextState(State* state);

      		StateMachineContext& getStateMachineContext();

      		PermDataBlock* getFastAccessBlock(unsigned short idx) const;
      		void setFastAccessBlock(PermDataBlock* pdb_p, unsigned short idx);

      		PermDataBlock* getPermDataBlock() const;
      		void setPermDataBlock(PermDataBlock* pdb_p);

      		void addTempDataBlock(TempDataBlock* tdb_p);
      		void removeTempDataBlock(TempDataBlock* tdb_p);
      		TempDataBlock* getFirstTempDataBlock();

      		void addDebugInfo(debugEventInfo& eventInfo);
      		inline deque<debugEventInfo> getDebugInfoQueue()const
      		{
      		    return debugEventInfoQ;
      		}

      		void setControlBlockState(ControlBlockState state);
      		ControlBlockState getControlBlockState();

      		bool isInProcQueue();
      		void setProcQueueFlag(bool flag);
      		void resetProcQueueFlag();
		
      		virtual void display();

   	private:
      		std::mutex mutex_m;
      		const uint32_t cbIndex_m;
      		ControlBlockState cbState_m;

      		StateMachineContext smCtxt_m;

      		PermDataBlock* pdb_mp;
      		PermDataBlock* fadb_mpa[MAX_FAST_BLOCK_IDX];
      		TempDataBlock* tdb_mp;

      		// Q for storing events generated by entities
      		// external to state machine
      		queue<Event> externalEventQ;

      		// Q for storing internal events,
      		// for eg. events generated by action
      		// handlers while processing an external event.
      		queue<Event> internalEventQ;
		
	    	deque<debugEventInfo> debugEventInfoQ;
	    	bool inProcQueue_m;
	};
}
#endif /* CONTROLBLOCK_H_ */


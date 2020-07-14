/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string>
#include <deque>
#include "smTypes.h"

#ifndef ACTIONTABLE_H_
#define ACTIONTABLE_H_

namespace SM
{

	class ControlBlock;
	class State;

	using namespace std;

	class ActionTable
	{
   	public:
		ActionTable();
		virtual ~ActionTable();

		virtual void display();

		ActStatus executeActions(ControlBlock& cb, State* statep);
		void addAction(ActionPointer act);
		void setNextState(State*st);
	private:
     		deque<ActionPointer> actionsQ;
        	State* nextStatep;
	};
}
#endif /* ACTIONTABLE_H_ */

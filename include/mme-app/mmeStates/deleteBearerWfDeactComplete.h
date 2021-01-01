/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 /******************************************************
 * deleteBearerWfDeactComplete.h
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/state.h.tt>
 ******************************************************/
 
#ifndef __DeleteBearerWfDeactComplete__
#define __DeleteBearerWfDeactComplete__

#include "state.h"

namespace mme {

	class DeleteBearerWfDeactComplete : public SM::State
	{
		public:
			/******************************************
			* Instance 
			*    Creates static instance for the state
			*******************************************/
			static DeleteBearerWfDeactComplete* Instance();

			/****************************************
			* DeleteBearerWfDeactComplete
			*    Destructor
			****************************************/
			~DeleteBearerWfDeactComplete();			
			
			/******************************************
			* initialize
			*  Initializes action handlers for the state
			* and next state
			******************************************/
			void initialize();
	
		private:
			/****************************************
			* DeleteBearerWfDeactComplete
			*    Private constructor
			****************************************/
			DeleteBearerWfDeactComplete();  
	};
};
#endif // __DeleteBearerWfDeactComplete__

 /*
 * Copyright 2021-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BearerContextManager__
#define __BearerContextManager__
/******************************************************
* bearerContextManager.h
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/ctxtManagerTmpls/blockPoolManager.h.tt>
 ***************************************/
#include "memPoolManager.h"

namespace mme
{
	class BearerContext;
	class BearerContextManager
	{
		public:
			/****************************************
			* BearerContextManager
			*  constructor
			****************************************/
			BearerContextManager(int numOfBlocks);
			
			/****************************************
			* BearerContextManager
			*    Destructor
			****************************************/
			~BearerContextManager();
			
			/******************************************
			 * allocateBearerContext
			 * allocate BearerContext data block
			 ******************************************/
			BearerContext* allocateBearerContext();
			
			/******************************************
			 * deallocateBearerContext
			 * deallocate a BearerContext data block
			 ******************************************/
			void deallocateBearerContext(BearerContext* BearerContextp );
	
		private:
			cmn::memPool::MemPoolManager<BearerContext> poolManager_m;
	};
};

#endif
		
		

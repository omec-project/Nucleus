 /*
 * Copyright 2019-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __MmeSmCreateBearerProcCtxtManager__
#define __MmeSmCreateBearerProcCtxtManager__
/******************************************************
* mmeSmCreateBearerProcCtxtManager.h
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/ctxtManagerTmpls/blockPoolManager.h.tt>
 ***************************************/
#include "memPoolManager.h"

namespace mme
{
	class MmeSmCreateBearerProcCtxt;
	class MmeSmCreateBearerProcCtxtManager
	{
		public:
			/****************************************
			* MmeSmCreateBearerProcCtxtManager
			*  constructor
			****************************************/
			MmeSmCreateBearerProcCtxtManager(int numOfBlocks);
			
			/****************************************
			* MmeSmCreateBearerProcCtxtManager
			*    Destructor
			****************************************/
			~MmeSmCreateBearerProcCtxtManager();
			
			/******************************************
			 * allocateMmeSmCreateBearerProcCtxt
			 * allocate MmeSmCreateBearerProcCtxt data block
			 ******************************************/
			MmeSmCreateBearerProcCtxt* allocateMmeSmCreateBearerProcCtxt();
			
			/******************************************
			 * deallocateMmeSmCreateBearerProcCtxt
			 * deallocate a MmeSmCreateBearerProcCtxt data block
			 ******************************************/
			void deallocateMmeSmCreateBearerProcCtxt(MmeSmCreateBearerProcCtxt* MmeSmCreateBearerProcCtxtp );
	
		private:
			cmn::memPool::MemPoolManager<MmeSmCreateBearerProcCtxt> poolManager_m;
	};
};

#endif
		
		

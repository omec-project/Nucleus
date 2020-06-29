 /*
 * Copyright 2019-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __MmeProcedureCtxtManager__
#define __MmeProcedureCtxtManager__
/******************************************************
* mmeProcedureCtxtManager.h
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/ctxtManagerTmpls/blockPoolManager.h.tt>
 ***************************************/
#include "memPoolManager.h"

namespace mme
{
	class MmeProcedureCtxt;
	class MmeProcedureCtxtManager
	{
		public:
			/****************************************
			* MmeProcedureCtxtManager
			*  constructor
			****************************************/
			MmeProcedureCtxtManager(int numOfBlocks);
			
			/****************************************
			* MmeProcedureCtxtManager
			*    Destructor
			****************************************/
			~MmeProcedureCtxtManager();
			
			/******************************************
			 * allocateMmeProcedureCtxt
			 * allocate MmeProcedureCtxt data block
			 ******************************************/
			MmeProcedureCtxt* allocateMmeProcedureCtxt();
			
			/******************************************
			 * deallocateMmeProcedureCtxt
			 * deallocate a MmeProcedureCtxt data block
			 ******************************************/
			void deallocateMmeProcedureCtxt(MmeProcedureCtxt* MmeProcedureCtxtp );
	
		private:
			cmn::memPool::MemPoolManager<MmeProcedureCtxt> poolManager_m;
	};
};

#endif
		
		
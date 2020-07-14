 /*
 * Copyright 2019-present, Infosys Limited.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __MmeErabModIndProcedureCtxtManager__
#define __MmeErabModIndProcedureCtxtManager__
/******************************************************
* mmeErabModIndProcedureCtxtManager.h
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/ctxtManagerTmpls/blockPoolManager.h.tt>
 ***************************************/
#include "memPoolManager.h"

namespace mme
{
	class MmeErabModIndProcedureCtxt;
	class MmeErabModIndProcedureCtxtManager
	{
		public:
			/****************************************
			* MmeErabModIndProcedureCtxtManager
			*  constructor
			****************************************/
			MmeErabModIndProcedureCtxtManager(int numOfBlocks);
			
			/****************************************
			* MmeErabModIndProcedureCtxtManager
			*    Destructor
			****************************************/
			~MmeErabModIndProcedureCtxtManager();
			
			/******************************************
			 * allocateMmeErabModIndProcedureCtxt
			 * allocate MmeErabModIndProcedureCtxt data block
			 ******************************************/
			MmeErabModIndProcedureCtxt* allocateMmeErabModIndProcedureCtxt();
			
			/******************************************
			 * deallocateMmeErabModIndProcedureCtxt
			 * deallocate a MmeErabModIndProcedureCtxt data block
			 ******************************************/
			void deallocateMmeErabModIndProcedureCtxt(MmeErabModIndProcedureCtxt* MmeErabModIndProcedureCtxtp );
	
		private:
			cmn::memPool::MemPoolManager<MmeErabModIndProcedureCtxt> poolManager_m;
	};
};

#endif
		
		

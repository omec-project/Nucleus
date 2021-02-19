 /*
 * Copyright 2021-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */ 
/******************************************************************************
 * mmeAttachProcedureCtxtManager.cpp
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/ctxtManagerTmpls/blockPoolManager.cpp.tt>
 ******************************************************************************/

#include "memPoolManager.h"
#include "contextManager/dataBlocks.h"
#include "contextManager/mmeAttachProcedureCtxtManager.h"

using namespace cmn::memPool;

namespace mme
{
	/******************************************************************************
	* Constructor
	******************************************************************************/
	MmeAttachProcedureCtxtManager::MmeAttachProcedureCtxtManager(int numOfBlocks):poolManager_m(numOfBlocks)
	{
	}
	
	/******************************************************************************
	* Destructor
	******************************************************************************/
	MmeAttachProcedureCtxtManager::~MmeAttachProcedureCtxtManager()
	{
	}
	
	/******************************************************************************
	* Allocate MmeAttachProcedureCtxt data block
	******************************************************************************/
	MmeAttachProcedureCtxt* MmeAttachProcedureCtxtManager::allocateMmeAttachProcedureCtxt()
	{
		MmeAttachProcedureCtxt* MmeAttachProcedureCtxt_p = poolManager_m.allocate();
		return MmeAttachProcedureCtxt_p;
	}
	
	/******************************************************************************
	* Deallocate a MmeAttachProcedureCtxt data block
	******************************************************************************/
	void MmeAttachProcedureCtxtManager::deallocateMmeAttachProcedureCtxt(MmeAttachProcedureCtxt* MmeAttachProcedureCtxtp )
	{
		poolManager_m.free( MmeAttachProcedureCtxtp );
	}
}
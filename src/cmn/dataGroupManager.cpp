/*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dataGroupManager.h"
#include "memPoolManager.h"

namespace cmn {
namespace DGM
{
	DataGroupManager::DataGroupManager():cbstore_m(NULL)
	{

	}
	
	DataGroupManager::~DataGroupManager()
	{
		delete[] cbstore_m;
	}
	
	void DataGroupManager::initializeCBStore( int DataCount )
	{
		cbstore_m = new ControlBlock[DataCount];
		for( int idx = 0; idx < DataCount; idx ++ )
		{
			freeQ_m.push_back( &cbstore_m[idx] );
		}
	}
	
	ControlBlock* DataGroupManager::allocateCB()
	{
		std::lock_guard<std::mutex> lock(mutex_m);

		ControlBlock* cbp = freeQ_m.front();
		freeQ_m.pop_front();

		if (cbp != NULL)
		{
		    cbp->setControlBlockState(ALLOCATED);
		}

		return cbp;
	}

	ControlBlock* DataGroupManager::findControlBlock(uint32_t cbIndex)
	{
		if (cbIndex > ControlBlock::controlBlockArrayIdx ||
                    cbIndex == 0)
                {
                    return NULL;
                }
        
                ControlBlock* cbp = &cbstore_m[cbIndex - 1];
		if (cbp != NULL && cbp->getControlBlockState() != FREE)
		    return cbp;
		else
		    return NULL;
	}
	
	void DataGroupManager::deAllocateCB( uint32_t cbIndex )
	{
                if (cbIndex > ControlBlock::controlBlockArrayIdx ||
                    cbIndex == 0)
                {
                    return;
                }
        
                cbstore_m[cbIndex - 1].reset();

		std::lock_guard<std::mutex> lock(mutex_m);
		freeQ_m.push_back( &cbstore_m[cbIndex - 1]);
	}
};
};

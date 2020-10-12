/*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
/**************************************
 *
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/ctxtManagerTmpls/subsDataGroupManager.cpp.tt>
 ***************************************/
#include "s1apContextManager/s1apDataGroupManager.h"

namespace mme
{
	/******************************************************************************
	* Constructor
	******************************************************************************/
	S1apDataGroupManager::S1apDataGroupManager()
	{
			EnbContextManagerm_p = NULL;

			initialize();
	}
	
	/******************************************************************************
	* Destructor
	******************************************************************************/
	S1apDataGroupManager::~S1apDataGroupManager()
	{
			delete EnbContextManagerm_p;
	}
	
	/******************************************
	*  Initializes control block and pool managers
	******************************************/
	void S1apDataGroupManager::initialize()
	{
		initializeCBStore(100);

		EnbContextManagerm_p = new EnbContextManager(500);
	}
	
	/******************************************************************************
	* creates and returns static instance
	******************************************************************************/
	S1apDataGroupManager* S1apDataGroupManager::Instance()
	{
			static S1apDataGroupManager s1apDataGroupMgr;
			return &s1apDataGroupMgr;
	}

	EnbContext* S1apDataGroupManager::getEnbContext()
	{
		return EnbContextManagerm_p->allocateEnbContext();
	}

	void S1apDataGroupManager::deleteEnbContext(EnbContext* EnbContextp )
	{
		EnbContextManagerm_p->deallocateEnbContext( EnbContextp );
	}
	
	/******************************************
	* Add a enbFd as key and cb index as value to enbFd_cb_id_map
	******************************************/
	int S1apDataGroupManager::addenbFdkey( int key, int cb_index )
	{
		std::lock_guard<std::mutex> lock(enbFd_cb_id_map_mutex);

		int rc = 1;

		auto itr = enbFd_cb_id_map.insert({ key, cb_index });
		if (itr.second == false)
		{
			rc = -1;
		}
		return rc;
	}
	
	/******************************************
	* Delete a enbFd key from enbFd_cb_id_map
	******************************************/
	int S1apDataGroupManager::deleteenbFdkey( int key )
	{
		std::lock_guard<std::mutex> lock(enbFd_cb_id_map_mutex);
 
		return enbFd_cb_id_map.erase( key );
	}
	
	/******************************************
	* get size of  enbFd_cb_id_map
	******************************************/
	int S1apDataGroupManager::sizeEnbFdKeyMap()
	{
		std::lock_guard<std::mutex> lock(enbFd_cb_id_map_mutex);
 
		return enbFd_cb_id_map.size();
	}	
	
	/******************************************
	* Find cb with given enbFd from enbFd_cb_id_map
	* returns -1 if not found, else cb index
	******************************************/ 
	int S1apDataGroupManager::findCBWithenbFd( int key )
	{
		std::lock_guard<std::mutex> lock(enbFd_cb_id_map_mutex);
        
		auto itr = enbFd_cb_id_map.find( key );
		if( itr != enbFd_cb_id_map.end())
		{
			return itr->second;
		}
		return -1;
	}
	/******************************************
	* Add a enbId as key and cb index as value to enbId_cb_id_map
	******************************************/
	int S1apDataGroupManager::addenbIdkey( int key, int cb_index )
	{
		std::lock_guard<std::mutex> lock(enbId_cb_id_map_mutex);

		int rc = 1;

		auto itr = enbId_cb_id_map.insert({ key, cb_index });
		if (itr.second == false)
		{
			rc = -1;
		}
		return rc;
	}
	
	/******************************************
	* Delete a enbId key from enbId_cb_id_map
	******************************************/
	int S1apDataGroupManager::deleteenbIdkey( int key )
	{
		std::lock_guard<std::mutex> lock(enbId_cb_id_map_mutex);
 
		return enbId_cb_id_map.erase( key );
	}
	
	/******************************************
	* get size of  enbId_cb_id_map
	******************************************/
	int S1apDataGroupManager::sizeEnbIdKeyMap()
	{
		std::lock_guard<std::mutex> lock(enbId_cb_id_map_mutex);
 
		return enbId_cb_id_map.size();
	}	
	
	/******************************************
	* Find cb with given enbId from enbId_cb_id_map
	* returns -1 if not found, else cb index
	******************************************/ 
	int S1apDataGroupManager::findCBWithenbId( int key )
	{
		std::lock_guard<std::mutex> lock(enbId_cb_id_map_mutex);
        
		auto itr = enbId_cb_id_map.find( key );
		if( itr != enbId_cb_id_map.end())
		{
			return itr->second;
		}
		return -1;
	}
}

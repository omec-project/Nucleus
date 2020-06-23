/*
 * Copyright 2019-present, Infosys Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __S1AP_DATAGROUPMANAGER__
#define __S1AP_DATAGROUPMANAGER__
/**************************************
 *
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/ctxtManagerTmpls/subsDataGroupManager.h.tt>
 ***************************************/
#include <map>
#include <mutex>
#include "dataGroupManager.h"
#include "s1apContextManager/s1apDataBlocks.h"
#include "s1apContextManager/enbContextManager.h"
namespace mme
{	
	class S1apDataGroupManager:public cmn::DGM::DataGroupManager
	{
		public:
		
			/******************************************
			* Instance 
			*    Creates static instance for the S1apDataGroupManager
			*******************************************/
			static S1apDataGroupManager* Instance();
	
			/****************************************
			* S1apDataGroupManager
			*    Destructor
			****************************************/
			virtual ~S1apDataGroupManager();
			
			/******************************************
			* initialize
			* Initializes control block and pool managers
			******************************************/
			void initialize();

			/******************************************
			 * getEnbContext
			 * Get EnbContext data block
			 ******************************************/
			EnbContext* getEnbContext();
			
			/******************************************
			 * deleteEnbContext
			 *  Delete a EnbContext data block
			 ******************************************/
			void deleteEnbContext(EnbContext* EnbContextp );
			
			/******************************************
			* addenbFdkey
			* Add a enbFd as key and cb index as value to enbFd_cb_id_map
			******************************************/
			int addenbFdkey( int key, int cb_index );
			
			/******************************************
			* deleteenbFdkey
			* delete a enbFd key from enbFd_cb_id_map
			******************************************/		
			int deleteenbFdkey( int key );
			
			/******************************************
			* findCBWithenbFd
			* Find cb with given enbFd from enbFd_cb_id_map
			******************************************/	
			int findCBWithenbFd( int key );
			/******************************************
			* addenbIdkey
			* Add a enbId as key and cb index as value to enbId_cb_id_map
			******************************************/
			int addenbIdkey( int key, int cb_index );
			
			/******************************************
			* deleteenbIdkey
			* delete a enbId key from enbId_cb_id_map
			******************************************/		
			int deleteenbIdkey( int key );
			
			/******************************************
			* findCBWithenbId
			* Find cb with given enbId from enbId_cb_id_map
			******************************************/	
			int findCBWithenbId( int key );
			
			
		private:
			
			/****************************************
			* S1apDataGroupManager
			*    Private constructor
			****************************************/
			S1apDataGroupManager();
			
			/****************************************
			* EnbContext Pool Manager
			****************************************/
			EnbContextManager* EnbContextManagerm_p;
			
			
			/****************************************
			* enbFd Key Map
			****************************************/
			std::map<int,int> enbFd_cb_id_map;
			
			/****************************************
			* enbFd Key Map
			****************************************/
			std::mutex enbFd_cb_id_map_mutex;
			/****************************************
			* enbId Key Map
			****************************************/
			std::map<int,int> enbId_cb_id_map;
			
			/****************************************
			* enbId Key Map
			****************************************/
			std::mutex enbId_cb_id_map_mutex;
	};
};

#endif
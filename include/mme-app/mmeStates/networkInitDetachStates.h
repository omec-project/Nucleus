

 /*
 * Copyright 2021-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 
 /******************************************************
 * networkInitDetachStates.h
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/SMCodeGen/templates/stateMachineTmpls/state.h.tt>
 ******************************************************/
 #ifndef __NETWORK_INIT_DETACH__
 #define __NETWORK_INIT_DETACH__

 #include "state.h"

 namespace mme {
	class NiDetachState : public SM::State
	{
		public:
			/******************************************
			* Instance 
			*    Creates static instance for the state
			*******************************************/
			static NiDetachState* Instance();

			/****************************************
			* NiDetachState
			*    Destructor
			****************************************/
			~NiDetachState();			
			
			/******************************************
			* initialize
			*  Initializes action handlers for the state
			* and next state
			******************************************/
			void initialize();

			/*****************************************
			* returns stateId
			*****************************************/
			uint16_t getStateId() const;

			/*****************************************
			* returns stateName
			*****************************************/
			const char* getStateName() const;

		protected:
			/****************************************
			* NiDetachState
			*    Protected constructor
			****************************************/
			NiDetachState();  
	};
	
	class NiDetachStart : public NiDetachState
	{
		public:
			/******************************************
			* Instance 
			*    Creates static instance for the state
			*******************************************/
			static NiDetachStart* Instance();

			/****************************************
			* NiDetachStart
			*    Destructor
			****************************************/
			~NiDetachStart();			
			
			/******************************************
			* initialize
			*  Initializes action handlers for the state
			* and next state
			******************************************/
			void initialize();

			/*****************************************
			* returns stateId
			*****************************************/
			uint16_t getStateId() const;

			/*****************************************
			* returns stateName
			*****************************************/
			const char* getStateName() const;

		protected:
			/****************************************
			* NiDetachStart
			*    Protected constructor
			****************************************/
			NiDetachStart();  
	};
	
	class NiDetachWfDetAccptDelSessResp : public NiDetachState
	{
		public:
			/******************************************
			* Instance 
			*    Creates static instance for the state
			*******************************************/
			static NiDetachWfDetAccptDelSessResp* Instance();

			/****************************************
			* NiDetachWfDetAccptDelSessResp
			*    Destructor
			****************************************/
			~NiDetachWfDetAccptDelSessResp();			
			
			/******************************************
			* initialize
			*  Initializes action handlers for the state
			* and next state
			******************************************/
			void initialize();

			/*****************************************
			* returns stateId
			*****************************************/
			uint16_t getStateId() const;

			/*****************************************
			* returns stateName
			*****************************************/
			const char* getStateName() const;

		protected:
			/****************************************
			* NiDetachWfDetAccptDelSessResp
			*    Protected constructor
			****************************************/
			NiDetachWfDetAccptDelSessResp();  
	};
	
	class NiDetachWfDelSessResp : public NiDetachState
	{
		public:
			/******************************************
			* Instance 
			*    Creates static instance for the state
			*******************************************/
			static NiDetachWfDelSessResp* Instance();

			/****************************************
			* NiDetachWfDelSessResp
			*    Destructor
			****************************************/
			~NiDetachWfDelSessResp();			
			
			/******************************************
			* initialize
			*  Initializes action handlers for the state
			* and next state
			******************************************/
			void initialize();

			/*****************************************
			* returns stateId
			*****************************************/
			uint16_t getStateId() const;

			/*****************************************
			* returns stateName
			*****************************************/
			const char* getStateName() const;

		protected:
			/****************************************
			* NiDetachWfDelSessResp
			*    Protected constructor
			****************************************/
			NiDetachWfDelSessResp();  
	};
	
	class NiDetachWfDetachAccept : public NiDetachState
	{
		public:
			/******************************************
			* Instance 
			*    Creates static instance for the state
			*******************************************/
			static NiDetachWfDetachAccept* Instance();

			/****************************************
			* NiDetachWfDetachAccept
			*    Destructor
			****************************************/
			~NiDetachWfDetachAccept();			
			
			/******************************************
			* initialize
			*  Initializes action handlers for the state
			* and next state
			******************************************/
			void initialize();

			/*****************************************
			* returns stateId
			*****************************************/
			uint16_t getStateId() const;

			/*****************************************
			* returns stateName
			*****************************************/
			const char* getStateName() const;

		protected:
			/****************************************
			* NiDetachWfDetachAccept
			*    Protected constructor
			****************************************/
			NiDetachWfDetachAccept();  
	};
	
	class NiDetachWfS1RelComp : public NiDetachState
	{
		public:
			/******************************************
			* Instance 
			*    Creates static instance for the state
			*******************************************/
			static NiDetachWfS1RelComp* Instance();

			/****************************************
			* NiDetachWfS1RelComp
			*    Destructor
			****************************************/
			~NiDetachWfS1RelComp();			
			
			/******************************************
			* initialize
			*  Initializes action handlers for the state
			* and next state
			******************************************/
			void initialize();

			/*****************************************
			* returns stateId
			*****************************************/
			uint16_t getStateId() const;

			/*****************************************
			* returns stateName
			*****************************************/
			const char* getStateName() const;

		protected:
			/****************************************
			* NiDetachWfS1RelComp
			*    Protected constructor
			****************************************/
			NiDetachWfS1RelComp();  
	};
	
};
#endif // __NETWORK_INIT_DETACH__

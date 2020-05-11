/*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <time.h>
#include <grpcpp/grpcpp.h>

#include "mmeGrpc.grpc.pb.h"

#include <mmeSmDefs.h>
#include <smTypes.h>
#include <controlBlock.h>
#include <contextManager/dataBlocks.h>
#include <contextManager/subsDataGroupManager.h>
#include <procedureStats.h>
#include <log.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;                                                     
using grpc::ServerReaderWriter;                                                
using grpc::ServerWriter; 
using grpc::Status;
using mmeGrpc::UeContextReqBuf;
using mmeGrpc::UeContextRespBuf;
using mmeGrpc::UeContextRespBuf_SessionContextRespBuf;
using mmeGrpc::MmeGrpcCli;
using mmeGrpc::Empty;
using mmeGrpc::ProcedureStatsRespBuf;
using mmeGrpc::EventInfoRespBuf;
using mmeGrpc::EventInfoRespBuf_EventInfoBuf;

using namespace mme;

const string UEStates[5]={ "InvalidState", "NoState", "EpsAttached", "Detached" };
// Logic and data behind the server's behavior.
class MmeGrpcCliServiceImpl final : public MmeGrpcCli::Service {
  Status GetUeContextInfo(SM::ControlBlock* controlBlk_p, UeContextRespBuf* reply)   {
	//time_t my_time = time(NULL);
//	SM::ControlBlock* controlBlk_p = mme::SubsDataGroupManager::Instance()->findControlBlock((uint32_t)request->id());
	if (controlBlk_p)
	{
		UEContext* uecontext_p=dynamic_cast<UEContext*>(controlBlk_p->getPermDataBlock());
		if (uecontext_p != NULL)
		{
 			//cout << "MY TIME " << ctime(&my_time);
			
			// Display IMSI
			stringstream ss;
			ss << uecontext_p->getImsi();
			reply->set_imsi(ss.str());

			// MSISDN
			ss.str("");
			ss << uecontext_p->getMsisdn();
			string ms(ss.str());
            ms.resize(10);
            ms.shrink_to_fit();
            reply->set_msisdn(ms);
			
			// TAI
			ss.str("");
			const TAI& tai = uecontext_p->getTai().tai_m;
			ss << "MCC : ";
			ss << (unsigned char)((tai.plmn_id.idx[0] & 0x0f) + 0x30);
			ss << (unsigned char)((tai.plmn_id.idx[0] >> 4) + 0x30);
			ss << (unsigned char)((tai.plmn_id.idx[1] & 0x0f) + 0x30);
			ss << " MNC : ";
			ss << (unsigned char)((tai.plmn_id.idx[2] & 0x0f) + 0x30);
			ss << (unsigned char)((tai.plmn_id.idx[2] >> 4) + 0x30);
			ss << " TAC : " << tai.tac ;
			reply->set_tai(ss.str());

			// Cell Identity
			ss.str("");
			const CGI& cgi = uecontext_p->getUtranCgi().cgi_m;
			ss << "0x" << std::hex << cgi.cell_id;
			reply->set_eutran_cgi(ss.str());

			reply->set_context_id(uecontext_p->getContextID());

			reply->set_enb_ue_s1ap_id(uecontext_p->getS1apEnbUeId());
			
			MmContext* mmCtxt = uecontext_p->getMmContext();
			UE_State_e uestate = mmCtxt->getMmState();

			reply->set_ue_state(UEStates[uestate]);

			SessionContext* sessioncontext_p = uecontext_p->getSessionContext();
			if (sessioncontext_p != NULL)
			{
				UeContextRespBuf_SessionContextRespBuf* session_ctxt = reply->add_sessioncontext();
				if (session_ctxt)
				{
					// APN
					const apn_name& apn = sessioncontext_p->getAccessPtName().apnname_m;
					string apnStr((const char*)(apn.val), apn.len);
					session_ctxt->set_apn(apnStr);

					// PDN address
					const PAA& PdnAddr = sessioncontext_p->getPdnAddr().paa_m;
				        char ipStr[INET_ADDRSTRLEN];
				        inet_ntop(AF_INET, &(PdnAddr.ip_type.ipv4), ipStr, INET_ADDRSTRLEN);
					session_ctxt->set_pdn_address(ipStr);

					// Bearer ID
					session_ctxt->set_bearer_id(5);

					// S11 SGW GTP-C TEID
					const fteid& s11SgwCTeid = sessioncontext_p->getS11SgwCtrlFteid().fteid_m;

					ss.str("");
					memset(ipStr, 0, INET_ADDRSTRLEN);
					uint32_t ip = ntohl(s11SgwCTeid.ip.ipv4.s_addr);

					inet_ntop(AF_INET, &(ip), ipStr, INET_ADDRSTRLEN);
					ss << "IP " << ipStr << " TEID " << s11SgwCTeid.header.teid_gre;
					session_ctxt->set_s11_sgw_gtpc_teid(ss.str());

					// S5S8 PGW GTP-C TEID
					const fteid& s5s8PgwCTeid = sessioncontext_p->getS5S8PgwCtrlFteid().fteid_m;

					ss.str("");
                                        memset(ipStr, 0, INET_ADDRSTRLEN);
					ip = ntohl(s5s8PgwCTeid.ip.ipv4.s_addr);

					inet_ntop(AF_INET, &(ip), ipStr, INET_ADDRSTRLEN);
					ss << "IP " << ipStr << " TEID " << s5s8PgwCTeid.header.teid_gre;
					session_ctxt->set_s5_pgw_gtpc_teid(ss.str());
					
					BearerContext* bearerCtxt = sessioncontext_p->getBearerContext();
					// S1U ENB TEID
					const fteid& s1uEnbTeid = bearerCtxt->getS1uEnbUserFteid().fteid_m;

					ss.str("");
                    memset(ipStr, 0, INET_ADDRSTRLEN);
					ip = ntohl(s1uEnbTeid.ip.ipv4.s_addr);

                    inet_ntop(AF_INET, &(ip), ipStr, INET_ADDRSTRLEN);
                    ss << "IP " << ipStr << " TEID " << s1uEnbTeid.header.teid_gre;
					session_ctxt->set_s1u_enb_teid(ss.str());

					// S1U SGW TEID
				 	const fteid& s1uSgwTeid = bearerCtxt->getS1uSgwUserFteid().fteid_m;
					ss.str("");
                                        memset(ipStr, 0, INET_ADDRSTRLEN);
					ip = ntohl(s1uSgwTeid.ip.ipv4.s_addr);

                                        inet_ntop(AF_INET, &(ip), ipStr, INET_ADDRSTRLEN);
                                        ss << "IP " << ipStr << " TEID " << s1uSgwTeid.header.teid_gre;
                                        session_ctxt->set_s1u_sgw_teid(ss.str());

                                        // S5-U PGW TEID
                                        const fteid& s5uPgwTeid = bearerCtxt->getS5S8PgwUserFteid().fteid_m;
                                        ss.str("");
                                        memset(ipStr, 0, INET_ADDRSTRLEN);
                                        ip = ntohl(s5uPgwTeid.ip.ipv4.s_addr);

                                        inet_ntop(AF_INET, &(ip), ipStr, INET_ADDRSTRLEN);
                                        ss << "IP " << ipStr << " TEID " << s5uPgwTeid.header.teid_gre;
                                        session_ctxt->set_s5u_pgw_teid(ss.str());					

				}
			}	
		}
	}
	return Status::OK;

  }

  Status GetUeContext(ServerContext* context, const UeContextReqBuf* request,
                  UeContextRespBuf* reply) override 
  {
        SM::ControlBlock* controlBlk_p = mme::SubsDataGroupManager::Instance()->findControlBlock((uint32_t)request->id());
        GetUeContextInfo(controlBlk_p, reply);

        return Status::OK;
  }

  Status ShowAllMobileContexts(ServerContext* context, const Empty* request,
                      ServerWriter<UeContextRespBuf>* writer) override 
  {
      for (uint32_t i = 1; i <= 8000; i++)
      {
              SM::ControlBlock* controlBlk_p = mme::SubsDataGroupManager::Instance()->findControlBlock(i);
              if (controlBlk_p != NULL && controlBlk_p->getPermDataBlock() != NULL)
              {
                      UeContextRespBuf reply;
                      GetUeContextInfo(controlBlk_p, &reply);
                      writer->Write(reply);
              }
      }
      return Status::OK;
  }

  Status GetDebugUeContext(ServerContext* context, const UeContextReqBuf* request,
		  	EventInfoRespBuf* reply) override {
	SM::ControlBlock* controlBlk_p = mme::SubsDataGroupManager::Instance()->findControlBlock((uint32_t)request->id());
	if( controlBlk_p )
	{
		UEContext* uecontext_p=dynamic_cast<UEContext*>(controlBlk_p->getPermDataBlock());
		if( uecontext_p )
		{
			MmContext* mmCtxt = uecontext_p->getMmContext();
			UE_State_e uestate = mmCtxt->getMmState();
        		reply->set_ue_state(UEStates[static_cast<int>(uestate)]);
		}
		deque<SM::debugEventInfo> evtQ = controlBlk_p->getDebugInfoQueue();
        	for(auto it=evtQ.begin();it!=evtQ.end();it++)
		{
        		EventInfoRespBuf_EventInfoBuf* EvtInfo = reply->add_eventinfo();
	                string strEvent = SM::SmUtility::Instance()->convertEventToString(it->event);
        	        EvtInfo->set_event(strEvent);
                	string strState = SM::SmUtility::Instance()->convertStateToString(it->state);
	                EvtInfo->set_state(strState);
        	        EvtInfo->set_time(ctime(&(it->evt_time)));
        	}
	}
	return Status::OK;
  }

  Status GetProcStats(ServerContext* context,const Empty* request,
		  ProcedureStatsRespBuf* reply) override {
	/*time_t my_time = time(NULL);
	my_time = time(NULL);
        cout << "Req recieved at server" << ctime(&my_time);*/
        reply->set_num_of_air_sent(ProcedureStats::num_of_air_sent);
		reply->set_num_of_ulr_sent(ProcedureStats::num_of_ulr_sent);
        reply->set_num_of_processed_aia(ProcedureStats::num_of_processed_aia);
        reply->set_num_of_processed_ula(ProcedureStats::num_of_processed_ula);
        reply->set_num_of_auth_req_to_ue_sent(ProcedureStats::num_of_auth_req_to_ue_sent);
        reply->set_num_of_processed_auth_response(ProcedureStats::num_of_processed_auth_response);
        reply->set_num_of_sec_mode_cmd_to_ue_sent(ProcedureStats::num_of_sec_mode_cmd_to_ue_sent);
        reply->set_num_of_processed_sec_mode_resp(ProcedureStats::num_of_processed_sec_mode_resp);
        reply->set_num_of_esm_info_req_to_ue_sent(ProcedureStats::num_of_esm_info_req_to_ue_sent);
        reply->set_num_of_handled_esm_info_resp(ProcedureStats::num_of_handled_esm_info_resp);
        reply->set_num_of_cs_req_to_sgw_sent(ProcedureStats::num_of_cs_req_to_sgw_sent);
        reply->set_num_of_processed_cs_resp(ProcedureStats::num_of_processed_cs_resp);
        reply->set_num_of_init_ctxt_req_to_ue_sent(ProcedureStats::num_of_init_ctxt_req_to_ue_sent);
        reply->set_num_of_processed_init_ctxt_resp(ProcedureStats::num_of_processed_init_ctxt_resp);
        reply->set_num_of_mb_req_to_sgw_sent(ProcedureStats::num_of_mb_req_to_sgw_sent);
        reply->set_num_of_processed_attach_cmp_from_ue(ProcedureStats::num_of_processed_attach_cmp_from_ue);
        reply->set_num_of_processed_mb_resp(ProcedureStats::num_of_processed_mb_resp);
        reply->set_num_of_attach_done(ProcedureStats::num_of_attach_done);
        reply->set_num_of_del_session_req_sent(ProcedureStats::num_of_del_session_req_sent);
        reply->set_num_of_purge_req_sent(ProcedureStats::num_of_purge_req_sent);
        reply->set_num_of_processed_del_session_resp(ProcedureStats::num_of_processed_del_session_resp);
        reply->set_num_of_processed_pur_resp(ProcedureStats::num_of_processed_pur_resp);
        reply->set_num_of_detach_accept_to_ue_sent(ProcedureStats::num_of_detach_accept_to_ue_sent);
        reply->set_num_of_processed_detach_accept(ProcedureStats::num_of_processed_detach_accept);
        reply->set_num_of_ue_ctxt_release(ProcedureStats::num_of_ue_ctxt_release);
        reply->set_num_of_processed_ctxt_rel_resp(ProcedureStats::num_of_processed_ctxt_rel_resp);
        reply->set_num_of_subscribers_attached(ProcedureStats::num_of_subscribers_attached);
        reply->set_num_of_rel_access_bearer_req_sent(ProcedureStats::num_of_rel_access_bearer_req_sent);
        reply->set_num_of_rel_access_bearer_resp_received(ProcedureStats::num_of_rel_access_bearer_resp_received);
        reply->set_num_of_s1_rel_req_received(ProcedureStats::num_of_s1_rel_req_received);
        reply->set_num_of_s1_rel_cmd_sent(ProcedureStats::num_of_s1_rel_cmd_sent);
        reply->set_num_of_s1_rel_comp_received(ProcedureStats::num_of_s1_rel_comp_received);
        reply->set_num_of_clr_received(ProcedureStats::num_of_clr_received);
        reply->set_num_of_cla_sent(ProcedureStats::num_of_cla_sent);
        reply->set_num_of_detach_req_to_ue_sent(ProcedureStats::num_of_detach_req_to_ue_sent);
        reply->set_num_of_detach_accept_from_ue(ProcedureStats::num_of_detach_accept_from_ue);		
        reply->set_total_num_of_subscribers(ProcedureStats::total_num_of_subscribers);
        reply->set_num_of_subscribers_detached(ProcedureStats::num_of_subscribers_detached); 	
        reply->set_num_of_tau_response_to_ue_sent(ProcedureStats::num_of_tau_response_to_ue_sent);
        reply->set_num_of_service_request_received(ProcedureStats::num_of_service_request_received);
        reply->set_num_of_service_reject_sent(ProcedureStats::num_of_service_reject_sent);
        reply->set_num_of_attach_reject_sent(ProcedureStats::num_of_attach_reject_sent);
        reply->set_num_of_emm_info_sent(ProcedureStats::num_of_emm_info_sent);
        reply->set_num_of_ddn_received(ProcedureStats::num_of_ddn_received);
        reply->set_num_of_ddn_ack_sent(ProcedureStats::num_of_ddn_ack_sent);
        reply->set_num_of_attach_req_received(ProcedureStats::num_of_attach_req_received);
        reply->set_num_of_detach_req_received(ProcedureStats::num_of_detach_req_received);
        reply->set_num_of_tau_req_received(ProcedureStats::num_of_tau_req_received);
        reply->set_num_of_paging_request_sent(ProcedureStats::num_of_paging_request_sent);
        reply->set_num_of_id_req_sent(ProcedureStats::num_of_id_req_sent);
        reply->set_num_of_id_resp_received(ProcedureStats::num_of_id_resp_received);
        reply->set_num_of_auth_reject_sent(ProcedureStats::num_of_auth_reject_sent);
        reply->set_num_of_ho_required_received(ProcedureStats::num_of_ho_required_received);
        reply->set_num_of_ho_request_to_target_enb_sent(ProcedureStats::num_of_ho_request_to_target_enb_sent);
        reply->set_num_of_ho_request_ack_received(ProcedureStats::num_of_ho_request_ack_received);
        reply->set_num_of_ho_command_to_src_enb_sent(ProcedureStats::num_of_ho_command_to_src_enb_sent);
        reply->set_num_of_ho_notify_received(ProcedureStats::num_of_ho_notify_received);
        reply->set_num_of_ho_complete(ProcedureStats::num_of_ho_complete);
        reply->set_num_of_ho_failure_received(ProcedureStats::num_of_ho_failure_received);
        reply->set_num_of_ho_prep_failure_sent(ProcedureStats::num_of_ho_prep_failure_sent);
        reply->set_num_of_ho_cancel_received(ProcedureStats::num_of_ho_cancel_received);
        reply->set_num_of_ho_cancel_ack_sent(ProcedureStats::num_of_ho_cancel_ack_sent);



        return Status::OK;
  }

};

void * RunServer(void* data) {
  std::string server_address("0.0.0.0:50051");

  MmeGrpcCliServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);

  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  
  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();

  return NULL;
}

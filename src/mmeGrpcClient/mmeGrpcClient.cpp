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
#include <google/protobuf/text_format.h>
#include <../mmeGrpcProtos/mmeGrpc.grpc.pb.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using mmeGrpc::UeContextReqBuf;
using mmeGrpc::UeContextRespBuf;
using mmeGrpc::UeContextRespBuf_SessionContextRespBuf;
using mmeGrpc::MmeGrpcCli;
using mmeGrpc::Empty;
using mmeGrpc::ProcedureStatsRespBuf;
using mmeGrpc::EventInfoRespBuf;
using mmeGrpc::EventInfoRespBuf_EventInfoBuf;

using namespace std;

class MmeGrpcCliClient {
 public:
  MmeGrpcCliClient(std::shared_ptr<Channel> channel)
      : stub_(MmeGrpcCli::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  void GetUeContext(const int32_t id) {
    // Data we are sending to the server.
    UeContextReqBuf request;
    request.set_id(id);

    // Container for the data we expect from the server.
    UeContextRespBuf reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->GetUeContext(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {

	    /*std::string textString;
	    google::protobuf::TextFormat::PrintToString(reply, &textString);
	    std::cout << textString << std::endl;
		*/
	    cout << "-----mobile context----- " << id << endl; 
	    cout << "IMSI               " << reply.imsi() << endl;
	    cout << "MSISDN             " << reply.msisdn() << endl;
	    cout << "TAI                " << reply.tai() << endl;
	    cout << "EUTRAN CGI         " << reply.eutran_cgi() << endl;
	    cout << "Context ID         " << reply.context_id() << endl;
	    cout << "Enb UE s1ap Teid   " << reply.enb_ue_s1ap_id() << endl;
	    cout << "UE State           " << reply.ue_state() << endl;
	    
	    int sessioncontext_size=reply.sessioncontext_size();	    
	    for(int i=0;i<sessioncontext_size;i++)
	    {
	    	const UeContextRespBuf_SessionContextRespBuf& sessioncontext = reply.sessioncontext(i);

		cout << "-----session context------ " << i+1 << endl;
		cout << "	apn                     " << sessioncontext.apn() << endl;
		cout << " 	PDN address             " << sessioncontext.pdn_address() << endl;
		cout << " 	Bearer ID               " << sessioncontext.bearer_id() << endl;
		cout << "	s11 sgw gtpc teid       " << sessioncontext.s11_sgw_gtpc_teid() << endl;
		cout << "	s5 pgw gtpc teid        " << sessioncontext.s5_pgw_gtpc_teid() << endl;
		cout << "	s1u enb teid            " << sessioncontext.s1u_enb_teid() << endl;
		cout << "	s1u sgw teid            " << sessioncontext.s1u_sgw_teid() << endl;
		cout << "	s5u pgw teid            " << sessioncontext.s5u_pgw_teid() << endl;
	    }

    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
    }
  }

  void ShowAllMobileContexts() {
     UeContextRespBuf reply;

     Empty request;
     ClientContext context;

     std::unique_ptr<ClientReader<UeContextRespBuf> > reader(
                stub_->ShowAllMobileContexts(&context, request));

     while (reader->Read(&reply))
     {
            cout << "-----mobile context----- " << endl;
            cout << "IMSI               " << reply.imsi() << endl;
            cout << "MSISDN             " << reply.msisdn() << endl;
            cout << "TAI                " << reply.tai() << endl;
            cout << "EUTRAN CGI         " << reply.eutran_cgi() << endl;
            cout << "Context ID         " << reply.context_id() << endl;
            cout << "Enb UE s1ap Teid   " << reply.enb_ue_s1ap_id() << endl;
            cout << "UE State           " << reply.ue_state() << endl;

            int sessioncontext_size=reply.sessioncontext_size();
            for(int i=0;i<sessioncontext_size;i++)
            {
                const UeContextRespBuf_SessionContextRespBuf& sessioncontext = reply.sessioncontext(i);

                cout << "-----session context------ " << i+1 << endl;
                cout << "       apn                     " << sessioncontext.apn() << endl;
                cout << "       PDN address             " << sessioncontext.pdn_address() << endl;
                cout << "       Bearer ID               " << sessioncontext.bearer_id() << endl;
                cout << "       s11 sgw gtpc teid       " << sessioncontext.s11_sgw_gtpc_teid() << endl;
                cout << "       s5 pgw gtpc teid        " << sessioncontext.s5_pgw_gtpc_teid() << endl;
                cout << "       s1u enb teid            " << sessioncontext.s1u_enb_teid() << endl;
                cout << "       s1u sgw teid            " << sessioncontext.s1u_sgw_teid() << endl;
                cout << "       s5u pgw teid            " << sessioncontext.s5u_pgw_teid() << endl;
            }

     }
     Status status = reader->Finish();
     if (status.ok()) {
              std::cout << "show all mobile-contexts rpc succeeded." << std::endl;
     } else {
              std::cout << "show all mobile-contexts rpc failed." << std::endl;
     }
  }


  void GetProcStats(){
    // Container for the data we expect from the server.
    ProcedureStatsRespBuf reply;

    Empty request;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    // The actual RPC.
    Status status = stub_->GetProcStats(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
            cout << endl << "-----procedure stats-----" << endl;
	     cout << "num_of_subs_attached                    " << reply.num_of_subscribers_attached() << endl;
            cout << "num_of_air_sent                         " << reply.num_of_air_sent() << endl;
	     cout << "num_of_ulr_sent                         " << reply.num_of_ulr_sent() << endl;
            cout << "num_of_processed_aia                    " << reply.num_of_processed_aia() << endl;
            cout << "num_of_processed_ula                    " << reply.num_of_processed_ula() << endl;
            cout << "num_of_auth_req_to_ue_sent              " << reply.num_of_auth_req_to_ue_sent() << endl;
            cout << "num_of_processed_auth_response          " << reply.num_of_processed_auth_response() << endl;
            cout << "num_of_sec_mode_cmd_to_ue_sent          " << reply.num_of_sec_mode_cmd_to_ue_sent() << endl;
            cout << "num_of_processed_sec_mode_resp          " << reply.num_of_processed_sec_mode_resp() << endl;
            cout << "num_of_esm_info_req_to_ue_sent          " << reply.num_of_esm_info_req_to_ue_sent() << endl;
            cout << "num_of_handled_esm_info_resp            " << reply.num_of_handled_esm_info_resp() << endl;
            cout << "num_of_cs_req_to_sgw_sent               " << reply.num_of_cs_req_to_sgw_sent() << endl;
            cout << "num_of_processed_cs_resp                " << reply.num_of_processed_cs_resp() << endl;
            cout << "num_of_init_ctxt_req_to_ue_sent         " << reply.num_of_init_ctxt_req_to_ue_sent() << endl;
            cout << "num_of_processed_init_ctxt_resp         " << reply.num_of_processed_init_ctxt_resp() << endl;
            cout << "num_of_mb_req_to_sgw_sent               " << reply.num_of_mb_req_to_sgw_sent() << endl;
            cout << "num_of_processed_attach_cmp_from_ue     " << reply.num_of_processed_attach_cmp_from_ue() << endl;
            cout << "num_of_processed_mb_resp                " << reply.num_of_processed_mb_resp() << endl;
            cout << "num_of_attach_done                      " << reply.num_of_attach_done() << endl;
            cout << "num_of_del_session_req_sent             " << reply.num_of_del_session_req_sent() << endl;
            cout << "num_of_purge_req_sent                   " << reply.num_of_purge_req_sent() << endl;
            cout << "num_of_processed_del_session_resp       " << reply.num_of_processed_del_session_resp() << endl;
            cout << "num_of_processed_pur_resp               " << reply.num_of_processed_pur_resp() << endl;
            cout << "num_of_detach_accept_to_ue_sent         " << reply.num_of_detach_accept_to_ue_sent() << endl;
            cout << "num_of_processed_detach_accept          " << reply.num_of_processed_detach_accept() << endl;
            cout << "num_of_ue_ctxt_release                  " << reply.num_of_ue_ctxt_release() << endl;
            cout << "num_of_processed_ctxt_rel_resp          " << reply.num_of_processed_ctxt_rel_resp() << endl;
	    cout << "num_of_rel_access_bearer_req_sent       " << reply.num_of_rel_access_bearer_req_sent() << endl;											    cout << "num_of_rel_access_bearer_resp_received  " << reply.num_of_rel_access_bearer_resp_received() << endl;
	    cout << "num_of_s1_rel_req_received              " << reply.num_of_s1_rel_req_received() << endl;
	    cout << "num_of_s1_rel_cmd_sent                  " << reply.num_of_s1_rel_cmd_sent() << endl;
	    cout << "num_of_s1_rel_comp_received             " << reply.num_of_s1_rel_comp_received() << endl;
	    cout << "num_of_clr_received                     " << reply.num_of_clr_received() << endl;
	    cout << "num_of_cla_sent                         " << reply.num_of_cla_sent() << endl;
	    cout << "num_of_detach_req_to_ue_sent            " << reply.num_of_detach_req_to_ue_sent() << endl;
	    cout << "num_of_detach_accept_from_ue            " << reply.num_of_detach_accept_from_ue() << endl;
	    cout << "num_of_ddn_received		     " << reply.num_of_ddn_received() << endl;
	    cout << "num_of_ddn_ack_sent		     " << reply.num_of_ddn_ack_sent() << endl;
	    cout << "total_num_of_subscribers                " << reply.total_num_of_subscribers() << endl;
	    cout << "num_of_subscribers_detached             " << reply.num_of_subscribers_detached() << endl;
	    cout << "num_of_tau_response_to_ue_sent          " << reply.num_of_tau_response_to_ue_sent() << endl;
	    cout << "num_of_service_request_received	     " << reply.num_of_service_request_received() << endl;
	    cout << "num_of_service_reject_sent	             " << reply.num_of_service_reject_sent() << endl;
	    cout << "num_of_attach_reject_sent               " << reply.num_of_attach_reject_sent() << endl;
	    cout << "num_of_emm_info_sent		     " << reply.num_of_emm_info_sent() << endl;
        cout << "num_of_attach_req_received          " << reply.num_of_attach_req_received() << endl;
        cout << "num_of_detach_req_received              " << reply.num_of_detach_req_received() << endl;
        cout << "num_of_tau_req_received                 " << reply.num_of_tau_req_received() << endl;
        cout << "num_of_paging_request_sent              " << reply.num_of_paging_request_sent() << endl;
        cout << "num_of_id_req_sent              " << reply.num_of_id_req_sent() << endl;
        cout << "num_of_id_resp_received             " << reply.num_of_id_resp_received() << endl;
        cout << "num_of_auth_reject_sent             " << reply.num_of_auth_reject_sent() << endl;
	    cout << "num_of_ho_required_received    	     " << reply.num_of_ho_required_received() << endl;
	    cout << "num_of_ho_request_to_target_enb_sent    " << reply.num_of_ho_request_to_target_enb_sent() << endl;
	    cout << "num_of_ho_request_ack_received          " << reply.num_of_ho_request_ack_received() << endl;
	    cout << "num_of_ho_command_to_src_enb_sent       " << reply.num_of_ho_command_to_src_enb_sent() << endl;
	    cout << "num_of_ho_notify_received               " << reply.num_of_ho_notify_received() << endl;
	    cout << "num_of_ho_complete     		     " << reply.num_of_ho_complete() << endl;
	    cout << "num_of_ho_failure_received     		 " << reply.num_of_ho_failure_received() << endl;
	    cout << "num_of_ho_prep_failure_sent     		 " << reply.num_of_ho_prep_failure_sent() << endl;
	    cout << "num_of_ho_cancel_received     		     " << reply.num_of_ho_cancel_received() << endl;
	    cout << "num_of_ho_cancel_ack_sent     		     " << reply.num_of_ho_cancel_ack_sent() << endl;
            cout << "num_of_erab_mod_ind_received	     " << reply.num_of_erab_mod_ind_received() << endl;
	    cout << "num_of_erab_mod_conf_sent		     " << reply.num_of_erab_mod_conf_sent() << endl;
	    cout << "num_of_erab_mod_ind_timeout             " << reply.num_of_erab_mod_ind_timeout() << endl;
	    cout << "num_of_abort_erab_mod_indication        " << reply.num_of_abort_erab_mod_indication() << endl;

    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
    }
  }

  void GetDebugUeContext(const int32_t id){
    // Container for the data we expect from the server.
    EventInfoRespBuf reply;

    UeContextReqBuf request;
    request.set_id(id);

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    // The actual RPC.
    Status status = stub_->GetDebugUeContext(&context, request, &reply);

    // Act upon its status.
    if (status.ok())
    {
	    int event_info_buf_size=reply.eventinfo_size();
	    cout << "EVENT\t\t\t\t\t\tSTATE\t\t\t\t\t\tTIMESTAMP" << endl;
	    cout << "-----------------------------------------------------------------" << endl;
	    for(int i=0; i<event_info_buf_size; i++)
	    {
		    const EventInfoRespBuf_EventInfoBuf& event_info=reply.eventinfo(i);

		    cout << event_info.event() <<  "\t\t\t\t\t\t"
		         << event_info.state()  << "\t\t\t\t\t\t"
		         << event_info.time() << endl;
	    }
    } else {
      cout << status.error_code() << ": " << status.error_message() << endl;
    }
  }


 private:
  std::unique_ptr<MmeGrpcCli::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).

  if((0==strcmp(argv[1],"mme-app"))&&(0==strcmp(argv[2],"show"))&&(0==strcmp(argv[3],"mobile-context")))
  {
  	MmeGrpcCliClient MmeGrpcCli(grpc::CreateChannel(
      		"localhost:50051", grpc::InsecureChannelCredentials()));
	stringstream sid;
	sid << argv[4];
  	int32_t id = 1;
	sid >> id;

  	MmeGrpcCli.GetUeContext(id);
  }
  else if((0==strcmp(argv[1],"mme-app"))&&(0==strcmp(argv[2],"show"))&&(0==strcmp(argv[3],"procedure-stats")))
  {
	MmeGrpcCliClient MmeGrpcCli(grpc::CreateChannel(
                "localhost:50051", grpc::InsecureChannelCredentials()));

        MmeGrpcCli.GetProcStats();
  }
 else if((0==strcmp(argv[1],"mme-app"))&&(0==strcmp(argv[2],"show"))&&(0==strcmp(argv[3],"mobile-contexts-all")))
  {
        MmeGrpcCliClient MmeGrpcCli(grpc::CreateChannel(
                "localhost:50051", grpc::InsecureChannelCredentials()));

        MmeGrpcCli.ShowAllMobileContexts();
  }

  else if((0==strcmp(argv[1],"mme-app"))&&(0==strcmp(argv[2],"debug"))&&(0==strcmp(argv[3],"show")) &&(0==strcmp(argv[4],"mobile-context")))
  {
  	MmeGrpcCliClient MmeGrpcCli(grpc::CreateChannel(
                "localhost:50051", grpc::InsecureChannelCredentials()));
        stringstream sid;
        sid << argv[5];
        int32_t id = 1;
        sid >> id;
	cout << endl << "-----debug mobile context----- " << id << endl;
	MmeGrpcCli.GetUeContext(id);
	MmeGrpcCli.GetDebugUeContext(id);
  }
  return 0;
}

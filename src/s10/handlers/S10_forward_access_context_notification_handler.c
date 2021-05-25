#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "err_codes.h"
#include "s10_options.h"
#include "ipc_api.h"
#include "s10.h"
#include "s10.h"
#include "s10_config.h"
#include "msgType.h"

#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"

extern int g_s10_fd;
extern int g_resp_fd;

extern struct GtpV2Stack* gtpStack_gp;

int
S10_fwd_acc_ctxt_noti_handler(MsgBuffer* message, GtpV2MessageHeader* hdr, uint32_t tmme_ip){


	struct Fwd_Acc_Ctxt_Notif_Q_msg fAcCtxtNot_info={0};

	fAcCtxtNot_info.s10_mme_cp_teid=hdr->teid;

	fAcCtxtNot_info.header.msg_type=forward_access_context_notification;

	delete_gtp_transaction(hdr->sequenceNumber);

	ForwardAccessContextNotificationMsgData msgData;

	memset(&msgData,0,sizeof(ForwardAccessContextNotificationMsgData));

	bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
		if(rc == false)
		{
				log_msg(LOG_ERROR, "s10_ID_resp_handler: "
									"Failed to decode Forward Access Context Notification Msg %d",
									hdr->teid);
				return E_PARSING_FAILED;
		}

//	fAcCtxtNot_info.f_container=msgData.eUtranTransparentContainer;  -- need to change in GTP datasheet.

	fAcCtxtNot_info.header.destInstAddr = htonl(mmeAppInstanceNum_c);

	fAcCtxtNot_info.header.srcInstAddr =  htonl(s10AppInstanceNum_c);

	send_tipc_message(g_resp_fd, mmeAppInstanceNum_c, (char *)&fAcCtxtNot_info, sizeof(struct Fwd_Acc_Ctxt_Notif_Q_msg));


}

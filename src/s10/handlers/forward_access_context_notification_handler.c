#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "log.h"
#include "err_codes.h"
#include "ipc_api.h"
#include "msgType.h"

#include "gtpv2c.h"
#include "gtpv2c_ie.h"
#include "s11_config.h"
#include <gtpV2StackWrappers.h>
#include "gtp_cpp_wrapper.h"
#include "s11.h"
#include "s10_config.h"
#include "s10.h"

extern int g_s10_fd;
extern struct sockaddr_in g_s10_cp_addr;
extern socklen_t g_s10_serv_size;

extern s10_config_t g_s10_cfg;

/****Global and externs end***/


struct FWD_ACC_CTXT_NOTIF_Q_msg *g_fAcCtNotInfo;

extern struct GtpV2Stack* gtpStack_gp;




static int forward_access_context_notification_processing(struct FWD_ACC_CTXT_NOTIF_Q_msg* g_fAcCtNotInfo){
	struct MsgBuffer* fAcCNot_msgBuf=createMsgBuffer(S10_MSGBUF_SIZE);
	 if(fAcCNot_msgBuf == NULL)
	    {
	        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.");
	        return -1;
	    }

	 struct sockaddr_in tmme_addr={0};

	 GtpV2MessageHeader gtpHeader;

	 gtpHeader.msgType=GTP_FORWARD_ACCESS_CONTEXT_NOTIFICATION;

	 uint32_t seq = 0;
	 	get_sequence(&seq);
	 	gtpHeader.sequenceNumber = seq;
	 	gtpHeader.teidPresent = true;
	 	gtpHeader.teid = 0; // Need to check from 3gpp

	 	tmme_addr.sin_family = AF_INET;
	 	tmme_addr.sin_port = htons(g_s10_cfg.egtp_def_port);

	 	if(g_fAcCtNotInfo->target_mme_ip != 0) {
	 	    	tmme_addr.sin_addr.s_addr = g_fAcCtNotInfo->target_mme_ip;
	 	    } else {
	 	    	tmme_addr = g_s10_cp_addr;
	 	    }

	 	log_msg(LOG_INFO,"In Forward Access Context Notification handler->ue_idx:%d",g_fAcCtNotInfo->ue_idx);

	 	add_gtp_transaction(gtpHeader.sequenceNumber,
	 			g_fAcCtNotInfo->ue_idx);

	 ForwardAccessContextNotificationMsgData msgData;

	 memset(&msgData, 0, sizeof(msgData));

	 msgData.eUtranTransparentContainerIePresent=true;

	 msgData.eUtranTransparentContainer.containerType=3;
	 msgData.eUtranTransparentContainer.fContainerField=0;

	 GtpV2Stack_buildGtpV2Message(gtpStack_gp, fAcCNot_msgBuf, &gtpHeader, &msgData);

	 int res = sendto (
	 			g_s10_fd,
	 			MsgBuffer_getDataPointer(fAcCNot_msgBuf),
	 			MsgBuffer_getBufLen(fAcCNot_msgBuf), 0,
	 			(struct sockaddr*)(&tmme_addr),
	 			g_s10_serv_size);
	 	if (res < 0) {
	 		log_msg(LOG_ERROR,"Error in sendto in detach stage 3 post to next");
	 	}

	 	log_msg(LOG_INFO,"%d bytes sent. Err : %d, %s",res,errno,
	 			strerror(errno));

	 	MsgBuffer_free(fAcCNot_msgBuf);

	 	return SUCCESS;

}




void* forward_access_context_notification_handler(void *data){
	log_msg(LOG_INFO, "Forward Access Context Notification handler");
	forward_access_context_notification_processing((struct FWD_ACC_CTXT_NOTIF_Q_msg *) data);

	return NULL;
}

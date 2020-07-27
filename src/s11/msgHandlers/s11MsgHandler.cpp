/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <msgHandlers/s11MsgHandler.h>
#include <event.h>
#include <ipcTypes.h>
#include <log.h>
#include <utils/mmeCommonUtils.h>
#include <eventMessage.h>
#include "gtpv2c.h"
#include "gtpv2c_ie.h"
#include "s11_config.h"
#include <gtpV2StackWrappers.h>


extern int g_s11_fd;
extern socklen_t g_s11_serv_size;
extern s11_config_t *s11_cfg;
volatile uint32_t g_s11_sequence = 1;
MsgBuffer*  csReqMsgBuf_p = NULL;
MsgBuffer* mbReqMsgBuf_p = NULL;
MsgBuffer* dsReqMsgBuf_p = NULL;
MsgBuffer* rbReqMsgBuf_p = NULL;
MsgBuffer* ddnAckMsgBuf_p = NULL;

extern struct GtpV2Stack* gtpStack_gp;

using namespace cmn;

S11MsgHandler::S11MsgHandler()
{
	csReqMsgBuf_p = createMsgBuffer(4096);
	mbReqMsgBuf_p = createMsgBuffer(4096);
	dsReqMsgBuf_p = createMsgBuffer(4096);
	rbReqMsgBuf_p = createMsgBuffer(4096);
	ddnAckMsgBuf_p = createMsgBuffer(4096);

	if (csReqMsgBuf_p == NULL || mbReqMsgBuf_p == NULL || dsReqMsgBuf_p == NULL || rbReqMsgBuf_p == NULL || ddnAckMsgBuf_p == NULL)
	{
		log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
        assert(0);
	}
}

S11MsgHandler::~S11MsgHandler()
{

}

S11MsgHandler* S11MsgHandler::Instance()
{
	static S11MsgHandler msgHandler;
	return &msgHandler;
}

// Starting point 
void S11MsgHandler::handleMmeMessage_v(cmn::IpcEventMessage* eMsg)
{
	log_msg(LOG_INFO, "mmea-app - handleMMEMessage_v\n");

	if (eMsg == NULL)
		return;

	utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
	if (msgBuf == NULL)
    {
        log_msg(LOG_INFO, "mme-app Message Buffer is empty \n");
        delete eMsg;
        return;
    }
    log_msg(LOG_INFO, "message size %d in s1 ipc message \n",msgBuf->getLength());
    log_msg(LOG_INFO, "message size %d  \n",sizeof(gtp_outgoing_msg_data_t));
    log_msg(LOG_INFO, "message size %d  \n",sizeof(gtp_outgoing_msgs_t));
	if (msgBuf->getLength() < sizeof (gtp_outgoing_msgs_t))
	{
	    log_msg(LOG_INFO, "Not enough bytes in s1 ipc message"
	            "Received %d but should be %d\n", msgBuf->getLength(),
	            sizeof (gtp_outgoing_msgs_t));

	    	delete eMsg;
	    	return;
	}

	gtp_outgoing_msg_data_t *msgData_p = (gtp_outgoing_msg_data_t*)(msgBuf->getDataPointer());

	log_msg(LOG_INFO, "mme-app - handleMmeMessage_v %d\n",msgData_p->msg_data.msg_type);
	switch (msgData_p->msg_data.msg_type)
	{
		case msg_type_t::create_session_request:
			handleCreateSessionRequestMsg_v(eMsg);
			break;
        case msg_type_t::modify_bearer_request:
            break;

        case msg_type_t::delete_session_request:
            break;

        case msg_type_t::release_bearer_request:
            break;

        case msg_type_t::ddn_acknowledgement:
            break;

		default:
			log_msg(LOG_INFO, "Unhandled S1 Message %d \n", msgData_p->msg_data.msg_type);
			delete eMsg;
	}
}

uint32_t
convert_imsi_to_digits_array(uint8_t *src, uint8_t *dest, uint32_t len)
{
	uint8_t msb_digit = 0;
	uint8_t lsb_digit = 0;
	uint8_t num_of_digits = 0;

	for(uint32_t i = 0; i < len; i++)
	{
		lsb_digit = ((src[i] & 0xF0) >> 4);
		dest[(2*i) + 1] = lsb_digit;

		msb_digit = (src[i] & 0x0F);
		dest[2*i] = msb_digit;

		if (lsb_digit != 0x0F)
			num_of_digits = num_of_digits + 2;
		else
			num_of_digits++;
	}

	return num_of_digits;
}

void 
S11MsgHandler::handleCreateSessionRequestMsg_v(IpcEventMessage* eMsg)
{
	log_msg(LOG_INFO, "S1 - handleInitUeAttachRequestMsg_v\n");

	utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
	if (msgBuf == NULL)
        return; // error 

	gtp_outgoing_msg_data_t* msgData_p = (gtp_outgoing_msg_data_t*)(msgBuf->getDataPointer());

    struct CS_Q_msg *csr_Q_msg_m = &msgData_p->msg_data.csr_req_msg;

    struct sockaddr_in sgw_addr = {0};
	GtpV2MessageHeader gtpHeader;


	gtpHeader.msgType = GTP_CREATE_SESSION_REQ;
	gtpHeader.sequenceNumber = g_s11_sequence;
	gtpHeader.teidPresent = true;
	gtpHeader.teid = 0; 

	sgw_addr.sin_family = AF_INET;
	sgw_addr.sin_port = htons(s11_cfg->egtp_def_port);
    if(csr_Q_msg_m->sgw_ip != 0) {
        sgw_addr.sin_addr.s_addr = csr_Q_msg_m->sgw_ip;
    } else {
        sgw_addr.sin_addr.s_addr = s11_cfg->sgw_ip; 
    }
	
	g_s11_sequence++;
	
	log_msg(LOG_INFO,"In create session handler->ue_idx:%d\n",csr_Q_msg_m->ue_idx);

	CreateSessionRequestMsgData msgData;
	memset(&msgData, 0, sizeof(msgData));

	msgData.imsiIePresent = true;
	memset(msgData.imsi.imsiValue.digits, 0x0f, 16);
	

	uint8_t imsi_len =
			convert_imsi_to_digits_array(csr_Q_msg_m->IMSI,
					msgData.imsi.imsiValue.digits,
					BINARY_IMSI_LEN);

	msgData.imsi.imsiValue.length = imsi_len;
	log_msg(LOG_INFO, "IMSI Len: %d\n", imsi_len);

	msgData.msisdnIePresent = true;
	msgData.msisdn.msisdnValue.length = 10;
	for (uint8_t i = 1; i <= 5; i++)
	{
		msgData.msisdn.msisdnValue.digits[2*(i-1)] = (csr_Q_msg_m->MSISDN[i-1] & 0x0F);
		msgData.msisdn.msisdnValue.digits[(2*i) - 1] = ((csr_Q_msg_m->MSISDN[i-1] & 0xF0) >> 4);
	}

	struct TAI *tai = &(csr_Q_msg_m->tai);
	struct CGI *cgi = &(csr_Q_msg_m->utran_cgi);

	msgData.userLocationInformationIePresent = true;
	msgData.userLocationInformation.taipresent = true;
	msgData.userLocationInformation.ecgipresent = true;

	msgData.userLocationInformation.tai.trackingAreaCode = ntohs(tai->tac);
	msgData.userLocationInformation.tai.mccDigit1 = tai->plmn_id.idx[0] & 0x0F;
	msgData.userLocationInformation.tai.mccDigit2 = (tai->plmn_id.idx[0] & 0xF0) >> 4;
	msgData.userLocationInformation.tai.mccDigit3 = tai->plmn_id.idx[1] & 0x0F;
	msgData.userLocationInformation.tai.mncDigit1 = tai->plmn_id.idx[2] & 0x0F;
	msgData.userLocationInformation.tai.mncDigit2 = (tai->plmn_id.idx[2] & 0xF0) >> 4;
	msgData.userLocationInformation.tai.mncDigit3 = (tai->plmn_id.idx[1] & 0xF0) >> 4;

	msgData.userLocationInformation.ecgi.eUtranCellId = ntohl(cgi->cell_id);
	msgData.userLocationInformation.ecgi.mccDigit1 = cgi->plmn_id.idx[0] & 0x0F;
	msgData.userLocationInformation.ecgi.mccDigit2 = (cgi->plmn_id.idx[0] & 0xF0) >> 4;
	msgData.userLocationInformation.ecgi.mccDigit3 = cgi->plmn_id.idx[1] & 0x0F;
	msgData.userLocationInformation.ecgi.mncDigit1 = cgi->plmn_id.idx[2] & 0x0F;
	msgData.userLocationInformation.ecgi.mncDigit2 = (cgi->plmn_id.idx[2] & 0xF0) >> 4;
	msgData.userLocationInformation.ecgi.mncDigit3 = (cgi->plmn_id.idx[1] & 0xF0) >> 4;

	msgData.servingNetworkIePresent = true;
	msgData.servingNetwork.mccDigit1 = tai->plmn_id.idx[0] & 0x0F;
	msgData.servingNetwork.mccDigit2 = (tai->plmn_id.idx[0] & 0xF0) >> 4;
	msgData.servingNetwork.mccDigit3 = tai->plmn_id.idx[1] & 0x0F;
	msgData.servingNetwork.mncDigit1 = tai->plmn_id.idx[2] & 0x0F;
	msgData.servingNetwork.mncDigit2 = (tai->plmn_id.idx[2] & 0xF0) >> 4;
	msgData.servingNetwork.mncDigit3 = (tai->plmn_id.idx[1] & 0xF0) >> 4;

	msgData.ratType.ratType = 6;

	msgData.indicationFlagsIePresent = true;

	msgData.senderFTeidForControlPlane.ipv4present = true;
	msgData.senderFTeidForControlPlane.interfaceType = 10;
	msgData.senderFTeidForControlPlane.ipV4Address.ipValue = s11_cfg->local_egtp_ip;
	msgData.senderFTeidForControlPlane.teidGreKey = csr_Q_msg_m->ue_idx;

	msgData.pgwS5S8AddressForControlPlaneOrPmipIePresent = true;
	msgData.pgwS5S8AddressForControlPlaneOrPmip.ipv4present = true;
	msgData.pgwS5S8AddressForControlPlaneOrPmip.interfaceType = 7;
	msgData.pgwS5S8AddressForControlPlaneOrPmip.ipV4Address.ipValue = ntohl(csr_Q_msg_m->pgw_ip); /* host order address */

	msgData.accessPointName.apnValue.count = csr_Q_msg_m->selected_apn.len;
	memcpy(msgData.accessPointName.apnValue.values, csr_Q_msg_m->selected_apn.val, csr_Q_msg_m->selected_apn.len);

	msgData.selectionModeIePresent = true;
	msgData.selectionMode.selectionMode = 1;

	msgData.pdnTypeIePresent = true;
	msgData.pdnType.pdnType = 1;

	msgData.pdnAddressAllocationIePresent = true;
	msgData.pdnAddressAllocation.pdnType = 1;
	msgData.pdnAddressAllocation.ipV4Address.ipValue = csr_Q_msg_m->paa_v4_addr; /* host order - Get value from MME */

	msgData.maximumApnRestrictionIePresent = true;
	msgData.maximumApnRestriction.restrictionValue = 0;

	/* Bearer Context */
	msgData.bearerContextsToBeCreatedCount = 1;
	msgData.bearerContextsToBeCreated[0].epsBearerId.epsBearerId = 5;

	msgData.bearerContextsToBeCreated[0].bearerLevelQos.pci = 1;
	msgData.bearerContextsToBeCreated[0].bearerLevelQos.pl = 11;
	msgData.bearerContextsToBeCreated[0].bearerLevelQos.pvi = 0;
	msgData.bearerContextsToBeCreated[0].bearerLevelQos.qci = 9;

	uint32_t mbr_uplink = htonl(MBR_UPLINK);
	uint32_t mbr_downlink = htonl(MBR_DOWNLINK);

	msgData.bearerContextsToBeCreated[0].bearerLevelQos.maxBitRateUl.count = 5;
	msgData.bearerContextsToBeCreated[0].bearerLevelQos.maxBitRateDl.count = 5;
	memcpy(&msgData.bearerContextsToBeCreated[0].bearerLevelQos.maxBitRateUl.values, &mbr_uplink, sizeof(mbr_uplink));
	memcpy(&msgData.bearerContextsToBeCreated[0].bearerLevelQos.maxBitRateDl.values, &mbr_downlink, sizeof(mbr_downlink));
	msgData.bearerContextsToBeCreated[0].bearerLevelQos.guraranteedBitRateUl.count = 5;
	msgData.bearerContextsToBeCreated[0].bearerLevelQos.guaranteedBitRateDl.count = 5;

	msgData.aggregateMaximumBitRateIePresent = true;
	msgData.aggregateMaximumBitRate.maxMbrUplink = csr_Q_msg_m->max_requested_bw_ul;
	msgData.aggregateMaximumBitRate.maxMbrDownlink = csr_Q_msg_m->max_requested_bw_dl;

    log_msg(LOG_INFO, "PCO length = %d\n", csr_Q_msg_m->pco_length);
    if(csr_Q_msg_m->pco_length > 0)
    {
        msgData.protocolConfigurationOptionsIePresent = true;
        msgData.protocolConfigurationOptions.pcoValue.count = csr_Q_msg_m->pco_length;
        memcpy(&msgData.protocolConfigurationOptions.pcoValue.values[0], &csr_Q_msg_m->pco_options[0], csr_Q_msg_m->pco_length);
    }

	GtpV2Stack_buildGtpV2Message(gtpStack_gp, csReqMsgBuf_p, &gtpHeader, &msgData);

	log_msg(LOG_INFO, "send %d bytes.\n",MsgBuffer_getBufLen(csReqMsgBuf_p));

	int res = sendto (
			g_s11_fd,
			MsgBuffer_getDataPointer(csReqMsgBuf_p),
			MsgBuffer_getBufLen(csReqMsgBuf_p), 0,
			(struct sockaddr*)(&sgw_addr),
			g_s11_serv_size);
	if (res < 0) {
		log_msg(LOG_ERROR,"Error in sendto in detach stage 3 post to next\n");
	}

	log_msg(LOG_INFO,"%d bytes sent. Err : %d, %s\n",res,errno,
			strerror(errno));

	MsgBuffer_reset(csReqMsgBuf_p);

	return;
}

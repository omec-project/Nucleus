/*
 * Copyright 2020-present Open Networking Foundation
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
#include "s11.h"
#include <gtpV2StackWrappers.h>
#include "gtp_tables.h"
#include "utils/s11TimerUtils.h"
#include "utils/s11TimerTypes.h"


extern local_endpoint le; 
extern s11_config_t *s11_cfg;
volatile uint32_t g_s11_sequence = 1;

extern struct GtpV2Stack* gtpStack_gp;

using namespace cmn;
using namespace s11;

S11MsgHandler::S11MsgHandler()
{
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
void 
S11MsgHandler::handleMmeMessage_v(cmn::IpcEventMessage* eMsg)
{

    if (eMsg == NULL)
    {
        log_msg(LOG_INFO, "mme-app - handleMMEMessage_v message NULL \n");
        return;
    }

    utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
    {
        log_msg(LOG_INFO, "mme-app Message Buffer is empty \n");
        delete eMsg;
        return;
    }
    if (msgBuf->getLength() < sizeof (gtp_outgoing_msgs_t))
    {
        log_msg(LOG_INFO, "Not enough bytes in s1 ipc message"
                "Received %d but should be %d\n", msgBuf->getLength(),
                sizeof (gtp_outgoing_msgs_t));

        delete eMsg;
        return;
    }

    gtp_outgoing_msg_data_t *msgData_p = (gtp_outgoing_msg_data_t*)(msgBuf->getDataPointer());

    switch (msgData_p->msg_data.msg_type)
    {
        case msg_type_t::create_session_request:
            handleCreateSessionRequestMsg_v(eMsg);
            break;
        case msg_type_t::modify_bearer_request:
            handleModifyBearerRequestMsg_v(eMsg); 
            break;

        case msg_type_t::delete_session_request:
            handleDeleteSessionRequestMsg_v(eMsg);
            break;

        case msg_type_t::release_bearer_request:
            handleReleaseAccessBearerRequestMsg_v(eMsg);
            break;

        case msg_type_t::ddn_acknowledgement:
            handleDownlinkDataNotificationAckMsg_v(eMsg);
            break;

        default:
            log_msg(LOG_INFO, "Unhandled message (%d) received from mme-app \n", msgData_p->msg_data.msg_type);
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
    utils::MsgBuffer*  csReqMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
    if(csReqMsgBuf_p == NULL)
    {
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
        return;
    }
    log_msg(LOG_INFO, "S1 - handleInitUeAttachRequestMsg_v\n");

    utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
        return; // error 

    gtp_outgoing_msg_data_t* msgData_p = (gtp_outgoing_msg_data_t*)(msgBuf->getDataPointer());

    struct CS_Q_msg *csr_Q_msg_m = &msgData_p->msg_data.csr_req_msg;

    GtpV2MessageHeader gtpHeader;


    gtpHeader.msgType = GTP_CREATE_SESSION_REQ;
    gtpHeader.sequenceNumber = g_s11_sequence;
    gtpHeader.teidPresent = true;
    gtpHeader.teid = 0; 

    struct sockaddr_in sgw_addr = {0};
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

    if(csr_Q_msg_m->pco_length > 0)
    {
        msgData.protocolConfigurationOptionsIePresent = true;
        msgData.protocolConfigurationOptions.pcoValue.count = csr_Q_msg_m->pco_length;
        memcpy(&msgData.protocolConfigurationOptions.pcoValue.values[0], &csr_Q_msg_m->pco_options[0], csr_Q_msg_m->pco_length);
    }

    gtpTransData *transData = new gtpTransData(csr_Q_msg_m->ue_idx);
    gtpTrans trans(le.local_addr.sin_addr.s_addr, le.local_addr.sin_port, gtpHeader.sequenceNumber);
    gtpTables::Instance()->addSeqKey(trans, transData); 

    GtpV2Stack_buildGtpV2Message(gtpStack_gp, csReqMsgBuf_p, &gtpHeader, &msgData);

    int res = sendto (
            le.s11_fd,
            csReqMsgBuf_p->getDataPointer(),
            csReqMsgBuf_p->getLength(), 0,
            (struct sockaddr*)(&sgw_addr),
            sizeof(struct sockaddr_in));
    if (res < 0) {
        log_msg(LOG_ERROR,"Error in sendto in detach stage 3 post to next\n");
    }

    log_msg(LOG_DEBUG,"%d CSReq message sent Bytes sent. Err : %d, %s\n",res,errno, strerror(errno));

	//MsgBuffer_free(csReqMsgBuf_p);
    
    transData->key = trans;
    transData->buf = csReqMsgBuf_p;
    transData->sgw_addr = sgw_addr;
    transData->fd = le.s11_fd;
    s11TimerUtils::startTimer(s11_cfg->retransmission_interval, 1, transTimer_c, transData);

    return;
}

void 
S11MsgHandler::handleModifyBearerRequestMsg_v(IpcEventMessage* eMsg)
{
    log_msg(LOG_INFO, "%s\n",__FUNCTION__);

	utils::MsgBuffer*  mbReqMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
    if(mbReqMsgBuf_p == NULL)
    {
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
        return ;
    }

    utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
        return; // error 

    gtp_outgoing_msg_data_t* msgData_p = (gtp_outgoing_msg_data_t*)(msgBuf->getDataPointer());

    struct MB_Q_msg *mb_msg = &msgData_p->msg_data.mbr_req_msg;

    GtpV2MessageHeader gtpHeader;
    gtpHeader.msgType = GTP_MODIFY_BEARER_REQ;
    gtpHeader.sequenceNumber = g_s11_sequence;
    gtpHeader.teidPresent = true;
    gtpHeader.teid = mb_msg->s11_sgw_c_fteid.header.teid_gre;

    g_s11_sequence++;

    ModifyBearerRequestMsgData msgData;
    memset(&msgData, 0, sizeof(msgData));
    struct TAI *tai = &(mb_msg->tai);
    struct CGI *cgi = &(mb_msg->utran_cgi);

    if (mb_msg->servingNetworkIePresent)
    {
        msgData.servingNetworkIePresent = true;
        msgData.servingNetwork.mccDigit1 = tai->plmn_id.idx[0] & 0x0F;
        msgData.servingNetwork.mccDigit2 = (tai->plmn_id.idx[0] & 0xF0) >> 4;
        msgData.servingNetwork.mccDigit3 = tai->plmn_id.idx[1] & 0x0F;
        msgData.servingNetwork.mncDigit1 = tai->plmn_id.idx[2] & 0x0F;
        msgData.servingNetwork.mncDigit2 = (tai->plmn_id.idx[2] & 0xF0) >> 4;
        msgData.servingNetwork.mncDigit3 = (tai->plmn_id.idx[1] & 0xF0) >> 4;
    }

    if (mb_msg->userLocationInformationIePresent)
    {
        msgData.userLocationInformationIePresent = true;
        msgData.userLocationInformation.taipresent = true;
        msgData.userLocationInformation.ecgipresent = true;

        msgData.userLocationInformation.tai.trackingAreaCode = ntohs(tai->tac);
        msgData.userLocationInformation.tai.mccDigit1 = tai->plmn_id.idx[0]
            & 0x0F;
        msgData.userLocationInformation.tai.mccDigit2 = (tai->plmn_id.idx[0]
                & 0xF0) >> 4;
        msgData.userLocationInformation.tai.mccDigit3 = tai->plmn_id.idx[1]
            & 0x0F;
        msgData.userLocationInformation.tai.mncDigit1 = tai->plmn_id.idx[2]
            & 0x0F;
        msgData.userLocationInformation.tai.mncDigit2 = (tai->plmn_id.idx[2]
                & 0xF0) >> 4;
        msgData.userLocationInformation.tai.mncDigit3 = (tai->plmn_id.idx[1]
                & 0xF0) >> 4;

        msgData.userLocationInformation.ecgi.eUtranCellId = ntohl(cgi->cell_id)
            >> 4;
        msgData.userLocationInformation.ecgi.mccDigit1 = cgi->plmn_id.idx[0]
            & 0x0F;
        msgData.userLocationInformation.ecgi.mccDigit2 = (cgi->plmn_id.idx[0]
                & 0xF0) >> 4;
        msgData.userLocationInformation.ecgi.mccDigit3 = cgi->plmn_id.idx[1]
            & 0x0F;
        msgData.userLocationInformation.ecgi.mncDigit1 = cgi->plmn_id.idx[2]
            & 0x0F;
        msgData.userLocationInformation.ecgi.mncDigit2 = (cgi->plmn_id.idx[2]
                & 0xF0) >> 4;
        msgData.userLocationInformation.ecgi.mncDigit3 = (cgi->plmn_id.idx[1]
                & 0xF0) >> 4;
    }

    //TODO:Support dedicated bearer
    msgData.bearerContextsToBeModifiedCount = 1;
    msgData.bearerContextsToBeModified[0].epsBearerId.epsBearerId = 5;
    msgData.bearerContextsToBeModified[0].s1EnodebFTeidIePresent = true;
    msgData.bearerContextsToBeModified[0].s1EnodebFTeid.ipv4present = true;
    msgData.bearerContextsToBeModified[0].s1EnodebFTeid.interfaceType = mb_msg->s1u_enb_fteid.header.iface_type;
    msgData.bearerContextsToBeModified[0].s1EnodebFTeid.teidGreKey = mb_msg->s1u_enb_fteid.header.teid_gre;
    msgData.bearerContextsToBeModified[0].s1EnodebFTeid.ipV4Address.ipValue = mb_msg->s1u_enb_fteid.ip.ipv4.s_addr;

    struct sockaddr_in sgw_addr = {0};
    sgw_addr.sin_family = AF_INET;
    sgw_addr.sin_port = htons(s11_cfg->egtp_def_port);
    sgw_addr.sin_addr.s_addr = htonl(mb_msg->s11_sgw_c_fteid.ip.ipv4.s_addr); 

    gtpTransData *transData = new gtpTransData(mb_msg->ue_idx);
    gtpTrans trans(le.local_addr.sin_addr.s_addr, le.local_addr.sin_port, gtpHeader.sequenceNumber);
    gtpTables::Instance()->addSeqKey(trans, transData); 

    GtpV2Stack_buildGtpV2Message(gtpStack_gp, mbReqMsgBuf_p, &gtpHeader, &msgData);
    sendto(le.s11_fd,
            mbReqMsgBuf_p->getDataPointer(),
            mbReqMsgBuf_p->getLength(), 0,
            (struct sockaddr*)(&sgw_addr),
            sizeof(struct sockaddr_in));
    //TODO " error chk, eagain etc?	
    log_msg(LOG_INFO, "Modify bearer sent, len - %d bytes.\n", mbReqMsgBuf_p->getLength());

    transData->key = trans;
    transData->buf = mbReqMsgBuf_p;
    transData->sgw_addr = sgw_addr;
    transData->fd = le.s11_fd;
    s11TimerUtils::startTimer(s11_cfg->retransmission_interval, 1, transTimer_c, transData);

    return;
}


void 
S11MsgHandler::handleDeleteSessionRequestMsg_v(IpcEventMessage* eMsg)
{
	utils::MsgBuffer*  dsReqMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
 	if(dsReqMsgBuf_p == NULL)
 	{
 	    log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
             return ;
 	}
    log_msg(LOG_INFO, "%s\n",__FUNCTION__);
    utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
        return; // error 

    gtp_outgoing_msg_data_t* msgData_p = (gtp_outgoing_msg_data_t*)(msgBuf->getDataPointer());

    struct DS_Q_msg *ds_msg = &msgData_p->msg_data.dsr_req_msg;

    GtpV2MessageHeader gtpHeader;
    gtpHeader.msgType = GTP_DELETE_SESSION_REQ;
    gtpHeader.sequenceNumber = g_s11_sequence;
    gtpHeader.teidPresent = true;
    gtpHeader.teid = ds_msg->s11_sgw_c_fteid.header.teid_gre;

    DeleteSessionRequestMsgData msgData;
    memset(&msgData, 0, sizeof(DeleteSessionRequestMsgData));

    msgData.indicationFlagsIePresent = true;
    msgData.indicationFlags.iOI = true;

    msgData.linkedEpsBearerIdIePresent = true;
    msgData.linkedEpsBearerId.epsBearerId = ds_msg->bearer_id;

    gtpTransData *transData = new gtpTransData(ds_msg->ue_idx);
    gtpTrans trans(le.local_addr.sin_addr.s_addr, le.local_addr.sin_port, gtpHeader.sequenceNumber);
    gtpTables::Instance()->addSeqKey(trans, transData);

    GtpV2Stack_buildGtpV2Message(gtpStack_gp, dsReqMsgBuf_p, &gtpHeader, &msgData);
    g_s11_sequence++;

    struct sockaddr_in sgw_addr = {0};
    sgw_addr.sin_family = AF_INET;
    sgw_addr.sin_port = htons(s11_cfg->egtp_def_port);
    sgw_addr.sin_addr.s_addr = htonl(ds_msg->s11_sgw_c_fteid.ip.ipv4.s_addr); 

    sendto(le.s11_fd,
            dsReqMsgBuf_p->getDataPointer(),
            dsReqMsgBuf_p->getLength(), 0,
            (struct sockaddr*)(&sgw_addr), sizeof(struct sockaddr_in));
    log_msg(LOG_INFO, "Send delete session request\n");

    transData->key = trans;
    transData->buf = dsReqMsgBuf_p;
    transData->sgw_addr = sgw_addr;
    transData->fd = le.s11_fd;
    s11TimerUtils::startTimer(s11_cfg->retransmission_interval, 1, transTimer_c, transData);

    return;
}


void 
S11MsgHandler::handleReleaseAccessBearerRequestMsg_v(IpcEventMessage* eMsg)
{
    log_msg(LOG_INFO, "%s\n",__FUNCTION__);
    utils::MsgBuffer*  rbReqMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
    if(rbReqMsgBuf_p == NULL)
    {
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
        return ;
    }

    utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
        return; // error 

    gtp_outgoing_msg_data_t* msgData_p = (gtp_outgoing_msg_data_t*)(msgBuf->getDataPointer());

    struct RB_Q_msg *rb_msg = &msgData_p->msg_data.rabr_req_msg;
    GtpV2MessageHeader gtpHeader;	
    gtpHeader.msgType = GTP_RABR_REQ;
    gtpHeader.sequenceNumber = g_s11_sequence;
    gtpHeader.teidPresent = true;
    gtpHeader.teid = rb_msg->s11_sgw_c_fteid.header.teid_gre;

    g_s11_sequence++;

    ReleaseAccessBearersRequestMsgData msgData;
    memset(&msgData, 0, sizeof(msgData));

    msgData.indicationFlagsIePresent = true;
    msgData.indicationFlags.iOI = true;

    gtpTransData *transData = new gtpTransData(rb_msg->ue_idx);
    gtpTrans trans(le.local_addr.sin_addr.s_addr, le.local_addr.sin_port, gtpHeader.sequenceNumber);
    gtpTables::Instance()->addSeqKey(trans, transData); 
    GtpV2Stack_buildGtpV2Message(gtpStack_gp, rbReqMsgBuf_p, &gtpHeader, &msgData);

    struct sockaddr_in sgw_addr = {0};
    sgw_addr.sin_family = AF_INET;
    sgw_addr.sin_port = htons(s11_cfg->egtp_def_port);
    sgw_addr.sin_addr.s_addr = htonl(rb_msg->s11_sgw_c_fteid.ip.ipv4.s_addr); 


    sendto(le.s11_fd,
            rbReqMsgBuf_p->getDataPointer(),
            rbReqMsgBuf_p->getLength(), 0,
            (struct sockaddr*)(&sgw_addr),
            sizeof(struct sockaddr_in));
    //TODO " error chk, eagain etc?
    log_msg(LOG_INFO, "Release Bearer sent, len - %d bytes.\n", rbReqMsgBuf_p->getLength());

    transData->key = trans;
    transData->buf = rbReqMsgBuf_p;
    transData->sgw_addr = sgw_addr;
    transData->fd = le.s11_fd;
    s11TimerUtils::startTimer(s11_cfg->retransmission_interval, 1, transTimer_c, transData);

    return;
}


void 
S11MsgHandler::handleDownlinkDataNotificationAckMsg_v(IpcEventMessage* eMsg)
{
    utils::MsgBuffer* ddnAckMsgBuf_p = createMsgBuffer(S11_MSGBUF_SIZE);
    if(ddnAckMsgBuf_p == NULL)
    {
        log_msg(LOG_ERROR, "Error in initializing msg buffers required by gtp codec.\n");
        return ;
    }
    log_msg(LOG_INFO, "%s\n",__FUNCTION__);
    utils::MsgBuffer* msgBuf = eMsg->getMsgBuffer();
    if (msgBuf == NULL)
        return; // error 

    gtp_outgoing_msg_data_t* msgData_p = (gtp_outgoing_msg_data_t*)(msgBuf->getDataPointer());

    struct DDN_ACK_Q_msg *ddn_ack_msg = &msgData_p->msg_data.ddn_ack_msg;
    GtpV2MessageHeader gtpHeader;
    gtpHeader.msgType =  GTP_DOWNLINK_DATA_NOTIFICATION_ACK;
    gtpHeader.sequenceNumber = ddn_ack_msg->seq_no;
    gtpHeader.teidPresent = true;
	gtpHeader.teid = ddn_ack_msg->s11_sgw_c_fteid.header.teid_gre;

    gtpTrans trans(ddn_ack_msg->s11_sgw_c_fteid.ip.ipv4.s_addr, le.local_addr.sin_port, gtpHeader.sequenceNumber);
    gtpTransData *t1 = gtpTables::Instance()->delSeqKey(trans);
    if(t1 == nullptr)
    {
        log_msg(LOG_DEBUG, "Transaction not found while sending DDN Ack \n");
        // for now not dropping ack message 
        // return -1;
    }
    delete t1;

    DownlinkDataNotificationAcknowledgeMsgData msgData;
    memset(&msgData, 0, sizeof(DownlinkDataNotificationAcknowledgeMsgData));

    msgData.cause.causeValue = ddn_ack_msg->cause;


    GtpV2Stack_buildGtpV2Message(gtpStack_gp, ddnAckMsgBuf_p, &gtpHeader, &msgData);

    struct sockaddr_in sgw_addr = {0};
    sgw_addr.sin_family = AF_INET;
    sgw_addr.sin_port = htons(s11_cfg->egtp_def_port);
    sgw_addr.sin_addr.s_addr = htonl(ddn_ack_msg->s11_sgw_c_fteid.ip.ipv4.s_addr); 

    sendto(le.s11_fd,
            ddnAckMsgBuf_p->getDataPointer(),
            ddnAckMsgBuf_p->getLength(), 0,
            (struct sockaddr*)(&sgw_addr), sizeof(struct sockaddr_in));

    log_msg(LOG_INFO, "DDN Ack Sent, len - %d bytes.\n", ddnAckMsgBuf_p->getLength());
	MsgBuffer_free(ddnAckMsgBuf_p);

    return ;
}

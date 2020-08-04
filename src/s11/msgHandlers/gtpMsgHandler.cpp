/*
 * Copyright 2020-present Open Networking Foundation
 * Copyright 2020-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */


#include "msgHandlers/gtpIncomingMsgHandler.h"
#include "err_codes.h"
#include "msgType.h"
#include "tipcTypes.h"
#include "log.h"
#include "s11.h"
#include "gtpV2Stack.h"
#include <gtpV2StackWrappers.h>
#include <interfaces/s11IpcInterface.h>
#include "gtp_tables.h"

using namespace cmn;
using namespace cmn::utils;


extern struct GtpV2Stack* gtpStack_gp;

void 
gtpIncomingMsgHandler::handle_s11_message(MsgBuffer *msgBuf_p) 
{
    log_msg(LOG_INFO, "S11 recv msg handler.\n");

    GtpV2MessageHeader msgHeader;

    bool rc = GtpV2Stack_decodeMessageHeader(gtpStack_gp, &msgHeader, msgBuf_p);

    if(rc == false)
        log_msg(LOG_ERROR,"Failed to decode the GTP Message\n");

    switch(msgHeader.msgType){
        case S11_GTP_CREATE_SESSION_RESP:
            s11_CS_resp_handler(msgBuf_p, &msgHeader);
            break;

        case S11_GTP_MODIFY_BEARER_RESP:
            s11_MB_resp_handler(msgBuf_p, &msgHeader);
            break;

        case S11_GTP_DELETE_SESSION_RESP:
            s11_DS_resp_handler(msgBuf_p, &msgHeader);
            break;

        case S11_GTP_REL_ACCESS_BEARER_RESP:
            s11_RB_resp_handler(msgBuf_p, &msgHeader);
            break;

        case S11_GTP_DOWNLINK_DATA_NOTIFICATION:
            s11_DDN_handler(msgBuf_p, &msgHeader);
            break;

    }

    MsgBuffer_free(msgBuf_p);

    return;
}

int
gtpIncomingMsgHandler::s11_CS_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr)
{

    gtp_incoming_msg_data_t csr_info;
    /*****Message structure***
     */

    /*Check whether has teid flag is set. Also check whether this check is needed for CSR.*/
    csr_info.ue_idx = hdr->teid;
    csr_info.msg_type = create_session_response;
    gtpTables::Instance()->delSeqKey(hdr->sequenceNumber);

    CreateSessionResponseMsgData msgData;
    memset(&msgData, 0, sizeof(CreateSessionResponseMsgData));

    bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
    if(rc == false)
    {
        log_msg(LOG_ERROR, "s11_CS_resp_handler: "
                "Failed to decode Create Session Response Msg %d\n",
                hdr->teid);
        return E_PARSING_FAILED;
    }
    csr_info.msg_data.csr_Q_msg_m.status = msgData.cause.causeValue;
    csr_info.msg_data.csr_Q_msg_m.s11_sgw_fteid.header.iface_type = 11;
    csr_info.msg_data.csr_Q_msg_m.s11_sgw_fteid.header.teid_gre = msgData.senderFTeidForControlPlane.teidGreKey;
    csr_info.msg_data.csr_Q_msg_m.s11_sgw_fteid.header.v4 = 1;
    csr_info.msg_data.csr_Q_msg_m.s11_sgw_fteid.ip.ipv4.s_addr = msgData.senderFTeidForControlPlane.ipV4Address.ipValue;

    csr_info.msg_data.csr_Q_msg_m.s5s8_pgwc_fteid.header.iface_type = 7;
    csr_info.msg_data.csr_Q_msg_m.s5s8_pgwc_fteid.header.teid_gre = msgData.pgwS5S8S2bFTeid.teidGreKey;
    csr_info.msg_data.csr_Q_msg_m.s5s8_pgwc_fteid.header.v4 = 1;
    csr_info.msg_data.csr_Q_msg_m.s5s8_pgwc_fteid.ip.ipv4.s_addr = msgData.pgwS5S8S2bFTeid.ipV4Address.ipValue;

    csr_info.msg_data.csr_Q_msg_m.s1u_sgw_fteid.header.iface_type = 1;
    csr_info.msg_data.csr_Q_msg_m.s1u_sgw_fteid.header.teid_gre = msgData.bearerContextsCreated[0].s1USgwFTeid.teidGreKey;
    csr_info.msg_data.csr_Q_msg_m.s1u_sgw_fteid.header.v4 = 1;
    csr_info.msg_data.csr_Q_msg_m.s1u_sgw_fteid.ip.ipv4.s_addr = msgData.bearerContextsCreated[0].s1USgwFTeid.ipV4Address.ipValue;

    csr_info.msg_data.csr_Q_msg_m.s5s8_pgwu_fteid.header.iface_type = 5;
    csr_info.msg_data.csr_Q_msg_m.s5s8_pgwu_fteid.header.teid_gre = msgData.bearerContextsCreated[0].s5S8UPgwFTeid.teidGreKey;
    csr_info.msg_data.csr_Q_msg_m.s5s8_pgwu_fteid.header.v4 = 1;
    csr_info.msg_data.csr_Q_msg_m.s5s8_pgwu_fteid.ip.ipv4.s_addr = msgData.bearerContextsCreated[0].s5S8UPgwFTeid.ipV4Address.ipValue;

    csr_info.msg_data.csr_Q_msg_m.pdn_addr.pdn_type = 1;
    csr_info.msg_data.csr_Q_msg_m.pdn_addr.ip_type.ipv4.s_addr = msgData.pdnAddressAllocation.ipV4Address.ipValue;


    csr_info.msg_data.csr_Q_msg_m.pco_length = 0; 
    if(msgData.protocolConfigurationOptionsIePresent == true)
    {
        csr_info.msg_data.csr_Q_msg_m.pco_length = msgData.protocolConfigurationOptions.pcoValue.count; 
        memcpy(&csr_info.msg_data.csr_Q_msg_m.pco_options[0], &msgData.protocolConfigurationOptions.pcoValue.values[0], msgData.protocolConfigurationOptions.pcoValue.count);
    }

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::mmeAppInstanceNum_c;
    s11IpcInterface &mmeIpcIf = static_cast<s11IpcInterface&>(compDb.getComponent(S11IpcInterfaceCompId));        
    mmeIpcIf.dispatchIpcMsg(&csr_info, sizeof(csr_info), destAddr);

    log_msg(LOG_INFO, "Send CS resp to mme-app stage6 message type %d .\n", csr_info.msg_type);

    return SUCCESS;
}

int
gtpIncomingMsgHandler::s11_MB_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr)
{

    gtp_incoming_msg_data_t mbr_info;

    /*****Message structure***
     */
    log_msg(LOG_INFO, "Parse S11 MB resp message\n");

    //TODO : check cause foor the result verification
    /*Check whether has teid flag is set. Also check whether this check is needed for CSR.*/
    if(hdr->teid)
    {
        mbr_info.ue_idx = hdr->teid;
    }
    else
    {
        log_msg(LOG_WARNING, "Unknown Teid in MBR.\n");
        mbr_info.ue_idx = gtpTables::Instance()->findUeIdxWithSeq(hdr->sequenceNumber);
    }

    gtpTables::Instance()->delSeqKey(hdr->sequenceNumber);

    mbr_info.msg_type = modify_bearer_response;

    ModifyBearerResponseMsgData msgData;
    memset(&msgData, 0, sizeof(ModifyBearerResponseMsgData));

    bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
    if (rc == false)
    {
        log_msg(LOG_ERROR, "s11_MB_resp_handler: "
                "Failed to decode MB_resp Msg %d\n", hdr->teid);
        return E_PARSING_FAILED;
    }
    mbr_info.msg_data.MB_resp_Q_msg_m.cause = msgData.cause.causeValue;

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::mmeAppInstanceNum_c;
    s11IpcInterface &mmeIpcIf = static_cast<s11IpcInterface&>(compDb.getComponent(S11IpcInterfaceCompId));        
    mmeIpcIf.dispatchIpcMsg(&mbr_info, sizeof(mbr_info), destAddr);
    log_msg(LOG_INFO, "Send MB resp to mme-app stage8.\n");

    return SUCCESS;
}

int
gtpIncomingMsgHandler::s11_DS_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr)
{
    gtp_incoming_msg_data_t dsr_info;
    dsr_info.msg_type = delete_session_response;

    /*****Message structure****/
    log_msg(LOG_INFO, "Parse S11 DS resp message\n");

    //TODO : check cause for the result verification

    /*Check whether has teid flag is set.
     * Also check whether this check is needed for DSR.
     * */
    if(hdr->teid)
    {
        dsr_info.ue_idx = hdr->teid;
    }
    else
    {
        log_msg(LOG_WARNING, "Unknown Teid in DSR.\n");
        dsr_info.ue_idx = gtpTables::Instance()->findUeIdxWithSeq(hdr->sequenceNumber);
    }

    gtpTables::Instance()->delSeqKey(hdr->sequenceNumber);

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::mmeAppInstanceNum_c;
    s11IpcInterface &mmeIpcIf = static_cast<s11IpcInterface&>(compDb.getComponent(S11IpcInterfaceCompId));        
    mmeIpcIf.dispatchIpcMsg(&dsr_info, sizeof(dsr_info), destAddr);
    log_msg(LOG_INFO, "Send DS resp to mme-app stage8.\n");

    return SUCCESS;
}

int
gtpIncomingMsgHandler::s11_RB_resp_handler(MsgBuffer* message, GtpV2MessageHeader* hdr)
{	
    gtp_incoming_msg_data_t rbr_info;

    /*****Message structure***
     */
    log_msg(LOG_INFO, "Parse S11 RB resp message\n");
    if(hdr->teid)
    {
        rbr_info.ue_idx = hdr->teid;
    }
    else
    {
        log_msg(LOG_WARNING, "Unknown Teid in RABR.\n");
        rbr_info.ue_idx =  gtpTables::Instance()->findUeIdxWithSeq(hdr->sequenceNumber);
    }

    gtpTables::Instance()->delSeqKey(hdr->sequenceNumber);

    //TODO : check cause for the result verification
    rbr_info.msg_type = release_bearer_response;

    ReleaseAccessBearersResponseMsgData msgData;
    memset(&msgData, 0, sizeof(ReleaseAccessBearersResponseMsgData));

    bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
    if(rc == false)
    {
        log_msg(LOG_ERROR, "s11_RB_resp_handler: "
                "Failed to decode Release Access Bearer Response Msg %d\n",
                hdr->teid);
        return E_PARSING_FAILED;
    }

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::mmeAppInstanceNum_c;
    s11IpcInterface &mmeIpcIf = static_cast<s11IpcInterface&>(compDb.getComponent(S11IpcInterfaceCompId));        
    mmeIpcIf.dispatchIpcMsg(&rbr_info, sizeof(rbr_info), destAddr);

    log_msg(LOG_INFO, "Send RB resp to mme-app stage2.\n");

    return SUCCESS;
}

int
gtpIncomingMsgHandler::s11_DDN_handler(MsgBuffer* message, GtpV2MessageHeader* hdr)
{
    gtp_incoming_msg_data_t ddn_info;
    ddn_info.msg_type = downlink_data_notification;
    ddn_info.ue_idx = hdr->teid;
    ddn_info.msg_data.ddn_Q_msg_m.seq_no = hdr->sequenceNumber;


    DownlinkDataNotificationMsgData msgData;
    memset(&msgData, 0, sizeof(DownlinkDataNotificationMsgData));

    bool rc = GtpV2Stack_decodeMessage(gtpStack_gp, hdr, message, &msgData);
    if(rc == false)
    {

        log_msg(LOG_ERROR, "s11_Ddn_handler: "
                "Failed to decode ddn Msg %d\n",
                hdr->teid);
        return E_PARSING_FAILED;
    }
    /*****Message structure****/
    log_msg(LOG_INFO, "Parse S11 Ddn message\n");

    //TODO : check cause for the result verification
    if (msgData.allocationRetentionPriorityIePresent) {
        ddn_info.msg_data.ddn_Q_msg_m.arp.prioLevel = msgData.allocationRetentionPriority.pl;
        ddn_info.msg_data.ddn_Q_msg_m.arp.preEmptionCapab = msgData.allocationRetentionPriority.pci;
        ddn_info.msg_data.ddn_Q_msg_m.arp.preEmptionVulnebility = msgData.allocationRetentionPriority.pvi;
    }
    if (msgData.causeIePresent)
        ddn_info.msg_data.ddn_Q_msg_m.cause = msgData.cause.causeValue;
    if (msgData.epsBearerIdIePresent)
        ddn_info.msg_data.ddn_Q_msg_m.eps_bearer_id = msgData.epsBearerId.epsBearerId;

    cmn::ipc::IpcAddress destAddr;
    destAddr.u32 = TipcServiceInstance::mmeAppInstanceNum_c;
    s11IpcInterface &mmeIpcIf = static_cast<s11IpcInterface&>(compDb.getComponent(S11IpcInterfaceCompId));        
    mmeIpcIf.dispatchIpcMsg(&ddn_info, sizeof(ddn_info), destAddr);
    log_msg(LOG_INFO, "Send ddn to mme-app\n");

    return SUCCESS;
}

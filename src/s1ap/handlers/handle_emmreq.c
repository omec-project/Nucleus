/*
* Copyright 2019-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "err_codes.h"
#include "s1ap.h"
#include "ipc_api.h"
#include "main.h"
#include "msgType.h"
#include "sctp_conn.h"
#include "assert.h"

extern ipc_handle ipc_S1ap_Hndl;

int 
encode_network_name_ie(char* network_name, char* enc_str)
{
  uint32_t          i;
  uint32_t          bit_offset;
  uint32_t          byte_offset;
  uint32_t          name_length = strlen(network_name);

  // We put limit on the number if characters in the network name.
  assert(name_length <= 10);

  bit_offset  = 0;
  byte_offset = 2; // Min length; length  1 byte and 1 byte mandatory header field. 

  unsigned char mask_1[8] = {0xff ,0xff, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0,  0x80};
  unsigned char mask_2[8] = {0xff, 0xff, 0x01, 0x03, 0x07, 0x0f, 0x1f,  0x3f};

  for (i = 0; i < name_length; i++) {
    // check if printable character. Except some special charaters are not allowed    
    if (network_name[i] == 0x0A || network_name[i] == 0x0D || 
       (network_name[i] >= 0x20 && network_name[i] <= 0x3F) ||
       (network_name[i] >= 0x41 && network_name[i] <= 0x5A) || 
       (network_name[i] >= 0x61 && network_name[i] <= 0x7A)) {
      enc_str[byte_offset] |= ((network_name[i] << bit_offset) & mask_1[bit_offset]);
      if(bit_offset >=1) {
        byte_offset ++;
        enc_str[byte_offset] |= ((network_name[i] >>(8-bit_offset)) & mask_2[bit_offset]);
      }
      bit_offset = (bit_offset+7) % 8; 
    }
    else {
      return E_FAIL; //unsupported charater 
    }
  }
  if (0 == bit_offset) {
    enc_str[0] = byte_offset - 1;
    enc_str[1] = 0x80 ; // ci not supported 
  } else {
    enc_str[0] = byte_offset;
    enc_str[1] = 0x80 | ((8 - bit_offset) & 0x07);
  }
  return SUCCESS;
}

static int
emm_info_request_processing(struct ue_emm_info *g_ueEmmInfoMsg)
{
	
    Buffer g_buffer = {0};
    Buffer g_s1ap_buffer = {0};

    struct s1ap_PDU s1apPDU = {0};
    
    /* Assigning values to s1apPDU */
    s1apPDU.procedurecode = id_downlinkNASTransport;
    s1apPDU.criticality = CRITICALITY_IGNORE;
    
    /* Copy values to buffer from s1apPDU */
    g_buffer.pos = 0;
    
    uint8_t initiating_message = 0; 
    buffer_copy(&g_buffer, &initiating_message,
		    sizeof(initiating_message));
    
    buffer_copy(&g_buffer, &s1apPDU.procedurecode,
		    sizeof(s1apPDU.procedurecode));
    
    buffer_copy(&g_buffer, &s1apPDU.criticality,
		    sizeof(s1apPDU.criticality));

    g_s1ap_buffer.pos = 0; 
    
    uint8_t chProtoIENo[3] = {0,0,3};
    buffer_copy(&g_s1ap_buffer, chProtoIENo, 3);
    
    /* id-MME-UE-S1AP-ID */
    unsigned char tmpStr[4];
    uint16_t protocolIe_Id = id_MME_UE_S1AP_ID;
    copyU16(tmpStr, protocolIe_Id);
    buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
    uint8_t protocolIe_criticality = CRITICALITY_REJECT;
    buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
		    sizeof(protocolIe_criticality));
    
    uint8_t datalen = 2;
    /* TODO need to add proper handling*/
    unsigned char mme_ue_id[3];
    datalen = copyU16(mme_ue_id, g_ueEmmInfoMsg->mme_s1ap_ue_id);
    buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));
    buffer_copy(&g_s1ap_buffer, mme_ue_id, datalen);
    
    /* id-eNB-UE-S1AP-ID */
    protocolIe_Id = id_eNB_UE_S1AP_ID;
    copyU16(tmpStr, protocolIe_Id);
    buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
    buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
		    sizeof(protocolIe_criticality));
    
    unsigned char enb_ue_id[3];
    datalen = copyU16(enb_ue_id, g_ueEmmInfoMsg->enb_s1ap_ue_id);
    buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));
    buffer_copy(&g_s1ap_buffer, enb_ue_id, datalen);

    /* Lets add NAS AVP header */
    protocolIe_Id = id_NAS_PDU;
    copyU16(tmpStr, protocolIe_Id);
    buffer_copy(&g_s1ap_buffer, tmpStr, sizeof(protocolIe_Id));
    buffer_copy(&g_s1ap_buffer, &protocolIe_criticality,
		    sizeof(protocolIe_criticality));


#ifdef S1AP_ENCODE_NAS
    Buffer g_nas_buffer = {0};
    g_nas_buffer.pos = 0; 

    unsigned char nas_sec_hdr[1] = { 0x27}; 
    buffer_copy(&g_nas_buffer, nas_sec_hdr, 1);
	
    uint8_t mac_data_pos;
    char mac[4] = { 0x00, 0x00, 0x00, 0x00}; 
    buffer_copy(&g_nas_buffer, mac, 4);
    mac_data_pos = g_nas_buffer.pos;

    unsigned char seq_no = g_ueEmmInfoMsg->dl_seq_no;
    buffer_copy(&g_nas_buffer, &seq_no, sizeof(seq_no));
   
    char nas_plain_hdr[2] = { 0x07, 0x61}; 
    buffer_copy(&g_nas_buffer, nas_plain_hdr, 2);

    char bufBig[128] = {'\0'};
    bufBig[0] = 0x43;
    encode_network_name_ie(g_ueEmmInfoMsg->full_network_name, &bufBig[1]);
    buffer_copy(&g_nas_buffer, bufBig, bufBig[1] + 2);

    char bufShort[20] = {'\0'};
    bufShort[0] = 0x45;
    encode_network_name_ie(g_ueEmmInfoMsg->short_network_name, &bufShort[1]);
    buffer_copy(&g_nas_buffer, bufShort, bufShort[1] + 2);
    
    /* Calculate mac */
    uint8_t direction = 1;
    uint8_t bearer = 0;
    
    calculate_aes_mac(g_ueEmmInfoMsg->int_key, g_ueEmmInfoMsg->dl_count, direction,
		    bearer, &g_nas_buffer.buf[mac_data_pos],
		    g_nas_buffer.pos - mac_data_pos,
		    &g_nas_buffer.buf[mac_data_pos - MAC_SIZE]);

    uint8_t naslen = g_nas_buffer.pos+1;
    buffer_copy(&g_s1ap_buffer, &naslen, 1);

    naslen = g_nas_buffer.pos;
    buffer_copy(&g_s1ap_buffer, &naslen, 1);

    /* Now lets add NAS buffer to s1ap buffer */
    buffer_copy(&g_s1ap_buffer, &g_nas_buffer.buf[0], (g_nas_buffer.pos));
#else
	log_msg(LOG_INFO, "Received EMM information request has nas message %d \n",g_ueEmmInfoMsg->nasMsgSize);
	datalen = g_ueEmmInfoMsg->nasMsgSize + 1; 
	buffer_copy(&g_s1ap_buffer, &datalen, sizeof(datalen));
	buffer_copy(&g_s1ap_buffer, &g_ueEmmInfoMsg->nasMsgSize, sizeof(uint8_t));
	buffer_copy(&g_s1ap_buffer, &g_ueEmmInfoMsg->nasMsgBuf[0], g_ueEmmInfoMsg->nasMsgSize);

#endif

    /* adding length of s1ap message before adding s1ap message */
	uint8_t s1applen = g_s1ap_buffer.pos;
	buffer_copy(&g_buffer, &s1applen, sizeof(s1applen));

    buffer_copy(&g_buffer, &g_s1ap_buffer.buf[0], g_s1ap_buffer.pos);
    /* Now s1ap header + s1ap buffer is attached */
    
    log_msg(LOG_INFO, "EMM info request sent on fd = %u , enb-id %u mmeid %u EMM \n",g_ueEmmInfoMsg->enb_fd, 
		    g_ueEmmInfoMsg->enb_s1ap_ue_id, g_ueEmmInfoMsg->mme_s1ap_ue_id);

    log_msg(LOG_INFO, "EMM info request : Buffer size %d g_ueEmmInfoMsg = %p \n", g_buffer.pos,g_ueEmmInfoMsg);
    int res = send_sctp_msg(g_ueEmmInfoMsg->enb_fd, g_buffer.buf, g_buffer.pos, 1);
    
    log_msg(LOG_INFO, "EMM Info Message sent to UE %d\n", res);
    
    return SUCCESS;
}

void*
emm_info_req_handler(void *data)
{
    log_msg(LOG_INFO, "emm_info_req_handler ready.\n");
    
    emm_info_request_processing((struct ue_emm_info *) data);
    
    return NULL;
}





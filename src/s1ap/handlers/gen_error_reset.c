/*
* Copyright 2019-present Open Networking Foundation
*
* SPDX-License-Identifier: Apache-2.0
*
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
#include "sctp_conn.h"
#include "s1ap_error.h"

/**
* Stage specific message processing.
*/
static int
reset_request_processing(struct ue_reset_info *g_errorResetS1apInfo)
{
    struct Buffer g_buffer;
    struct Buffer g_value_buffer;
    struct Buffer g_reset_buffer;
    struct Buffer g_ue_associations_buf;

	struct s1ap_PDU s1apPDU;

	/* Assigning values to s1apPDU */
	s1apPDU.procedurecode = id_reset;
	s1apPDU.criticality = CRITICALITY_REJECT;

	/* Copy values to buffer from s1apPDU */

	g_buffer.pos = 0;

	uint8_t initiating_message = 0; 
	buffer_copy(&g_buffer, &initiating_message,
			sizeof(initiating_message));

	buffer_copy(&g_buffer, &s1apPDU.procedurecode,
			sizeof(s1apPDU.procedurecode));

	buffer_copy(&g_buffer, &s1apPDU.criticality,
			sizeof(s1apPDU.criticality));

	/* Copy values in g_value_buffer */
	g_value_buffer.pos = 0;

	unsigned char chProtoIENo[3] = {0,0,2}; /*no extension and 2 IEs */

	buffer_copy(&g_value_buffer, chProtoIENo, sizeof(chProtoIENo));
    /* Add cause */
    {
        unsigned char tmpStr[4];
        uint16_t protocolIe_Id = id_Cause;
        copyU16(tmpStr, protocolIe_Id);
        buffer_copy(&g_value_buffer, tmpStr,
                        sizeof(protocolIe_Id));
        
        uint8_t protocolIe_criticality = CRITICALITY_IGNORE;
        buffer_copy(&g_value_buffer, &protocolIe_criticality,
                        sizeof(protocolIe_criticality));
        
        uint8_t cause_length = 2; 
        buffer_copy(&g_value_buffer, &cause_length, sizeof(cause_length));
        // refer - 36.413 Section 9.1.8.1 
        uint16_t  final = (g_errorResetS1apInfo->failure_layer << 12) | (g_errorResetS1apInfo->cause << 5); 
        uint8_t byte=(final >> 8) & 0xff ;
        buffer_copy(&g_value_buffer, &byte, sizeof(byte));
        byte=(final) & 0xff ;
        buffer_copy(&g_value_buffer, &byte, sizeof(byte));
    }
    /* Add specific tunnel info to be resetted */
    {
        unsigned char tmpStr[4];
        uint16_t protocolIe_Id = id_ResetType;
        copyU16(tmpStr, protocolIe_Id);
        buffer_copy(&g_value_buffer, tmpStr,
                        sizeof(protocolIe_Id));
        
        uint8_t protocolIe_criticality = CRITICALITY_REJECT;
        buffer_copy(&g_value_buffer, &protocolIe_criticality,
                        sizeof(protocolIe_criticality));

	    g_reset_buffer.pos = 0;
        uint8_t reset_type_buf = (1 << 6); 
        buffer_copy(&g_reset_buffer, &reset_type_buf, sizeof(reset_type_buf));
        uint8_t no_of_conns = 0; /* Actual tunnels - 1 */ 
        buffer_copy(&g_reset_buffer, &no_of_conns, sizeof(no_of_conns));
        {
    	  unsigned char tmpStr[4];
	      uint16_t protocolIe_Id = id_ueAssociatedLogicalS1Conn;
	      copyU16(tmpStr, protocolIe_Id);
	      buffer_copy(&g_reset_buffer, tmpStr, sizeof(protocolIe_Id));
	      uint8_t protocolIe_criticality = CRITICALITY_REJECT;
	      buffer_copy(&g_reset_buffer, &protocolIe_criticality, sizeof(protocolIe_criticality));
          g_ue_associations_buf.pos = 0;
          {
            uint8_t enb_mme; 
            uint16_t mmeid = g_errorResetS1apInfo->mme_s1ap_ue_id;
            if(mmeid <= 255) 
            {
              enb_mme = 0x60; // eNB & mme s1ap id present and length 1 byte 
              buffer_copy(&g_ue_associations_buf, &enb_mme, sizeof(enb_mme));
              uint8_t mme_id = mmeid;
              buffer_copy(&g_ue_associations_buf, &mme_id, sizeof(mme_id));
            } 
            else if (mmeid <= 65535)
            {
              enb_mme = 0x64; // eNB & mme s1ap id present and length 1 byte 
              buffer_copy(&g_ue_associations_buf, &enb_mme, sizeof(enb_mme));
              uint8_t mme_id1 = (mmeid >> 8) & 0xff;
              uint8_t mme_id2 = (mmeid) & 0xff;
              buffer_copy(&g_ue_associations_buf, &mme_id1, sizeof(mme_id1));
              buffer_copy(&g_ue_associations_buf, &mme_id2, sizeof(mme_id2));
            } 
          }
          {
            uint16_t enb_id = g_errorResetS1apInfo->enb_s1ap_ue_id;
            if(enb_id <= 255) 
            {
              uint8_t len=0;
              buffer_copy(&g_ue_associations_buf, &len, sizeof(len));
              uint8_t enbid = enb_id;
              buffer_copy(&g_ue_associations_buf, &enbid, sizeof(enbid));
            } 
            else if (enb_id <= 65535)
            {
              uint8_t len=0x40;
              buffer_copy(&g_ue_associations_buf, &len, sizeof(len));
              uint8_t enb_id1 = (enb_id >> 8) & 0xff;
              uint8_t enb_id2 = (enb_id) & 0xff;
              buffer_copy(&g_ue_associations_buf, &enb_id1, sizeof(enb_id1));
              buffer_copy(&g_ue_associations_buf, &enb_id2, sizeof(enb_id2));
            } 
          }        
          // we are done with g_ue_associations_buf. Now lets add that to g_reset_buf  
          // we are always including only 1 end points ..so we are well within 1 byte length 
          uint8_t inner_payload = g_ue_associations_buf.pos;
          buffer_copy(&g_reset_buffer, &inner_payload, sizeof(inner_payload));
          buffer_copy(&g_reset_buffer, &g_ue_associations_buf.buf[0], g_ue_associations_buf.pos);
       }
       uint8_t resettype_payload = g_reset_buffer.pos;
       buffer_copy(&g_value_buffer, &resettype_payload, sizeof(resettype_payload));
       buffer_copy(&g_value_buffer, &g_reset_buffer.buf[0], g_reset_buffer.pos);
    }
    /* Assumed to be 1 byte ?*/
    uint8_t top_payload = g_value_buffer.pos;
    buffer_copy(&g_buffer, &top_payload, sizeof(top_payload));
    buffer_copy(&g_buffer, &g_value_buffer.buf[0], g_value_buffer.pos);

    send_sctp_msg(g_errorResetS1apInfo->enb_fd, g_buffer.buf, g_buffer.pos, 1);

	return SUCCESS;
}

/**
* Thread function for stage.
*/
void*
gen_reset_request_handler(void *data)
{
    struct ue_reset_info *g_errorResetS1apInfo = (struct ue_reset_info *) data;

    reset_request_processing(g_errorResetS1apInfo);

	return NULL;
}


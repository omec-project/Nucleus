/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2019, Infosys Ltd.
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation 
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __HSS_MSG_H_
#define __HSS_MSG_H_

#include "sec.h"

/*Information needed for AIR*/
enum e_BUF_HDR {
	HSS_AIR_MSG,
	HSS_AIA_MSG,
	HSS_ULR_MSG,
	HSS_ULA_MSG,
	//NI Detach
	HSS_CLR_MSG,
	HSS_CLA_MSG,
	//NI Detach
	HSS_PURGE_MSG,
	HSS_PURGE_RESP_MSG,
};


#define IMSI_STR_LEN 16
/*AIR  - request. Struct is same for ULR*/
struct hss_air_msg {
	char imsi[IMSI_STR_LEN];
	unsigned char  plmn_id[3];
};

/*AIA  - response*/
struct hss_aia_msg {
	struct RAND rand;
	struct XRES xres;
	struct AUTN autn;
	struct KASME kasme;
};

#define HSS_MSISDN_LEN 10
#define SPARE_LEN 52

/*ULA  - response*/
struct hss_ula_msg {
	int subscription_state;
	unsigned char msisdn[HSS_MSISDN_LEN];
	unsigned char a_msisdn[HSS_MSISDN_LEN];
	unsigned char spare[SPARE_LEN];
};

struct hss_clr_msg {
	char origin_host;
	char origin_realm;
	char user_name;
	unsigned char cancellation_type;
};


struct hss_pur_msg {
	int ue_idx;
	char imsi[IMSI_STR_LEN];
};

struct hss_req_msg {
	enum e_BUF_HDR hdr;
	int ue_idx;
	union req_data {
		struct hss_air_msg air;
		struct hss_pur_msg pur;
	}data;
};

struct hss_resp_msg {
	enum e_BUF_HDR hdr;
	int ue_idx;
	int result;
	union resp_data {
		struct hss_aia_msg aia;
		struct hss_ula_msg ula;
		struct hss_clr_msg clr;
	}data;
};

#define HSS_RCV_BUF_SIZE 128
#define HSS_REQ_MSG_SIZE sizeof(struct hss_req_msg)
#define HSS_RESP_MSG_SIZE sizeof(struct hss_resp_msg)

#endif /*__STAGE6_S6A_MSG_H_*/


/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef __GTPV2C_IE_H_
#define __GTPV2C_IE_H_


#include <inttypes.h>
#include <stdlib.h>
#include <netinet/in.h>

#define IPV4_IE_LENGTH          4
#define IPV6_IE_LENGTH          16

/* TODO: Check size */
#define GTPV2C_MAX_LEN          4096

#define AMBR_UPLINK             100000000
#define AMBR_DOWNLINK           50000000

#define MBR_UPLINK              245018193L//245018193976L
#define MBR_DOWNLINK            245018193L//245018193976L

enum gtpv2c_ie_type {
	IE_RESERVED = 0,
	IE_IMSI = 1,
	IE_CAUSE = 2,
	IE_APN = 71,
	IE_AMBR = 72,
	IE_EBI = 73,
	IE_INDICATION = 77,
	IE_MSISDN = 76,
	IE_PAA = 79,
	IE_BEARER_QOS = 80,
	IE_RAT_TYPE = 82,
	IE_SERVING_NETWORK = 83,
	IE_ULI = 86,
	IE_FTEID = 87,
	IE_BEARER_CONTEXT = 93,
	IE_PDN_TYPE = 99,
	IE_APN_RESTRICTION = 127,
	IE_SELECTION_MODE = 128,
};

#define PDN_IP_TYPE_IPV4                                      1
#define PDN_IP_TYPE_IPV6                                      2
#define PDN_IP_TYPE_IPV4V6                                    3


enum gtpv2c_ie_instance {
	INSTANCE_ZERO = 0,
	INSTANCE_ONE = 1,
	INSTANCE_TWO = 2,
};

#pragma pack(1)


typedef struct gtpv2c_ie_t {
	uint8_t type;
	uint16_t length;
	uint8_t instance;
	uint8_t value[GTPV2C_MAX_LEN];
} gtpv2c_ie;


typedef struct fteid_ie_t {
	uint8_t iface_type :6;
	uint8_t ipv6 :1;
	uint8_t ipv4 :1;
	uint32_t teid;
	union ip_t {
		uint32_t ipv4;
		uint8_t ipv6[INET6_ADDRSTRLEN];
		struct ipv4v6_t_x {
			uint32_t ipv4;
			uint8_t ipv6[INET6_ADDRSTRLEN];
		} ipv4v6;
	} ipaddr;
} fteid_ie;


#pragma pack()

#endif /* __GTPV2C_IE_H_ */

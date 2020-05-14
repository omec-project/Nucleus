/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_COMMON_S6_COMMON_TYPES_H_
#define INCLUDE_COMMON_S6_COMMON_TYPES_H_


// S6a Access Restriction Data Bit Masks (1 << 8)
static const uint32_t nrAsSecRatInEutranNotAllowedBitMask_c = 0x00000080;

// Feature List ID 2 BitMasks (1 << 27)
static const uint32_t nrAsSecRatBitMask_c = 0x08000000;

typedef struct supported_features {
        uint8_t feature_list_id;
        uint32_t feature_list;
}supported_features;

typedef struct supported_features_list {
	uint8_t count;
	supported_features supp_features[2];
} supported_features_list;

#endif

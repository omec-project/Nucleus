/*
 * Copyright 2019-present Open Networking Foundation
 * Copyright (c) 2019 Infosys Ltd.
 * Copyright (c) 2003-2018, Great Software Laboratory Pvt. Ltd.
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>

#include <openssl/x509.h>
#include <openssl/hmac.h>

#include "sec.h"
#include "secUtils.h"
#define NEXT_HOP_FC 0x12
#define SYNC_INPUT_LEN_BYTE_1 0x00
#define SYNC_INPUT_LEN_BYTE_2 0x20
/**
 * @brief Create integrity key
 * @param[in] kasme key
 * @param[out] int_key generated integrity key
 * @return void
 */
void SecUtils::create_integrity_key(uint8_t int_alg, 
                                    unsigned char *kasme, unsigned char *int_key)
{
	/*TODO : Handle appropriate security values in salt. Remove
	 * hardcoding*/
	uint8_t salt[HASH_SALT_LEN] = {
		0x15,
		0x02, /*sec algo code*/
		0,
		1,
		int_alg,
		0,
		1
	};

	unsigned char out_key[HMAC_SIZE] = {0};
	unsigned int out_len = 0;
	calculate_hmac_sha256(salt, HASH_SALT_LEN, kasme, AIA_KASME_SIZE, out_key, &out_len);

	memcpy(int_key, &out_key[AIA_KASME_SIZE - NAS_INT_KEY_SIZE],
			NAS_INT_KEY_SIZE);
}

/**
 * @brief Create eNodeB key to exchange in init ctx message
 * @param [in]kasme key
 * @param [out]kenb_key output the generated key
 * @return void
 */
void SecUtils::create_kenb_key(unsigned char *kasme, unsigned char *kenb_key,
		unsigned int seq_no)
{
	uint8_t salt[HASH_SALT_LEN] = {
		0x11, /*TODO : Sec algo. handle properly instead of harcoding here*/
		(seq_no >> 24) & 0xFF, /*Byte 1 of seq no*/
		(seq_no >> 16) & 0xFF, /*Byte 2 of seq no*/
		(seq_no >> 8 ) & 0xFF, /*Byte 3 of seq no*/
		(seq_no      ) & 0xFF, /*Byte 4 of seq no*/
		0x00,
		0x04
	};

	uint8_t out_key[HMAC_SIZE];
	unsigned int out_len = 0;
	calculate_hmac_sha256(salt, HASH_SALT_LEN, kasme, AIA_KASME_SIZE, out_key, &out_len);
	memcpy(kenb_key, out_key, KENB_SIZE);

}

/**
 * @brief Create Cighering key
 * @param[in] kasme key
 * @param[out] int_key generated integrity key
 * @return void
 */
void SecUtils::create_ciphering_key(uint8_t sec_alg,
                                    unsigned char *kasme, unsigned char *sec_key)
{
	/*TODO : Handle appropriate security values in salt. Remove
	 * hardcoding*/
	uint8_t salt[HASH_SALT_LEN] = {
		0x15,
		0x01, /*sec algo code*/
		0,
		1,
		sec_alg,
		0,
		1
	};

	unsigned char out_key[HMAC_SIZE] = {0};
	unsigned int out_len = 0;
	calculate_hmac_sha256(salt, HASH_SALT_LEN, kasme, AIA_KASME_SIZE, out_key, &out_len);

	memcpy(sec_key, &out_key[AIA_KASME_SIZE - NAS_INT_KEY_SIZE],
			NAS_INT_KEY_SIZE);
}

/**
 * @brief Create Next hop value to exchange in Handover Request
 * @param [in]kasme key
 * @param [out]nh_key output the generated key
 * @return void
 */

void SecUtils::create_nh_key(const unsigned char *kasme, unsigned char *nh_key,
              const unsigned char *old_nh_key)
{
       uint8_t salt[35] = {0};
       salt[0] = NEXT_HOP_FC; /*TODO : Sec algo. handle properly instead of harcoding here*/
       memcpy(&(salt[1]),&( old_nh_key), KENB_SIZE);/* sync input*/
       salt[33] = SYNC_INPUT_LEN_BYTE_1;
       salt[34] = SYNC_INPUT_LEN_BYTE_2;

       uint8_t out_key[HMAC_SIZE];
       unsigned int out_len = 0;
       calculate_hmac_sha256(salt, 35, kasme, AIA_KASME_SIZE, out_key, &out_len);
       memcpy(nh_key, out_key, KENB_SIZE);

}
/**
* @brief Create MAC(message authentication code)
* @param [in]input data and  key
* @param [out]output MAC, out_len size of MAC
* @return void
*/


void SecUtils::calculate_hmac_sha256(const unsigned char *input_data,
				int input_data_len, const unsigned char *key,
				int key_length, void *output, unsigned int *out_len)
{

	unsigned int mac_length = 0;
	unsigned char mac_buffer[EVP_MAX_MD_SIZE] = {0};
	HMAC(EVP_sha256(), key, key_length, input_data, input_data_len, mac_buffer, &mac_length);
	memcpy(output, mac_buffer, mac_length);
	*out_len = mac_length;

}



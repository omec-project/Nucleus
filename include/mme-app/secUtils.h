/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */



class SecUtils
{
	public:
	static void create_integrity_key(uint8_t int_alg,
                                     unsigned char *kasme, unsigned char *int_key);

	static void create_ciphering_key(uint8_t sec_alg,
                                     unsigned char *kasme, unsigned char *int_key);

	static void create_kenb_key(unsigned char *kasme, unsigned char *kenb_key,
			unsigned int seq_no);

	static void create_nh_key(const unsigned char *kasme, unsigned char *nh_key,
            const unsigned char *old_nh_key);

	static void calculate_hmac_sha256(const unsigned char *input_data,
			int input_data_len, const unsigned char *key,
			int key_length, void *output, unsigned int *out_len);
};

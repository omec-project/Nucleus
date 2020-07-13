/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

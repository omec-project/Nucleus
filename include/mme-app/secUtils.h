class SecUtils
{
	public:
	static void create_integrity_key(unsigned char *kasme, unsigned char *int_key);

	static void create_kenb_key(unsigned char *kasme, unsigned char *kenb_key,
			unsigned int seq_no);

	static void calculate_hmac_sha256(const unsigned char *input_data,
			int input_data_len, const unsigned char *key,
			int key_length, void *output, unsigned int *out_len);
};

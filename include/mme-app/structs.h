/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <iostream>
#include "msgType.h"
#include "s1ap_structs.h"
#include "s11_structs.h"
#include "eventMessage.h"
#include "mme_app.h"
#include <memory>

struct nas_count {
    unsigned int mask :8;
    unsigned int overflow_count :16;
    unsigned int seq_no :8;
};

struct secinfo {
    uint8_t is_current_flag;
    uint8_t ksi;
    nas_ciph_algo_enum sec_alg;
    nas_int_algo_enum int_alg;
    uint32_t next_hop_chaining_count;
    uint8_t int_key[NAS_INT_KEY_SIZE];
    uint8_t sec_key[NAS_SEC_KEY_SIZE];
    uint8_t kenb_key[KENB_SIZE];
    struct KASME kasme;
    struct nas_count uplink_count;
    struct nas_count downlink_count;
    uint8_t next_hop_nh[KENB_SIZE]; /* merge issue ? ajaymerge - checkonce */
};

struct AMBR {
    uint32_t max_requested_bw_dl;  // bps
    uint32_t max_requested_bw_ul;  // bps
    uint32_t ext_max_requested_bw_dl;  // kbps
    uint32_t ext_max_requested_bw_ul;  // kbps
};

class Tai
{
	public:
		Tai();
		Tai( const TAI& tai_i );
		~Tai();
		void operator = ( const Tai& tai_i );
		bool operator== (const Tai& tai_i) const;
	public:
		TAI tai_m;
};

class Cgi
{
	public:
		Cgi();
		Cgi( const CGI& cgi_i );
		~Cgi();
		void operator = ( const Cgi& cgi_i );
	public:
		CGI cgi_m;
};

class Stmsi
{
        public:
                Stmsi();
                Stmsi( const STMSI& stmsi_i );
                ~Stmsi();
                void operator = ( const Stmsi& stmsi_i );
        public:
                STMSI stmsi_m;
};


class Arp
{
        public:
                Arp();
                Arp( const ARP& arp_i );
                ~Arp();
                void operator = ( const Arp& arp_i );
        public:
                ARP arp_m;
};

class Ms_net_capab
{
	public:
		Ms_net_capab();
		Ms_net_capab( const MS_net_capab& ms_net_capb_i );
		~Ms_net_capab();
		void operator = ( const Ms_net_capab& ms_net_capb_i );
	public:
		MS_net_capab ms_net_capab_m;
};

class Ue_net_capab
{
	public:
		Ue_net_capab();
		Ue_net_capab( const UE_net_capab& ue_net_capab_i );
		~Ue_net_capab();
		void operator = ( const Ue_net_capab& ue_net_capab_i );
	public:
		UE_net_capab ue_net_capab_m;
};

class Secinfo
{
	public:
		Secinfo();
		Secinfo( const secinfo& secinfo_i );
		~Secinfo();
		void operator = ( const Secinfo& secinfo_i );
        void setSelectedIntAlg(nas_int_algo_enum val)
        {
            secinfo_m.int_alg = val;
        }
        void setSelectedSecAlg(nas_ciph_algo_enum val)
        {
            secinfo_m.sec_alg = val;
        }
        nas_int_algo_enum getSelectIntAlg() { return secinfo_m.int_alg; }
        nas_ciph_algo_enum getSelectSecAlg() { return secinfo_m.sec_alg; }
        void resetUplinkCount()
        {
            secinfo_m.uplink_count.seq_no = 0;
            secinfo_m.uplink_count.overflow_count = 0;
            secinfo_m.uplink_count.mask = 0;
        }
        void resetDownlinkCount()
        {
            secinfo_m.downlink_count.seq_no = 0;
            secinfo_m.downlink_count.overflow_count = 0;
            secinfo_m.downlink_count.mask = 0;
        }
        uint8_t getUplinkSeqNo()
        {
            return secinfo_m.uplink_count.seq_no;
        }
        uint32_t getUplinkCount()
        {
            uint32_t val = 0;
            uint32_t seqno  = secinfo_m.uplink_count.seq_no;
            uint32_t oc = secinfo_m.uplink_count.overflow_count;
            val = seqno | (oc << 8);
            return val;
        }
        uint8_t getDownlinkSeqNo()
        {
            return secinfo_m.downlink_count.seq_no;
        }
        uint32_t getDownlinkCount()
        {
            uint32_t val = 0;
            uint32_t seqno  = secinfo_m.downlink_count.seq_no;
            uint32_t oc = secinfo_m.downlink_count.overflow_count;
            val = seqno | (oc << 8);
            return val;
        }
        void increment_uplink_count()
        {
            secinfo_m.uplink_count.seq_no++;
            if(!secinfo_m.uplink_count.seq_no)
            {
                secinfo_m.uplink_count.overflow_count++;
            }
        }
        void increment_downlink_count()
        {
            secinfo_m.downlink_count.seq_no++;
            if(!secinfo_m.downlink_count.seq_no)
            {
                secinfo_m.downlink_count.overflow_count++;
            }
        }

		void getIntKey(uint8_t *key) const { memcpy(key, &secinfo_m.int_key[0], NAS_INT_KEY_SIZE);}
	public:
		secinfo secinfo_m;
};

class Ambr
{
	public:
		Ambr();
		Ambr( const AMBR& ambr_i );
		~Ambr();
		void operator = ( const Ambr& ambr_i );
	public:
		AMBR ambr_m;
};

class E_utran_sec_vector
{
	public:
		E_utran_sec_vector();
		E_utran_sec_vector( const E_UTRAN_sec_vector& secinfo_i );
		~E_utran_sec_vector();
		void operator = ( const E_utran_sec_vector& secinfo_i );
		friend std::ostream& operator << ( std::ostream& os, const E_utran_sec_vector& data_i );
	public:
		E_UTRAN_sec_vector* AiaSecInfo_mp;
};
	
class Fteid
{
	public:
		Fteid();
		Fteid( const fteid& fteid_i );
		~Fteid();
		void operator = ( const Fteid& fteid_i );
		bool operator == (const Fteid& fteid_i) const;
	public:
		fteid fteid_m;
};

class Paa
{
	public:
		Paa();
		Paa( const PAA& paa_i );
		~Paa();
		void operator = ( const Paa& paa_i );
	public:
		PAA paa_m;
};

class Apn_name
{
	public:
		Apn_name();
		Apn_name( const apn_name& apn_name_i );
		~Apn_name();
		void operator = ( const Apn_name& apn_name_i );
		bool operator== (const Apn_name& apn_name_i) const;
	public:
		apn_name apnname_m;
};

class Auts
{
	public:
		Auts();
		Auts( const AUTS& auts_i );
		~Auts();
		void operator = ( const Auts& auts_i );
	public:
		AUTS auts_m;
};

class S1apCause
{
	public:
                S1apCause();
                S1apCause( const s1apCause_t& s1apCause_i );
                ~S1apCause();
                void operator = ( const S1apCause& s1apCause_i );
        public:
                s1apCause_t s1apCause_m;
};

class DigitRegister15
{
	public:
		DigitRegister15();
		void convertToBcdArray(uint8_t* arrayOut) const;
		void convertFromBcdArray(const uint8_t* bcdArrayIn);
		void setImsiDigits( uint8_t* digitsArrayIn );
		void getImsiDigits( uint8_t* digitsArrayIn ) const;	
		bool isValid() const;
		void operator = ( const DigitRegister15& data_i );
		bool operator == ( const DigitRegister15& data_i )const;
		bool operator < ( const DigitRegister15& data_i )const;
		const uint8_t * getDigitsArray() const;

		friend std::ostream& operator << ( std::ostream& os, const DigitRegister15& data_i );
	
	private:

		uint8_t digitsArray[15];
};

struct DigitRegister15Hash {
    std::size_t operator()(const DigitRegister15 &data_i ) const
    {
	std::size_t hash=0;
	std::hash< uint8_t> hasher;
	for (int i = 0;i<15;i++)
	{
		hash = hash *31+ hasher(data_i.getDigitsArray()[i]);
	}
        return hash;
    }
};

class BearerCtxtCBResp
{
        public:
                BearerCtxtCBResp();
                BearerCtxtCBResp( bearer_ctxt_cb_resp_t& bearer_ctxt_cb_resp_i );
                ~BearerCtxtCBResp();
                void operator = ( const BearerCtxtCBResp& bearer_ctxt_cb_resp_i );
		bool operator == ( const BearerCtxtCBResp& bearer_ctxt_cb_resp_i )const;
        public:
                bearer_ctxt_cb_resp_t bearer_ctxt_cb_resp_m;
};

class BearerCtxtDBResp
{
        public:
                BearerCtxtDBResp();
                ~BearerCtxtDBResp();
                bool operator == ( const BearerCtxtDBResp& bearer_ctxt_db_resp_i )const;
        public:
                uint8_t eps_bearer_id;
                struct gtp_cause cause;
                struct pco pco_from_ue_opt;
};

class BearerStatusEMsg : public cmn::EventMessage
{
public:
    BearerStatusEMsg(uint8_t bearerId, uint8_t cause):
        EventMessage(BEARER_STATUS_EMSG), bearerId_m(bearerId), cause_m(cause)
    { }

    ~BearerStatusEMsg(){}

    uint8_t getBearerId() const
    {
        return bearerId_m;
    }

    uint8_t getCause() const
    {
        return cause_m;
    }

private:
    uint8_t bearerId_m;
    uint8_t cause_m;
};

using BearerStatusEMsgShPtr = std::shared_ptr<BearerStatusEMsg>;

class NasPduParseFailureIndEMsg : public cmn::EventMessage
{
public:
    NasPduParseFailureIndEMsg(unsigned char msgType, uint32_t s1ap_enb_ue_id, uint32_t errCode):
        EventMessage(NAS_PDU_PARSE_FAILURE_IND_EMSG),
        nasMsgType_m(msgType),
        s1ap_enb_ue_id_m(s1ap_enb_ue_id),
        errorCode_m(errCode)
    { }

    ~NasPduParseFailureIndEMsg(){}

    unsigned char getNasMsgType() const
    {
        return nasMsgType_m;
    }

    uint32_t getS1apEnbUeId() const
    {
        return s1ap_enb_ue_id_m;
    }

    uint32_t getErrorCode() const
    {
        return errorCode_m;
    }

private:
    unsigned char nasMsgType_m;
    uint32_t s1ap_enb_ue_id_m;
    uint32_t errorCode_m;
};

using NasPduParseFailureIndEMsgShPtr = std::shared_ptr<NasPduParseFailureIndEMsg>;

#endif

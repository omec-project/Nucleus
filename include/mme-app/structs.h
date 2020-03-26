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

#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <iostream>
#include "msgType.h"
#include "s1ap_structs.h"
#include "s11_structs.h"
#include <utils/mmeProcedureTypes.h>


struct secinfo {
    uint8_t int_key[NAS_INT_KEY_SIZE];
    uint8_t kenb_key[KENB_SIZE];
};

struct AMBR {
    unsigned int max_requested_bw_dl;
    unsigned int max_requested_bw_ul;
};

class Tai
{
	public:
		Tai();
		Tai( const TAI& tai_i );
		~Tai();
		void operator = ( const Tai& tai_i );
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

		friend std::ostream& operator << ( std::ostream& os, const DigitRegister15& data_i );
	
	private:

		uint8_t digitsArray[15];
};

#endif

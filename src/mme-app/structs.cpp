/*
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string>
#include <sstream>
#include <cstring>
#include <iomanip>
#include "structs.h"
#include "log.h"

using namespace std;

uint8_t ASCII_TO_BCD(uint8_t left, uint8_t right)
{
	uint8_t bcd_value;

	if(left >= '0' && left <= '9')  // 0-9 range
	{
		bcd_value = ( left - 0x30)  << 4 ; // 48 for '0' ASCII offset
	}
	else
	{
		bcd_value = 0x0f << 4; // filler 1111
	}

	if(right  >= '0' && right <= '9')  // 0-9 range
	{
		bcd_value |= ( right - 0x30); // 48 for '0' ASCII offset
	}
	else
	{
		bcd_value |= 0x0f; // filler 1111
	}

	return bcd_value;
}

/*******************************************************
 *TAI
********************************************************/
Tai::Tai()
{
	 memset( &tai_m, 0, sizeof( tai_m ));
}

Tai::Tai( const TAI& tai_i )
{
	memset( &tai_m, 0, sizeof( tai_m ));
	memcpy( &tai_m, &tai_i, sizeof( tai_i ));
}

Tai::~Tai()
{
}

void Tai::operator = ( const Tai& tai_i )
{
	memcpy( &tai_m, &(tai_i.tai_m), sizeof( tai_i.tai_m ));
}

bool Tai::operator==(const Tai &tai_i) const
{
    return (memcmp(&tai_m, &tai_i.tai_m, sizeof(struct TAI)) == 0);
}

/*******************************************************
 *CGI
********************************************************/
Cgi::Cgi()
{
	memset( &cgi_m, 0, sizeof( cgi_m ));
}

Cgi::Cgi( const CGI& cgi_i )
{
	memset( &cgi_m, 0, sizeof( cgi_m ));
	memcpy( &cgi_m, &cgi_i, sizeof(cgi_i));
}

Cgi::~Cgi()
{
}

void Cgi::operator = ( const Cgi& cgi_i )
{
	memcpy( &cgi_m, &(cgi_i.cgi_m), sizeof(cgi_i.cgi_m));
}

/*******************************************************
 *STMSI
********************************************************/
Stmsi::Stmsi()
{
	 memset( &stmsi_m, 0, sizeof( stmsi_m ));
}

Stmsi::Stmsi( const STMSI& stmsi_i )
{
	memset( &stmsi_m, 0, sizeof( stmsi_m ));
	memcpy( &stmsi_m, &stmsi_i, sizeof( stmsi_i ));
}

Stmsi::~Stmsi()
{
}

void Stmsi::operator = ( const Stmsi& stmsi_i )
{
	memcpy( &stmsi_m, &(stmsi_i.stmsi_m), sizeof( stmsi_i.stmsi_m ));
}

/*******************************************************
 *ARP
********************************************************/
Arp::Arp()
{
	 memset( &arp_m, 0, sizeof( arp_m ));
}

Arp::Arp( const ARP& arp_i )
{
	memset( &arp_m, 0, sizeof( arp_m ));
	memcpy( &arp_m, &arp_i, sizeof( arp_i ));
}

Arp::~Arp()
{
}

void Arp::operator = ( const Arp& arp_i )
{
	memcpy( &arp_m, &(arp_i.arp_m), sizeof( arp_i.arp_m ));
}


/*******************************************************
 *Ms_net_capab
********************************************************/
Ms_net_capab::Ms_net_capab()
{
	memset( &ms_net_capab_m, 0, sizeof( ms_net_capab_m ));
}

Ms_net_capab::Ms_net_capab( const MS_net_capab& net_cap_i )
{
	memset( &ms_net_capab_m, 0, sizeof( ms_net_capab_m ));
	memcpy( &ms_net_capab_m, &net_cap_i, sizeof( net_cap_i ));
}

Ms_net_capab::~Ms_net_capab()
{
}

void Ms_net_capab::operator = ( const Ms_net_capab& net_cap_i )
{
	memcpy( &ms_net_capab_m, &(net_cap_i.ms_net_capab_m), sizeof( net_cap_i.ms_net_capab_m ));
}

/*******************************************************
*Ue_net_capab
********************************************************/
Ue_net_capab::Ue_net_capab()
{
	memset( &ue_net_capab_m, 0, sizeof( ue_net_capab_m ));
}

Ue_net_capab::Ue_net_capab( const UE_net_capab& net_cap_i )
{
	memset( &ue_net_capab_m, 0, sizeof( ue_net_capab_m ));
	memcpy( &ue_net_capab_m, &net_cap_i, sizeof( net_cap_i ));
}

Ue_net_capab::~Ue_net_capab()
{
}

void Ue_net_capab::operator = ( const Ue_net_capab& net_cap_i )
{
	memcpy( &ue_net_capab_m, &(net_cap_i.ue_net_capab_m), sizeof( net_cap_i.ue_net_capab_m ));
}

/*******************************************************
*Secinfo
********************************************************/
Secinfo::Secinfo()
{
	memset( &secinfo_m, 0, sizeof( secinfo_m ));
}

Secinfo::Secinfo( const secinfo& sec_i )
{
	memset( &secinfo_m, 0, sizeof( secinfo_m ));
	memcpy( &secinfo_m, &sec_i, sizeof( sec_i ));
}

Secinfo::~Secinfo()
{
}

void Secinfo::operator = ( const Secinfo& sec_i )
{
	memcpy( &secinfo_m, &(sec_i.secinfo_m), sizeof( sec_i.secinfo_m ));
}

/*******************************************************
*Ambr
********************************************************/
Ambr::Ambr()
{
	memset( &ambr_m, 0, sizeof( ambr_m ));
}

Ambr::Ambr( const AMBR& ambr_i )
{
	memset( &ambr_m, 0, sizeof( ambr_m ));
	memcpy( &ambr_m, &ambr_i, sizeof( ambr_i ));
}

Ambr::~Ambr()
{
}

void Ambr::operator = ( const Ambr& ambr_i )
{
	memcpy( &ambr_m, &(ambr_i.ambr_m), sizeof( ambr_i.ambr_m ));
}

/*******************************************************
*E_utran_sec_vector
********************************************************/
E_utran_sec_vector::E_utran_sec_vector():AiaSecInfo_mp( NULL )
{
	AiaSecInfo_mp = (struct E_UTRAN_sec_vector*)calloc(sizeof(struct E_UTRAN_sec_vector), 1);
}

E_utran_sec_vector::E_utran_sec_vector( const E_UTRAN_sec_vector& secinfo_i )
{
	AiaSecInfo_mp = (struct E_UTRAN_sec_vector*)calloc(sizeof(struct E_UTRAN_sec_vector), 1);
	memcpy( AiaSecInfo_mp, &secinfo_i, sizeof( E_UTRAN_sec_vector ));	
}

E_utran_sec_vector::~E_utran_sec_vector()
{
	free(AiaSecInfo_mp);
}

void E_utran_sec_vector::operator = ( const E_utran_sec_vector& secinfo_i )
{
	if( NULL != secinfo_i.AiaSecInfo_mp )
	{
		memcpy( AiaSecInfo_mp, secinfo_i.AiaSecInfo_mp, sizeof( E_UTRAN_sec_vector ));
	}
}

std::ostream& operator << ( std::ostream& os, const E_utran_sec_vector& data_i )
{
	os << "RAND ";
	for ( uint32_t i = 0; i < data_i.AiaSecInfo_mp->rand.len; i++) 
	{
		os << setfill('0') << setw(2) << std::hex << 
			static_cast<uint32_t>(data_i.AiaSecInfo_mp->rand.val[i]);
	}
	os << std::endl;
	
	os  << "RES ";
	for ( uint32_t i = 0; i < data_i.AiaSecInfo_mp->xres.len; i++)
	{
		os << setfill('0') << setw(2) << std::hex << 
			static_cast<uint32_t>(data_i.AiaSecInfo_mp->xres.val[i]);
	}
	os << std::endl;
	
	os << "AUTN ";
	for ( uint32_t i = 0; i < data_i.AiaSecInfo_mp->autn.len; i++)
	{
		os << setfill('0') << setw(2) << std::hex << 
			static_cast<uint32_t>(data_i.AiaSecInfo_mp->autn.val[i]);
	}
	os << std::endl;
	
	os << "KASME ";
	for ( uint32_t i = 0; i < data_i.AiaSecInfo_mp->kasme.len; i++)
	{
		os << setfill('0') << setw(2) << std::hex << 
			static_cast<uint32_t>(data_i.AiaSecInfo_mp->kasme.val[i]);
	}
	os << std::endl;
	
	return os;
}

/*******************************************************
*Fteid
********************************************************/
Fteid::Fteid()
{
	memset( &fteid_m, 0, sizeof( fteid_m ));
}

Fteid::Fteid( const fteid& fteid_i )
{
	memset( &fteid_m, 0, sizeof( fteid_m ));
	memcpy( &fteid_m, &fteid_i, sizeof( fteid_i ));
}

Fteid::~Fteid()
{
}

void Fteid::operator = ( const Fteid& fteid_i )
{
	memcpy( &fteid_m, &(fteid_i.fteid_m), sizeof( fteid_i.fteid_m ));
}

/*******************************************************
*Paa
********************************************************/
Paa::Paa()
{
	memset( &paa_m, 0, sizeof( paa_m ));
}

Paa::Paa( const PAA& paa_i )
{
	memset( &paa_m, 0, sizeof( paa_m ));
	memcpy( &paa_m, &paa_i, sizeof( paa_i ));
}

Paa::~Paa()
{
}

void Paa::operator = ( const Paa& paa_i )
{
	memcpy( &paa_m, &(paa_i.paa_m), sizeof( paa_i.paa_m ));
}

/*******************************************************
*Apn_name
********************************************************/
Apn_name::Apn_name()
{
	memset( &apnname_m, 0, sizeof( apnname_m ));
}

Apn_name::Apn_name( const apn_name& apn_name_i )
{
	memset( &apnname_m, 0, sizeof( apnname_m ));
	memcpy( &apnname_m, &apn_name_i, sizeof( apn_name_i ));
}

Apn_name::~Apn_name()
{
}

void Apn_name::operator = ( const Apn_name& apn_name_i )
{
	memcpy( &apnname_m, &(apn_name_i.apnname_m), sizeof( apn_name_i.apnname_m ));
}


/*******************************************************
*Auts
********************************************************/
Auts::Auts()
{
	memset( &auts_m, 0, sizeof( auts_m ));
}

Auts::Auts( const AUTS& auts_i )
{
	memset( &auts_m, 0, sizeof( auts_m ));
	memcpy( &auts_m, &auts_i, sizeof( auts_i ));
}

Auts::~Auts()
{
}

void Auts::operator = ( const Auts& auts_i )
{
	memcpy( &auts_m, &(auts_i.auts_m), sizeof( auts_i.auts_m ));
}


/*******************************************************
*S1apCause
********************************************************/
S1apCause::S1apCause()
{
    memset( &s1apCause_m, 0, sizeof( s1apCause_m ));
}

S1apCause::S1apCause( const s1apCause_t& s1apCause_i )
{
    memset( &s1apCause_m, 0, sizeof( s1apCause_m ));
    memcpy( &s1apCause_m, &s1apCause_i, sizeof( s1apCause_i ));
}

S1apCause::~S1apCause()
{
}

void S1apCause::operator = ( const S1apCause& s1apCause_i )
{
    memcpy( &s1apCause_m, &(s1apCause_i.s1apCause_m), sizeof( s1apCause_i.s1apCause_m ));
}

/*******************************************************
*DigitRegister15
********************************************************/
DigitRegister15::DigitRegister15()
{
	memset(&digitsArray,0,15);
}

void DigitRegister15::convertToBcdArray( uint8_t * arrayOut ) const
{
	for( int i=0; i < 7; i++ )
	{
		arrayOut[i] = ASCII_TO_BCD(digitsArray[2*i+1],digitsArray[2*i]);
	}
	arrayOut[7] = ASCII_TO_BCD(0x0f,digitsArray[14]);
}

void DigitRegister15::convertFromBcdArray( const uint8_t* bcdArrayIn )
{
	// For the imsi value upper nibble of first octect should only be considered
	if(( bcdArrayIn[0] & 0x0f ) == 0x0f )
	{
		digitsArray[0] = (uint8_t)(((bcdArrayIn[0] >> 4) & 0x0f ) + 0x30);
		for( int i = 1; i < 8; i++ )
		{
			digitsArray[2*i] = (uint8_t)(((bcdArrayIn[i] >> 4) & 0x0f) + 0x30);
			digitsArray[2*i-1] = (uint8_t)((bcdArrayIn[i] & 0x0f) + 0x30);
		}
	}
	else
	{
		for( int i = 0; i < 7; i++ )
		{
			digitsArray[2*i+1] = (uint8_t)(((bcdArrayIn[i] >> 4) & 0x0f) + 0x30);
			digitsArray[2*i] = (uint8_t)((bcdArrayIn[i] & 0x0f) + 0x30);
		}
		digitsArray[14] = (uint8_t)((bcdArrayIn[7]  & 0x0f) );
	}
                
	return;
}


void DigitRegister15::setImsiDigits( uint8_t* digitsArrayIn )
{
	memcpy( digitsArray, digitsArrayIn, 15);
}

void DigitRegister15::getImsiDigits( uint8_t* digitsArrayIn ) const
{
	memcpy( digitsArrayIn, digitsArray, 15);
}

bool DigitRegister15::isValid() const
{
    bool rc = false;
    for (uint32_t i = 0; i < 15; i++)
    {
        if (digitsArray[i] != 0)
        {
            rc = true;
            break;
        }
    }
    return rc;
}

void DigitRegister15::operator = ( const DigitRegister15& data_i )
{
	memcpy( digitsArray, data_i.digitsArray, sizeof( data_i.digitsArray ));
}

bool DigitRegister15::operator == ( const DigitRegister15& data_i )const
{
	int data_cmp = memcmp(digitsArray, data_i.digitsArray,  sizeof( data_i.digitsArray));
	bool rc = false;
	if(data_cmp == 0)
		rc = true;
	return rc;
}

bool DigitRegister15::operator < ( const DigitRegister15& data_i )const
{
    bool rc = true;
    int data_cmp = memcmp(digitsArray, data_i.digitsArray,  sizeof( data_i.digitsArray));
    if (data_cmp >= 0)
        rc = false;
    return rc;
}

std::ostream& operator << ( std::ostream& os, const DigitRegister15& data_i )
{
	for( auto x : data_i.digitsArray )
	{
		if( x  != 0x0f )
			os << x - 0x30;
	}
	os << "\0";	
	return os;
}


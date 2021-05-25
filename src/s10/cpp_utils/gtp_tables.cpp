// Copyright 2020-present Open Networking Foundation
//
// SPDX-License-Identifier: LicenseRef-ONF-Member-Only-1.0

#include <iostream>
#include <map>
#include "gtp_tables.h"

int gtpTables::addSeqKey( uint32_t key, uint32_t ue_index )
{
     std::lock_guard<std::mutex> lock(seq_ueidx_map_mutex);

     int rc = 1;

     auto itr = seq_ue_idx_map.insert(std::pair<uint32_t, uint32_t>( key, ue_index ));
     if (itr.second == false)
     {
         rc = -1;
     }
     return rc;
}

 /******************************************
  * Delete seq number key
  ******************************************/
int gtpTables::delSeqKey( uint32_t key )
{
     std::lock_guard<std::mutex> lock(seq_ueidx_map_mutex);

     return seq_ue_idx_map.erase( key );
}

/******************************************
 * Find Ue Idx with given Seq number
 ******************************************/ 
int gtpTables::findUeIdxWithSeq( uint32_t key )
{
    std::lock_guard<std::mutex> lock(seq_ueidx_map_mutex);

    auto itr = seq_ue_idx_map.find( key );
    if( itr != seq_ue_idx_map.end())
    {
        return itr->second;
    }
    return -1;
}

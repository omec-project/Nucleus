// Copyright 2020-present Open Networking Foundation
//
// SPDX-License-Identifier: LicenseRef-ONF-Member-Only-1.0

#ifndef __GTP_TABLES_H__
#define __GTP_TABLES_H__
#include <iostream>
#include <map>
#include <mutex>

class gtpTables
{
   public:
      gtpTables()
      {
      }
      ~gtpTables()
      {
      }
    int addSeqKey( uint32_t key, uint32_t ue_index);
    int delSeqKey( uint32_t key );
    int findUeIdxWithSeq( uint32_t key);
	std::map<uint32_t,uint32_t> seq_ue_idx_map;
 	std::mutex seq_ueidx_map_mutex;
};
#endif

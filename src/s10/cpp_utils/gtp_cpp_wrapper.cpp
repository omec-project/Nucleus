// Copyright 2020-present Open Networking Foundation
//
// SPDX-License-Identifier: LicenseRef-ONF-Member-Only-1.0

#include "gtp_tables.h"
static gtpTables *table = nullptr; 

extern "C"
{
    #include "gtp_cpp_wrapper.h"
    #include "log.h"

    void init_cpp_gtp_tables(void)
    {
        table = new gtpTables();
    }

    int add_gtp_transaction(uint32_t msg_seq, uint32_t ue_index)
    {
        log_msg(LOG_DEBUG, "Seq %d, Index - %d ",msg_seq, ue_index);
        return table->addSeqKey(msg_seq, ue_index); 
    }

    int find_gtp_transaction(uint32_t msg_seq)
    {
        log_msg(LOG_DEBUG,"Seq %d ",msg_seq);
        return table->findUeIdxWithSeq(msg_seq); 
    }

    int delete_gtp_transaction(uint32_t msg_seq) 
    {
        return table->delSeqKey(msg_seq); 
    }
}

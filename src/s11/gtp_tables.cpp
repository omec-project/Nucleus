/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <iostream>
#include <map>
#include "gtp_tables.h"
#include "log.h"

using namespace s11;

uint32_t gtpTables::addSeqKey(gtpTrans &trans, gtpTransData *data)
{
    std::lock_guard<std::mutex> lock(seq_ueidx_map_mutex);
    int rc = 1;
    auto itr = gtp_transaction_map.insert(std::pair<gtpTrans, gtpTransData*>(trans, data));
    if (itr.second == false)
    {
        rc = -1;
    }
    log_msg(LOG_DEBUG, "Added GTP transaction %s \n", trans.printTrans().c_str());
    return rc;
}

 /******************************************
  * Delete seq number key
  ******************************************/
gtpTransData* gtpTables::delSeqKey(gtpTrans &key)
{
    std::lock_guard<std::mutex> lock(seq_ueidx_map_mutex);
    std::map<gtpTrans, gtpTransData*>::iterator it;

    it = gtp_transaction_map.find(key);
    if(it == gtp_transaction_map.end())
    {
        std::cout<<"Key not Found"<<std::endl;
        log_msg(LOG_DEBUG, "Delete GTP transaction %s, Result- Not Found \n", key.printTrans().c_str());
        return 0;
    }
    else
    {
        log_msg(LOG_DEBUG,"Delete GTP transaction %s, Result - found - \n",key.printTrans().c_str());
        gtp_transaction_map.erase(it);
        return it->second;
    }
}

/******************************************
 * Find Ue Idx with given Seq number
 ******************************************/ 
gtpTransData* gtpTables::findUeIdxWithSeq(gtpTrans &key )
{
    std::lock_guard<std::mutex> lock(seq_ueidx_map_mutex);
    std::map<gtpTrans, gtpTransData*>::iterator it;

    it = gtp_transaction_map.find(key);
    if(it == gtp_transaction_map.end())
    {
        log_msg(LOG_DEBUG,"Find GTP transaction %s, Result - not found -\n ",key.printTrans().c_str());
        return 0;
    }
    else
    {
        log_msg(LOG_DEBUG,"Find GTP transaction %s, Result - found - \n",key.printTrans().c_str());
        return it->second;
    }
}

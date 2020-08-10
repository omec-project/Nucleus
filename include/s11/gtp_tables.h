/*
 * Copyright 2020-present Open Networking Foundation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __GTP_TABLES_H__
#define __GTP_TABLES_H__
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "msgBuffer.h"
#include "utils/s11TimerTypes.h"

class gtpTrans 
{
    public:
        gtpTrans(uint32_t ip, uint32_t port, uint32_t num)
        {
            src_port = port; 
            src_ip = ip;
            seq_num = num;
        }
        ~gtpTrans() {}
        std::string printTrans()
        {
            std::stringstream op;
            struct in_addr test; test.s_addr = src_ip;
            op<<"("<<inet_ntoa(test)<<","<<ntohs(src_port)<<","<<seq_num<<")";
            return std::move(op.str()); 
        }
    public:
        uint32_t src_port;
        uint32_t src_ip;
        uint32_t seq_num;
};

//#define DEBUG_LOG_CPP
struct comp
{
    bool operator()(const gtpTrans &lhs, const gtpTrans &rhs) const
    {
#ifdef DEBUG_LOG_CPP
        std::cout<<std::endl<<"**Compare function called "<<std::endl;
#endif
        if(lhs.src_port == rhs.src_port)
        {
#ifdef DEBUG_LOG_CPP
            std::cout<<"Port Match found "<<std::endl;
#endif
            if(lhs.src_ip == rhs.src_ip)
            {
#ifdef DEBUG_LOG_CPP
                std::cout<<"Source Match found "<<std::endl;
#endif
                if(lhs.seq_num == rhs.seq_num)
                {
#ifdef DEBUG_LOG_CPP
                    std::cout<<"Seq num Match found "<<std::endl;
#endif
                    return false;
                }
#ifdef DEBUG_LOG_CPP
                std::cout<<"seq num not matched "<<std::endl;
#endif
                return lhs.seq_num < rhs.seq_num;
            }
#ifdef DEBUG_LOG_CPP
            std::cout<<"IP address not matched "<<std::endl;
#endif
            return lhs.src_ip < rhs.src_ip;
        }
#ifdef DEBUG_LOG_CPP
        std::cout<<"port number not matched "<<std::endl;
#endif
        return lhs.src_port < rhs.src_port;
    }
};

class gtpTransData
{
    public:
        gtpTransData(uint32_t index):key(0,0,0) 
    { 
        ue_index = index; 
        tx_count = 0; 
        buf = nullptr;
        timer = nullptr;
    } 
        ~gtpTransData() { delete buf;}
    public:
        gtpTrans key;
        uint32_t ue_index; 
        cmn::utils::MsgBuffer *buf;
        s11TimerContext *timer;
        struct sockaddr_in sgw_addr;
        int fd;
        uint16_t tx_count;
};

class gtpTables
{
    public:
        std::map<gtpTrans, gtpTransData*, comp> gtp_transaction_map; 
        std::mutex seq_ueidx_map_mutex;
    public:
        gtpTables()
        {
        }

        ~gtpTables()
        {
        }

        static gtpTables* Instance() 
        {
            static gtpTables obj;
            return &obj; 
        }

        uint32_t addSeqKey(gtpTrans &key, gtpTransData *trans);
        gtpTransData* delSeqKey(gtpTrans &key);
        gtpTransData* findUeIdxWithSeq(gtpTrans &key);
};
#endif

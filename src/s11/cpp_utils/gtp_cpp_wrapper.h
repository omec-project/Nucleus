// Copyright 2020-present Open Networking Foundation
//
// SPDX-License-Identifier: LicenseRef-ONF-Member-Only-1.0

#ifndef __S11__CPP_WRAPPER__H
#define __S11__CPP_WRAPPER__H
void init_cpp_gtp_tables(void);
int add_gtp_transaction(uint32_t msg_seq, uint32_t ue_index);
int find_gtp_transaction(uint32_t msg_seq);
int delete_gtp_transaction(uint32_t msg_seq); 
#endif

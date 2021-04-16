/*
 * Copyright 2021-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#ifndef __TransactionDb_H
#define __TransactionDb_H

#include <stdint.h>

typedef struct DbItem {
    void *appData_p;
}DbItem;

#ifdef __cplusplus
#include <unordered_map>
#include <mutex>

namespace cmn {

class TransactionDb
{
public:

    TransactionDb();

    ~TransactionDb();

    bool addItem(uint32_t key, DbItem& item);

    bool findItem(uint32_t key, DbItem& item);

    uint32_t deleteItem(uint32_t key);

private:
    std::unordered_map<uint32_t, DbItem> transDb_m;
    std::mutex mutex_m;
};

};
#endif // #ifdef __cplusplus

#ifdef __cplusplus

extern "C"
{
#endif // #ifdef __cplusplus

void * newTransDb();
bool addTransItem(void* db, uint32_t key, DbItem *item);
void deleteTransItem(void* db, uint32_t key);
bool findTransItem(void* db, uint32_t key, DbItem* item);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus

#endif // #ifndef __TransactionDb_H

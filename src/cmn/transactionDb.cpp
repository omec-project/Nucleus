/*
 * Copyright 2021-present Infosys Limited  
 *   
 * SPDX-License-Identifier: Apache-2.0    
 */

#include <transactionDb.h>

cmn::TransactionDb::TransactionDb() : transDb_m(), mutex_m()
{

}

cmn::TransactionDb::~TransactionDb()
{

}

bool cmn::TransactionDb::addItem(uint32_t key, DbItem& item)
{
    std::unique_lock<std::mutex> lock(mutex_m);

    bool rc = true;

    auto itr = transDb_m.insert({key, item});
    if(itr.second == false)
    {
        rc = false;
    }

    return rc;
}

bool cmn::TransactionDb::findItem(uint32_t key, DbItem& item)
{
    bool rc = false;

    std::unique_lock<std::mutex> lock(mutex_m);

    auto foundItem = transDb_m.find(key);
    if (foundItem != transDb_m.end())
    {
        item = foundItem->second;
        rc = true;
    }
    return rc;
}

uint32_t cmn::TransactionDb::deleteItem(uint32_t key)
{
    std::unique_lock<std::mutex> lock(mutex_m);
    return transDb_m.erase(key);
}


void * newTransDb()
{
    cmn::TransactionDb *db(new cmn::TransactionDb());
    return (reinterpret_cast<void *>(db));
}

bool addTransItem(void* db, uint32_t key, DbItem *item)
{
    return reinterpret_cast<cmn::TransactionDb *>(db)->addItem(key, *item);
}

void deleteTransItem(void* db, uint32_t key)
{
    reinterpret_cast<cmn::TransactionDb *>(db)->deleteItem(key);
}

bool findTransItem(void* db, uint32_t key, DbItem *item)
{
   return reinterpret_cast<cmn::TransactionDb *>(db)->findItem(key, *item);
}

/*
 * Copyright 2020-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMPONENTDB_H_
#define COMPONENTDB_H_

#include <map>
#include <stdint.h>

namespace cmn
{
    class ComponentIf{
    public:
    virtual ~ComponentIf() {};
    };

    class ComponentDb{
    public:
    void registerComponent(uint16_t componentId, ComponentIf* comp);
    ComponentIf& getComponent(uint16_t componentId);
  
    private:
    std::map<uint16_t, ComponentIf*> componentContainer;
    };

extern ComponentDb compDb;
}

#endif /* COMPONENTDB_H_ */



/*
 * Copyright (c) 2020, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 /******************************************************************************
 *
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/GtpV2StackCodeGen/tts/ietemplate.h.tt>
 ******************************************************************************/
#ifndef MAPPEDUEUSAGETYPEIE_H_
#define MAPPEDUEUSAGETYPEIE_H_

#include "manual/gtpV2Ie.h"



class MappedUeUsageTypeIe: public GtpV2Ie {
public:
    MappedUeUsageTypeIe();
    virtual ~MappedUeUsageTypeIe();

    bool encodeMappedUeUsageTypeIe(MsgBuffer &buffer,
                 MappedUeUsageTypeIeData const &data);
    bool decodeMappedUeUsageTypeIe(MsgBuffer &buffer,
                 MappedUeUsageTypeIeData &data, Uint16 length);
    void displayMappedUeUsageTypeIe_v(MappedUeUsageTypeIeData const &data,
                 Debug &stream);
};

#endif /* MAPPEDUEUSAGETYPEIE_H_ */

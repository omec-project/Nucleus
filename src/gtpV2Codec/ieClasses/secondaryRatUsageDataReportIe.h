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
#ifndef SECONDARYRATUSAGEDATAREPORTIE_H_
#define SECONDARYRATUSAGEDATAREPORTIE_H_

#include "manual/gtpV2Ie.h"



class SecondaryRatUsageDataReportIe: public GtpV2Ie {
public:
    SecondaryRatUsageDataReportIe();
    virtual ~SecondaryRatUsageDataReportIe();

    bool encodeSecondaryRatUsageDataReportIe(MsgBuffer &buffer,
                 SecondaryRatUsageDataReportIeData const &data);
    bool decodeSecondaryRatUsageDataReportIe(MsgBuffer &buffer,
                 SecondaryRatUsageDataReportIeData &data, Uint16 length);
    void displaySecondaryRatUsageDataReportIe_v(SecondaryRatUsageDataReportIeData const &data,
                 Debug &stream);
};

#endif /* SECONDARYRATUSAGEDATAREPORTIE_H_ */

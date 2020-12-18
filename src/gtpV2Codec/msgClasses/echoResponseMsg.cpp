/*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *
 * This is an auto generated file.
 * Please do not edit this file.
 * All edits to be made through template source file
 * <TOP-DIR/scripts/GtpV2StackCodeGen/tts/msgtemplate.cpp.tt>
 ******************************************************************************/ 

#include "echoResponseMsg.h"
#include "../ieClasses/manual/gtpV2Ie.h"
#include "../ieClasses/gtpV2IeFactory.h"
#include "../ieClasses/recoveryIe.h"
#include "../ieClasses/nodeFeaturesIe.h"

EchoResponseMsg::EchoResponseMsg()
{
    msgType = EchoResponseMsgType;
    Uint16 mandIe;
    mandIe = RecoveryIeType;
    mandIe = (mandIe << 8) | 0; // recovery
    mandatoryIeSet.insert(mandIe);
}

EchoResponseMsg::~EchoResponseMsg()
{

}

bool EchoResponseMsg::encodeEchoResponseMsg(MsgBuffer &buffer,
                        EchoResponseMsgData
							const &data)
{
    bool rc = false;
    GtpV2IeHeader header;
    Uint16 startIndex = 0;
    Uint16 endIndex = 0;
    Uint16 length = 0;

    
    // Encode the Ie Header
    header.ieType = RecoveryIeType;
    header.instance = 0;
    header.length = 0; // We will encode the IE first and then update the length
    GtpV2Ie::encodeGtpV2IeHeader(buffer, header);
    startIndex = buffer.getCurrentIndex(); 
    RecoveryIe recovery=
    dynamic_cast<
    RecoveryIe&>(GtpV2IeFactory::getInstance().getIeObject(RecoveryIeType));
    rc = recovery.encodeRecoveryIe(buffer, data.recovery);
    endIndex = buffer.getCurrentIndex();
    length = endIndex - startIndex;
    
    // encode the length value now
    buffer.goToIndex(startIndex - 3);
    buffer.writeUint16(length, false);
    buffer.goToIndex(endIndex);

    if (!(rc))
    { 
        errorStream.add((char *)"Failed to encode IE: recovery\n");
        return false;
    }

    if (data.sendingNodeFeaturesIePresent)
    {
        
        // Encode the Ie Header
        header.ieType = NodeFeaturesIeType;
        header.instance = 0;
        header.length = 0; // We will encode the IE first and then update the length
        GtpV2Ie::encodeGtpV2IeHeader(buffer, header);
        startIndex = buffer.getCurrentIndex(); 
        NodeFeaturesIe sendingNodeFeatures=
        dynamic_cast<
        NodeFeaturesIe&>(GtpV2IeFactory::getInstance().getIeObject(NodeFeaturesIeType));
        rc = sendingNodeFeatures.encodeNodeFeaturesIe(buffer, data.sendingNodeFeatures);
        endIndex = buffer.getCurrentIndex();
        length = endIndex - startIndex;
        
        // encode the length value now
        buffer.goToIndex(startIndex - 3);
        buffer.writeUint16(length, false);
        buffer.goToIndex(endIndex);

        if (!(rc))
        { 
            errorStream.add((char *)"Failed to encode IE: sendingNodeFeatures\n");
            return false;
        }
    }
    return rc;

}

bool EchoResponseMsg::decodeEchoResponseMsg(MsgBuffer &buffer,
 EchoResponseMsgData 
 &data, Uint16 length)
{

    bool rc = false;
    GtpV2IeHeader ieHeader;
  
    set<Uint16> mandatoryIeLocalList = mandatoryIeSet;
    while (buffer.lengthLeft() > IE_HEADER_SIZE)
    {
        GtpV2Ie::decodeGtpV2IeHeader(buffer, ieHeader);
        if (ieHeader.length > buffer.lengthLeft())
        {
            // We do not have enough bytes left in the message for this IE
            errorStream.add((char *)"IE Length exceeds beyond message boundary\n");
            errorStream.add((char *)"  Offending IE Type: ");
            errorStream.add(ieHeader.ieType);
            errorStream.add((char *)"\n  Ie Length in Header: ");
            errorStream.add(ieHeader.length);
            errorStream.add((char *)"\n  Bytes left in message: ");
            errorStream.add(buffer.lengthLeft());
            errorStream.endOfLine();
            return false;
        }

        switch (ieHeader.ieType){
     
            case RecoveryIeType:
            {
                RecoveryIe ieObject =
                dynamic_cast<
                RecoveryIe&>(GtpV2IeFactory::getInstance().getIeObject(RecoveryIeType));

                if(ieHeader.instance == 0)
                {
                    rc = ieObject.decodeRecoveryIe(buffer, data.recovery, ieHeader.length);

                    if (!(rc))
                    {
                        errorStream.add((char *)"Failed to decode IE: recovery\n");
                        return false;
                    }
                }

                else
                {
                    // Unknown IE instance print error
                    errorStream.add((char *)"Unknown IE Type: ");
                    errorStream.add(ieHeader.ieType);
                    errorStream.endOfLine();
                    buffer.skipBytes(ieHeader.length);
                }
                break;
            }
     
            case NodeFeaturesIeType:
            {
                NodeFeaturesIe ieObject =
                dynamic_cast<
                NodeFeaturesIe&>(GtpV2IeFactory::getInstance().getIeObject(NodeFeaturesIeType));

                if(ieHeader.instance == 0)
                {
                    rc = ieObject.decodeNodeFeaturesIe(buffer, data.sendingNodeFeatures, ieHeader.length);

                    data.sendingNodeFeaturesIePresent = true;
                    if (!(rc))
                    {
                        errorStream.add((char *)"Failed to decode IE: sendingNodeFeatures\n");
                        return false;
                    }
                }

                else
                {
                    // Unknown IE instance print error
                    errorStream.add((char *)"Unknown IE Type: ");
                    errorStream.add(ieHeader.ieType);
                    errorStream.endOfLine();
                    buffer.skipBytes(ieHeader.length);
                }
                break;
            }

            default:
            {
                // Unknown IE print error
                errorStream.add((char *)"Unknown IE Type: ");
                errorStream.add(ieHeader.ieType);
                errorStream.endOfLine();
                buffer.skipBytes(ieHeader.length);
            }
        }
    }
    return rc; // TODO validations
}

void EchoResponseMsg::
displayEchoResponseMsgData_v(EchoResponseMsgData const &data, Debug &stream)
{
    stream.incrIndent();
    stream.add((char *)"EchoResponseMsg:");
    stream.endOfLine();
    stream.incrIndent();
        
    
    stream.add((char *)"IE - recovery:");
    stream.endOfLine();
    RecoveryIe recovery=
    dynamic_cast<
    RecoveryIe&>(GtpV2IeFactory::getInstance().getIeObject(RecoveryIeType));
    recovery.displayRecoveryIe_v(data.recovery, stream);

    if (data.sendingNodeFeaturesIePresent)
    {


        stream.add((char *)"IE - sendingNodeFeatures:");
        stream.endOfLine();
        NodeFeaturesIe sendingNodeFeatures=
        dynamic_cast<
        NodeFeaturesIe&>(GtpV2IeFactory::getInstance().getIeObject(NodeFeaturesIeType));
        sendingNodeFeatures.displayNodeFeaturesIe_v(data.sendingNodeFeatures, stream);

    }

    stream.decrIndent();
    stream.decrIndent();
}


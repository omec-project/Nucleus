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

#include "forwardAccessContextNotificationMsg.h"
#include "../ieClasses/manual/gtpV2Ie.h"
#include "../ieClasses/gtpV2IeFactory.h"
#include "../ieClasses/fContainerIe.h"

ForwardAccessContextNotificationMsg::ForwardAccessContextNotificationMsg()
{
    msgType = ForwardAccessContextNotificationMsgType;

}

ForwardAccessContextNotificationMsg::~ForwardAccessContextNotificationMsg()
{

}

bool ForwardAccessContextNotificationMsg::encodeForwardAccessContextNotificationMsg(MsgBuffer &buffer,
                        ForwardAccessContextNotificationMsgData
							const &data)
{
    bool rc = false;
    GtpV2IeHeader header;
    Uint16 startIndex = 0;
    Uint16 endIndex = 0;
    Uint16 length = 0;

    if (data.eUtranTransparentContainerIePresent)
    {
        
        // Encode the Ie Header
        header.ieType = FContainerIeType;
        header.instance = 0;
        header.length = 0; // We will encode the IE first and then update the length
        GtpV2Ie::encodeGtpV2IeHeader(buffer, header);
        startIndex = buffer.getCurrentIndex(); 
        FContainerIe eUtranTransparentContainer=
        dynamic_cast<
        FContainerIe&>(GtpV2IeFactory::getInstance().getIeObject(FContainerIeType));
        rc = eUtranTransparentContainer.encodeFContainerIe(buffer, data.eUtranTransparentContainer);
        endIndex = buffer.getCurrentIndex();
        length = endIndex - startIndex;
        
        // encode the length value now
        buffer.goToIndex(startIndex - 3);
        buffer.writeUint16(length, false);
        buffer.goToIndex(endIndex);

        if (!(rc))
        { 
            errorStream.add((char *)"Failed to encode IE: eUtranTransparentContainer\n");
            return false;
        }
    }
    return rc;

}

bool ForwardAccessContextNotificationMsg::decodeForwardAccessContextNotificationMsg(MsgBuffer &buffer,
 ForwardAccessContextNotificationMsgData 
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
     
            case FContainerIeType:
            {
                FContainerIe ieObject =
                dynamic_cast<
                FContainerIe&>(GtpV2IeFactory::getInstance().getIeObject(FContainerIeType));

                if(ieHeader.instance == 0)
                {
                    rc = ieObject.decodeFContainerIe(buffer, data.eUtranTransparentContainer, ieHeader.length);

                    data.eUtranTransparentContainerIePresent = true;
                    if (!(rc))
                    {
                        errorStream.add((char *)"Failed to decode IE: eUtranTransparentContainer\n");
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

void ForwardAccessContextNotificationMsg::
displayForwardAccessContextNotificationMsgData_v(ForwardAccessContextNotificationMsgData const &data, Debug &stream)
{
    stream.incrIndent();
    stream.add((char *)"ForwardAccessContextNotificationMsg:");
    stream.endOfLine();
    stream.incrIndent();
        
    
    if (data.eUtranTransparentContainerIePresent)
    {


        stream.add((char *)"IE - eUtranTransparentContainer:");
        stream.endOfLine();
        FContainerIe eUtranTransparentContainer=
        dynamic_cast<
        FContainerIe&>(GtpV2IeFactory::getInstance().getIeObject(FContainerIeType));
        eUtranTransparentContainer.displayFContainerIe_v(data.eUtranTransparentContainer, stream);

    }

    stream.decrIndent();
    stream.decrIndent();
}

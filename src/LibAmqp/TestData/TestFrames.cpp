/*
   Copyright 2011 StormMQ Limited

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */
#include <stdlib.h>
#include <string.h>
#include "libamqp.h"
#include "Buffer/Buffer.h"
#include "TestData/TestFrames.h"
namespace test_data
{
    test_data_def(sasl_protocol_1_0_0,
        'A', 'M', 'Q', 'P', AMQP_SASL_PROTOCOL_ID, 1, 0, 0
    );
    test_data_def(tls_protocol_1_0_0,
        'A', 'M', 'Q', 'P', AMQP_TLS_PROTOCOL_ID, 1, 0, 0
    );
    test_data_def(amqp_protocol_1_0_0,
        'A', 'M', 'Q', 'P', AMQP_PROTOCOL_ID, 1, 0, 0
    );

    test_data_def(sasl_protocol_1_1_0,
        'A', 'M', 'Q', 'P', AMQP_SASL_PROTOCOL_ID, 1, 1, 0
    );
    test_data_def(tls_protocol_1_1_0,
        'A', 'M', 'Q', 'P', AMQP_TLS_PROTOCOL_ID, 1, 1, 0
    );
    test_data_def(amqp_protocol_1_1_0,
        'A', 'M', 'Q', 'P', AMQP_PROTOCOL_ID, 1, 1, 0
    );

    test_data_def(minimal_frame_header,
        0, 0, 0, 8,
        2, 0, 0, 1
    );

    //SaslMechanisms(sasl­server­mechanisms=[PLAIN])
    test_data_def(sasl_mechanisms_frame,
            0x00, 0x00, 0x00, 0x15,
            0x02, 0x01, 0x00, 0x00,
            0x00,
                0x53, 0x40,
                0xC0, 0x08, 0x01, 0xA3, 0x05, 0x50, 0x4C, 0x41, 0x49, 0x4E
    );

    test_data_def(sasl_mechanisms_frame_long,
            0x00, 0x00, 0x00, 0x1b,
            0x02, 0x01, 0x00, 0x00,
            0x00,
                0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
                0xC0, 0x08, 0x01, 0xA3, 0x05, 0x50, 0x4C, 0x41, 0x49, 0x4E
    );

    test_data_def(sasl_mechanisms_frame_symbol,
            0x00, 0x00, 0x00, 0x2e,
            0x02, 0x01, 0x00, 0x00,
            0x00,
                0xa3, 0x19, 'a', 'm', 'q', 'p', ':', 's', 'a', 's', 'l', '-', 'm', 'e', 'c', 'h', 'a', 'n', 'i', 's', 'm', 's', ':', 'l', 'i', 's', 't',
                0xC0, 0x0c, 0x01, 0xA3, 0x09, 'A', 'N', 'O', 'N', 'Y', 'M', 'O', 'U', 'S'
    );



    // SaslInit(mechanism=PLAIN, initial­response=Binary, hostname=development.mysystem.mycompany.stormmq.com)
    test_data_def(SaslInit,
        0x00, 0x00, 0x01, 0x52,
        0x02, 0x01, 0x00, 0x00,
        0x00, 
	    0x53, 0x41,
            0xD0, 0x00, 0x00, 0x01, 0x42, 0x00, 0x00, 0x00, 0x03, 0xA3, 0x05, 0x50, 0x4C, 0x41, 0x49, 0x4E, 0xB0, 0x00, 0x00, 0x01, 0x06, 0x00, 0x72, 0x61, 0x70, 0x68, 0x00, 0x49, 0x33, 0x32, 0x5A, 0x55, 0x34, 0x36, 0x4F, 0x47, 0x5F, 0x5F, 0x43, 0x58, 0x6C, 0x58, 0x38, 0x51, 0x56, 0x31, 0x61, 0x62, 0x4D, 0x56, 0x64, 0x6C, 0x79, 0x64, 0x31, 0x57, 0x35, 0x45, 0x4B, 0x37, 0x36, 0x48, 0x37, 0x30, 0x4E, 0x56, 0x76, 0x4C, 0x54, 0x75, 0x45, 0x6D, 0x73, 0x43, 0x62, 0x4B, 0x41, 0x55, 0x2D, 0x43, 0x65, 0x79, 0x36, 0x6B, 0x69, 0x5F, 0x41, 0x4C, 0x75, 0x4B, 0x56, 0x48, 0x71, 0x43, 0x46, 0x45, 0x50, 0x4C, 0x52, 0x46, 0x33, 0x39, 0x34, 0x6C, 0x70, 0x72, 0x64, 0x57, 0x4B, 0x67, 0x37, 0x77, 0x7A, 0x75, 0x68, 0x66, 0x6A, 0x51, 0x79, 0x44, 0x54, 0x4A, 0x32, 0x46, 0x6E, 0x6F, 0x36, 0x6F, 0x58, 0x79, 0x5F, 0x4B, 0x5A, 0x43, 0x6D, 0x75, 0x5A, 0x2D, 0x31, 0x48, 0x76, 0x39, 0x52, 0x44, 0x63, 0x48, 0x65, 0x62, 0x53, 0x56, 0x35, 0x32, 0x57, 0x66, 0x31, 0x69, 0x50, 0x33, 0x46, 0x6B, 0x5F, 0x4C, 0x37, 0x70, 0x6C, 0x31, 0x31, 0x72, 0x5F, 0x57, 0x6E, 0x42, 0x31, 0x5F, 0x5F, 0x76, 0x6B, 0x69, 0x42, 0x69, 0x71, 0x35, 0x4E, 0x5F, 0x57, 0x65, 0x62, 0x71, 0x52, 0x51, 0x5A, 0x38, 0x30, 0x52, 0x4A, 0x63, 0x31, 0x55, 0x73, 0x63, 0x32, 0x52, 0x4E, 0x55, 0x71, 0x35, 0x72, 0x30, 0x45, 0x52, 0x4C, 0x73, 0x7A, 0x38, 0x7A, 0x42, 0x78, 0x66, 0x48, 0x31, 0x5F, 0x5F, 0x44, 0x42, 0x4B, 0x74, 0x59, 0x37, 0x70, 0x36, 0x69, 0x78, 0x64, 0x65, 0x41, 0x34, 0x34, 0x43, 0x6D, 0x36, 0x34, 0x72, 0x56, 0x50, 0x4B, 0x50, 0x63, 0x66, 0x78, 0x39, 0x72, 0x79, 0x4F, 0x43, 0x49, 0x31, 0x6A, 0x31, 0x66, 0x61, 0x41, 0x36, 0x77, 0x2D, 0x79, 0x5A, 0x6E, 0x53, 0x53, 0x4D, 0x53, 0x4A, 0x69, 0x5A, 0x6E, 0x64, 0x49, 0x4B, 0x4A, 0x6D, 0x76, 0x4A, 0x4E, 0xA1, 0x2A, 0x64, 0x65, 0x76, 0x65, 0x6C, 0x6F, 0x70, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x6D, 0x79, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x2E, 0x6D, 0x79, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x6E, 0x79, 0x2E, 0x73, 0x74, 0x6F, 0x72, 0x6D, 0x6D, 0x71, 0x2E, 0x63, 0x6F, 0x6D
    );

//SaslOutcome(code=SaslCode(ok, additional­data=:NULL:)
    test_data_def(frame_SaslOutcome,
        0x00, 0x00, 0x00, 0x10, 
	0x02, 0x01, 0x00, 0x00, 
	    0x00, 
	    0x53, 0x44, 
	    0xC0, 0x03, 0x01, 0x50, 0x00
    );

    // Open(container­id=localhost.unknown-process-name, hostname=development.mysystem.mycompany.stormmq.com, max­frame­size=512, channel­max=0x00FF, idle­time­out=15000, outgoing­locales=[], incoming­locales=[], offered­capabilities=[], desired­capabilities=[], properties={})
    test_data_def(Open,
        0x00, 0x00, 0x00, 0x6E,
        0x02, 0x00, 0x00, 0x00,
	    0x00, 
	    0x53, 0x10,
	    0xC0, 0x61, 0x0A, 0xA1, 0x1E, 0x6C, 0x6F, 0x63, 0x61, 0x6C, 0x68, 0x6F, 0x73, 0x74, 0x2E, 0x75, 0x6E, 0x6B, 0x6E, 0x6F, 0x77, 0x6E, 0x2D, 0x70, 0x72, 0x6F, 0x63, 0x65, 0x73, 0x73, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0xA1, 0x2A, 0x64, 0x65, 0x76, 0x65, 0x6C, 0x6F, 0x70, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x6D, 0x79, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x2E, 0x6D, 0x79, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x6E, 0x79, 0x2E, 0x73, 0x74, 0x6F, 0x72, 0x6D, 0x6D, 0x71, 0x2E, 0x63, 0x6F, 0x6D, 0x70, 0x00, 0x00, 0x02, 0x00, 0x60, 0x00, 0xFF, 0x70, 0x00, 0x00, 0x3A, 0x98, 0x40, 0x40, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

    //Open(container­id=\/mycompany\/mysystem\/development, hostname=:NULL:, max­frame­size=512, channel­max=0x00FF, idle­time­out=15000, outgoing­locales=[], incoming­locales=[], offered­capabilities=[], desired­capabilities=[], properties={})
    test_data_def(frame_Open,
        0x00, 0x00, 0x00, 0x44,
        0x02, 0x00, 0x00, 0x00,
	    0x00, 
	    0x53, 0x10,
	    0xC0, 0x37, 0x0A, 0xA1, 0x1F, 0x2F, 0x6D, 0x79, 0x63, 0x6F, 0x6D, 0x70, 0x61, 0x6E, 0x79, 0x2F, 0x6D, 0x79, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x2F, 0x64, 0x65, 0x76, 0x65, 0x6C, 0x6F, 0x70, 0x6D, 0x65, 0x6E, 0x74, 0x40, 0x70, 0x00, 0x00, 0x02, 0x00, 0x60, 0x00, 0xFF, 0x70, 0x00, 0x00, 0x3A, 0x98, 0x40, 0x40, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

    // Begin(remote­channel=:NULL:, next­outgoing­id=0, incoming­window=2147483647, outgoing­window=2147483647, handle­max=15, offered­capabilities=[], desired­capabilities=[], properties={})
    test_data_def(Begin,
        0x00, 0x00, 0x00, 0x21,
        0x02, 0x00, 0x00, 0x00,
	    0x00, 
	    0x53, 0x11,
	    0xC0, 0x14, 0x08, 0x40, 0x43, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x52, 0x0F, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

//Begin(remote­channel=0x0000, next­outgoing­id=0, incoming­window=2147483647, outgoing­window=2147483647, handle­max=15, offered­capabilities=[], desired­capabilities=[], properties={})
    test_data_def(frame_Begin,
        0x00, 0x00, 0x00, 0x23, 
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x11, 
            0xC0, 0x16, 0x08, 0x60, 0x00, 0x00, 0x43, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x52, 0x0F, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

    // Attach(name=sender-link-name, handle=0, role=sender, snd­settle­mode=SenderSettledMode(settled), rcv­settle­mode=ReceiverSettleMode(first), source=Source(address=client-sending-node, terminus­durability=TerminusDurability(unsettled-state), expiry­policy=never, timeout=0, dynamic=false, dynamic­node­properties=NodeProperties(unrecognised-size=0, lifetime-policy=:NULL:, supported-dist-modes=:NULL:), distribution­mode=move, filter={}, default­outcome=Rejected(error=:NULL:), outcomes=[amqp:accepted:list, amqp:released:list, amqp:rejected:list, amqp:modified:list], capabilities=[]), target=Target(address=:NULL:, terminus­durability=TerminusDurability(unsettled-state), expiry­policy=session-end, timeout=0, dynamic=true, dynamic­node­properties=NodeProperties(unrecognised-size=0, lifetime-policy=DeleteOnClose(), supported-dist-modes=CommonAmqpArray([copy, move])), capabilities=[]), unsettled={}, incomplete­unsettled=false, initial­delivery­count=0, max­message­size=0, offered­capabilities=[], desired­capabilities=[], properties={})
    test_data_def(reply_attach_frame,
        0x00, 0x00, 0x00, 0xFC, 
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x12, 
            0xC0, 0xEF, 0x0E, 0xA1, 0x10, 0x73, 0x65, 0x6E, 0x64, 0x65, 0x72, 0x2D, 0x6C, 0x69, 0x6E, 0x6B, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x43, 0x42, 0x50, 0x01, 0x40, 0x00, 0x53, 0x28, 0xC0, 0x7F, 0x0A, 0xA1, 0x13, 0x63, 0x6C, 0x69, 0x65, 0x6E, 0x74, 0x2D, 0x73, 0x65, 0x6E, 0x64, 0x69, 0x6E, 0x67, 0x2D, 0x6E, 0x6F, 0x64, 0x65, 0x52, 0x02, 0xA3, 0x05, 0x6E, 0x65, 0x76, 0x65, 0x72, 0x40, 0x40, 0x40, 0xA3, 0x04, 0x6D, 0x6F, 0x76, 0x65, 0xC1, 0x01, 0x00, 0x00, 0x53, 0x25, 0x45, 0xE0, 0x4E, 0x04, 0xA3, 0x12, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x61, 0x63, 0x63, 0x65, 0x70, 0x74, 0x65, 0x64, 0x3A, 0x6C, 0x69, 0x73, 0x74, 0x12, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x72, 0x65, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x64, 0x3A, 0x6C, 0x69, 0x73, 0x74, 0x12, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x72, 0x65, 0x6A, 0x65, 0x63, 0x74, 0x65, 0x64, 0x3A, 0x6C, 0x69, 0x73, 0x74, 0x12, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x6D, 0x6F, 0x64, 0x69, 0x66, 0x69, 0x65, 0x64, 0x3A, 0x6C, 0x69, 0x73, 0x74, 0x00, 0x53, 0x29, 0xC0, 0x43, 0x06, 0x40, 0x52, 0x02, 0x40, 0x40, 0x41, 0xC1, 0x3A, 0x04, 0xA3, 0x0F, 0x6C, 0x69, 0x66, 0x65, 0x74, 0x69, 0x6D, 0x65, 0x2D, 0x70, 0x6F, 0x6C, 0x69, 0x63, 0x79, 0x00, 0x53, 0x2B, 0x45, 0xA3, 0x14, 0x73, 0x75, 0x70, 0x70, 0x6F, 0x72, 0x74, 0x65, 0x64, 0x2D, 0x64, 0x69, 0x73, 0x74, 0x2D, 0x6D, 0x6F, 0x64, 0x65, 0x73, 0xE0, 0x0C, 0x02, 0xA3, 0x04, 0x63, 0x6F, 0x70, 0x79, 0x04, 0x6D, 0x6F, 0x76, 0x65, 0xC1, 0x01, 0x00, 0x40, 0x43, 0x40, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

//Attach(name=sender-link-name, handle=0, role=receiver, snd­settle­mode=SenderSettledMode(unsettled), rcv­settle­mode=ReceiverSettleMode(first), source=Source(address=client-sending-node, terminus­durability=TerminusDurability(unsettled-state), expiry­policy=never, timeout=0, dynamic=false, dynamic­node­properties=NodeProperties(unrecognised-size=0, lifetime-policy=:NULL:, supported-dist-modes=:NULL:), distribution­mode=move, filter={}, default­outcome=Rejected(error=:NULL:), outcomes=[amqp:accepted:list, amqp:released:list, amqp:rejected:list, amqp:modified:list], capabilities=[]), target=Target(address=localhost.unknown-process-name:sender-link-name, terminus­durability=TerminusDurability(unsettled-state), expiry­policy=session-end, timeout=0, dynamic=true, dynamic­node­properties=NodeProperties(unrecognised-size=0, lifetime-policy=DeleteOnClose(), supported-dist-modes=CommonAmqpArray([copy, move])), capabilities=[]), unsettled={}, incomplete­unsettled=false, initial­delivery­count=-1, max­message­size=0, offered­capabilities=[], desired­capabilities=[], properties={})
    test_data_def(frame_Attach,
        0x00, 0x00, 0x01, 0x32, 0x02, 0x00, 0x00, 0x00, 0x00, 0x53, 0x12, 0xD0, 0x00, 0x00, 0x01, 0x22, 0x00, 0x00, 0x00, 0x0E, 0xA1, 0x10, 0x73, 0x65, 0x6E, 0x64, 0x65, 0x72, 0x2D, 0x6C, 0x69, 0x6E, 0x6B, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x43, 0x41, 0x50, 0x00, 0x40, 0x00, 0x53, 0x28, 0xC0, 0x7F, 0x0A, 0xA1, 0x13, 0x63, 0x6C, 0x69, 0x65, 0x6E, 0x74, 0x2D, 0x73, 0x65, 0x6E, 0x64, 0x69, 0x6E, 0x67, 0x2D, 0x6E, 0x6F, 0x64, 0x65, 0x52, 0x02, 0xA3, 0x05, 0x6E, 0x65, 0x76, 0x65, 0x72, 0x40, 0x40, 0x40, 0xA3, 0x04, 0x6D, 0x6F, 0x76, 0x65, 0xC1, 0x01, 0x00, 0x00, 0x53, 0x25, 0x45, 0xE0, 0x4E, 0x04, 0xA3, 0x12, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x61, 0x63, 0x63, 0x65, 0x70, 0x74, 0x65, 0x64, 0x3A, 0x6C, 0x69, 0x73, 0x74, 0x12, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x72, 0x65, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x64, 0x3A, 0x6C, 0x69, 0x73, 0x74, 0x12, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x72, 0x65, 0x6A, 0x65, 0x63, 0x74, 0x65, 0x64, 0x3A, 0x6C, 0x69, 0x73, 0x74, 0x12, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x6D, 0x6F, 0x64, 0x69, 0x66, 0x69, 0x65, 0x64, 0x3A, 0x6C, 0x69, 0x73, 0x74, 0x00, 0x53, 0x29, 0xC0, 0x73, 0x06, 0xA1, 0x2F, 0x6C, 0x6F, 0x63, 0x61, 0x6C, 0x68, 0x6F, 0x73, 0x74, 0x2E, 0x75, 0x6E, 0x6B, 0x6E, 0x6F, 0x77, 0x6E, 0x2D, 0x70, 0x72, 0x6F, 0x63, 0x65, 0x73, 0x73, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A, 0x73, 0x65, 0x6E, 0x64, 0x65, 0x72, 0x2D, 0x6C, 0x69, 0x6E, 0x6B, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x52, 0x02, 0x40, 0x40, 0x41, 0xC1, 0x3A, 0x04, 0xA3, 0x0F, 0x6C, 0x69, 0x66, 0x65, 0x74, 0x69, 0x6D, 0x65, 0x2D, 0x70, 0x6F, 0x6C, 0x69, 0x63, 0x79, 0x00, 0x53, 0x2B, 0x45, 0xA3, 0x14, 0x73, 0x75, 0x70, 0x70, 0x6F, 0x72, 0x74, 0x65, 0x64, 0x2D, 0x64, 0x69, 0x73, 0x74, 0x2D, 0x6D, 0x6F, 0x64, 0x65, 0x73, 0xE0, 0x0C, 0x02, 0xA3, 0x04, 0x63, 0x6F, 0x70, 0x79, 0x04, 0x6D, 0x6F, 0x76, 0x65, 0xC1, 0x01, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

    // Flow(next­incoming­id=0, incoming­window=2147483647, next­outgoing­id=0, outgoing­window=2147483647, handle=0, delivery­count=0, link­credit=0, available=0, drain=false, echo=false, properties=FlowProperties(unrecognised-size=0, txn-id=:NULL:))
    test_data_def(flow_frame,
        0x00, 0x00, 0x00, 0x23, 
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x13, 
            0xC0, 0x16, 0x0B, 0x43, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x43, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x43, 0x43, 0x43, 0x43, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

//Flow(next­incoming­id=0, incoming­window=2147483647, next­outgoing­id=0, outgoing­window=2147483647, handle=0, delivery­count=0, link­credit=4096, available=0, drain=false, echo=false, properties=FlowProperties(unrecognised-size=0, txn-id=:NULL:))
    test_data_def(frame_Flow,
        0x00, 0x00, 0x00, 0x27, 
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x13, 
            0xC0, 0x1A, 0x0B, 0x43, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x43, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x43, 0x43, 0x70, 0x00, 0x00, 0x10, 0x00, 0x43, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

    // Flow(next­incoming­id=0, incoming­window=2147483647, next­outgoing­id=0, outgoing­window=2147483647, handle=0, delivery­count=0, link­credit=4096, available=0, drain=false, echo=false, properties=FlowProperties(unrecognised-size=0, txn-id=:NULL:))
    test_data_def(Flow,
        0x00, 0x00, 0x00, 0x27, 
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x13, 
            0xC0, 0x1A, 0x0B, 0x43, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x43, 0x70, 0x7F, 0xFF, 0xFF, 0xFF, 0x43, 0x43, 0x70, 0x00, 0x00, 0x10, 0x00, 0x43, 0x40, 0x40, 0xC1, 0x01, 0x00
    );

// Transfer(handle=0, delivery­id=0, delivery­tag=com.stormmq.amqp.connection.link.conveyancers.senders.SendSettledMutableDeliveryTag@283b18f, message­format=MessageFormat(OriginalAmqpMessageFormat, 0), settled=true, more=true, rcv­settle­mode=:NULL:, state=:NULL:, resume=false, aborted=false, batchable=false)
    test_data_def(Transfer_0,
        0x00, 0x00, 0x00, 0x2A, 
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x14, 0xC0, 0x08, 0x06, 0x43, 0x43, 0xA0, 0x00, 0x43, 0x41, 0x42, 0x00, 0x53, 0x70, 0xC0, 0x0B, 0x05, 0x40, 0x50, 0x09, 0x70, 0x26, 0x3A, 0x45, 0xB9, 0x40, 0x43, 0x00, 0x53, 0x75, 0xA0, 0x00
    );

// Transfer(handle=0, delivery­id=256, delivery­tag=com.stormmq.amqp.connection.link.conveyancers.senders.SendSettledMutableDeliveryTag@283b18f, message­format=MessageFormat(OriginalAmqpMessageFormat, 0), settled=true, more=true, rcv­settle­mode=:NULL:, state=:NULL:, resume=false, aborted=false, batchable=false)
    test_data_def(Transfer_id_256,
        0x00, 0x00, 0x00, 0x2E,
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x14, 0xC0, 0x0C, 0x06, 0x43, 0x70, 0x00, 0x00, 0x01, 0x00, 0xA0, 0x00, 0x43, 0x41, 0x42, 0x00, 0x53, 0x70, 0xC0, 0x0B, 0x05, 0x40, 0x50, 0x09, 0x70, 0x26, 0x3A, 0x45, 0xB9, 0x40, 0x43, 0x00, 0x53, 0x75, 0xA0, 0x00
    );

// Transfer(handle=0, delivery­id=677, delivery­tag=com.stormmq.amqp.connection.link.conveyancers.senders.SendSettledMutableDeliveryTag@283b18f, message­format=MessageFormat(OriginalAmqpMessageFormat, 0), settled=true, more=true, rcv­settle­mode=:NULL:, state=:NULL:, resume=false, aborted=false, batchable=false)
    test_data_def(Transfer,
        0x00, 0x00, 0x00, 0x2E,
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x14, 0xC0, 0x0C, 0x06, 0x43, 0x70, 0x00, 0x00, 0x02, 0xA5, 0xA0, 0x00, 0x43, 0x41, 0x42, 0x00, 0x53, 0x70, 0xC0, 0x0B, 0x05, 0x40, 0x50, 0x09, 0x70, 0x26, 0x39, 0xF9, 0x84, 0x40, 0x43, 0x00, 0x53, 0x75, 0xA0, 0x00
    );

//Close(error=Error(condition=amqp:connection:forced, description=Our idle time-out occurred (we have not received any frames in idle-time-out x 2), info={}))
    test_data_def(frame_Close,
        0x00, 0x00, 0x00, 0x82, 
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x18, 
            0xC0, 0x75, 0x01, 0x00, 0x53, 0x1D, 0xC0, 0x6F, 0x03, 0xA3, 0x16, 0x61, 0x6D, 0x71, 0x70, 0x3A, 0x63, 0x6F, 0x6E, 0x6E, 0x65, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x3A, 0x66, 0x6F, 0x72, 0x63, 0x65, 0x64, 0xA1, 0x51, 0x4F, 0x75, 0x72, 0x20, 0x69, 0x64, 0x6C, 0x65, 0x20, 0x74, 0x69, 0x6D, 0x65, 0x2D, 0x6F, 0x75, 0x74, 0x20, 0x6F, 0x63, 0x63, 0x75, 0x72, 0x72, 0x65, 0x64, 0x20, 0x28, 0x77, 0x65, 0x20, 0x68, 0x61, 0x76, 0x65, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x72, 0x65, 0x63, 0x65, 0x69, 0x76, 0x65, 0x64, 0x20, 0x61, 0x6E, 0x79, 0x20, 0x66, 0x72, 0x61, 0x6D, 0x65, 0x73, 0x20, 0x69, 0x6E, 0x20, 0x69, 0x64, 0x6C, 0x65, 0x2D, 0x74, 0x69, 0x6D, 0x65, 0x2D, 0x6F, 0x75, 0x74, 0x20, 0x78, 0x20, 0x32, 0x29, 0xC1, 0x01, 0x00
    );

    // Close(error=:NULL:)
    test_data_def(Close,
        0x00, 0x00, 0x00, 0x0C, 
        0x02, 0x00, 0x00, 0x00, 
            0x00, 
            0x53, 0x18, 
            0x45
    );

}

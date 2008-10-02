/*
** CDex - Open Source Digital Audio CD Extractor
**
** Copyright (C) 2006 - 2007 Georgy Berdyshev
** Copyright (C) 1999 Albert L. Faber
**
** http://cdexos.sourceforge.net/
** http://sourceforge.net/projects/cdexos 
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef _ASFERR_H
#define _ASFERR_H


#define STATUS_SEVERITY(hr)  (((hr) >> 30) & 0x3)


///////////////////////////////////////////////////////////////////////////
//
// Advanced Streaming Format (ASF) Errors (2000 - 2999)
//
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_NS                      0xD


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: ASF_E_BUFFEROVERRUN
//
// MessageText:
//
//  An attempt was made to seek or position past the end of a buffer.%0
//
#define ASF_E_BUFFEROVERRUN              0xC00D07D0L

//
// MessageId: ASF_E_BUFFERTOOSMALL
//
// MessageText:
//
//  The supplied input or output buffer was too small.%0
//
#define ASF_E_BUFFERTOOSMALL             0xC00D07D1L

//
// MessageId: ASF_E_BADLANGUAGEID
//
// MessageText:
//
//  The language ID was not found.%0
//
#define ASF_E_BADLANGUAGEID              0xC00D07D2L

//
// MessageId: ASF_E_NOPAYLOADLENGTH
//
// MessageText:
//
//  The multiple payload packet is missing the payload length.%0
//
#define ASF_E_NOPAYLOADLENGTH            0xC00D07DBL

//
// MessageId: ASF_E_TOOMANYPAYLOADS
//
// MessageText:
//
//  The packet contains too many payloads.%0
//
#define ASF_E_TOOMANYPAYLOADS            0xC00D07DCL

//
// MessageId: ASF_E_PACKETCONTENTTOOLARGE
//
// MessageText:
//
//  ASF_E_PACKETCONTENTTOOLARGE
//
#define ASF_E_PACKETCONTENTTOOLARGE      0xC00D07DEL

//
// MessageId: ASF_E_UNKNOWNPACKETSIZE
//
// MessageText:
//
//  Expecting a fixed packet size but min. and max. are not equal.%0
//
#define ASF_E_UNKNOWNPACKETSIZE          0xC00D07E0L

//
// MessageId: ASF_E_INVALIDHEADER
//
// MessageText:
//
//  ASF_E_INVALIDHEADER
//
#define ASF_E_INVALIDHEADER              0xC00D07E2L

//
// MessageId: ASF_E_NOCLOCKOBJECT
//
// MessageText:
//
//  The object does not have a valid clock object.%0
//
#define ASF_E_NOCLOCKOBJECT              0xC00D07E6L

//
// MessageId: ASF_E_UNKNOWNCLOCKTYPE
//
// MessageText:
//
//  ASF_E_UNKNOWNCLOCKTYPE
//
#define ASF_E_UNKNOWNCLOCKTYPE           0xC00D07EBL

//
// MessageId: ASF_E_OPAQUEPACKET
//
// MessageText:
//
//  An attempt was made to restore or access an opaque packet.%0
//
#define ASF_E_OPAQUEPACKET               0xC00D07EDL

//
// MessageId: ASF_E_WRONGVERSION
//
// MessageText:
//
//  ASF_E_WRONGVERSION
//
#define ASF_E_WRONGVERSION               0xC00D07EEL

//
// MessageId: ASF_E_OVERFLOW
//
// MessageText:
//
//  An attempt was made to store a value which was larger than then destination's maximum value.%0
//
#define ASF_E_OVERFLOW                   0xC00D07EFL

//
// MessageId: ASF_E_NOTFOUND
//
// MessageText:
//
//  The object was not found.%0
//
#define ASF_E_NOTFOUND                   0xC00D07F0L

//
// MessageId: ASF_E_OBJECTTOOBIG
//
// MessageText:
//
//  The object is too large to be processed in the requested manner.%0
//
#define ASF_E_OBJECTTOOBIG               0xC00D07F1L

//
// MessageId: ASF_E_UNEXPECTEDVALUE
//
// MessageText:
//
//  A value was not set as expected.%0
//
#define ASF_E_UNEXPECTEDVALUE            0xC00D07F2L

//
// MessageId: ASF_E_INVALIDSTATE
//
// MessageText:
//
//  The request is not valid in the object's current state.%0
//
#define ASF_E_INVALIDSTATE               0xC00D07F3L

//
// MessageId: ASF_E_NOLIBRARY
//
// MessageText:
//
//  This object does not have a valid library pointer; it has not been Init()'ed or it has been Shutdown().%0
//
#define ASF_E_NOLIBRARY                  0xC00D07F4L


///////////////////////////////////////////////////////////////////////////
//
// Advanced Streaming Format (ASF) Success Codes (2000 - 2999)
//

//
// MessageId: ASF_S_OPAQUEPACKET
//
// MessageText:
//
//  ASF_S_OPAQUEPACKET
//
#define ASF_S_OPAQUEPACKET               0x000D07F0L


///////////////////////////////////////////////////////////////////////////
//
// Advanced Streaming Format (ASF) Warnings (2000 - 2999)
//


#endif // _ASFERR_H


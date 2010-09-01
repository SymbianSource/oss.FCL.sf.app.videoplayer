/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Constants for EPG plugin.*
*/




#ifndef __LIVETVCONSTANTS_H__
#define __LIVETVCONSTANTS_H__

// INCLUDES
#include <e32std.h>
#include <e32const.h>
#include <e32def.h>

// CONSTANTS

_LIT8( KTagMpeg7Prefix, "mpeg7" );

// Channel data
_LIT8( KTagServiceInformationTable, "ServiceInformationTable" );
_LIT8( KTagServiceInformation, "ServiceInformation" );
	_LIT8( KAttrServiceId, "serviceId" );
_LIT8( KTagServiceInformationName, "Name" );
_LIT8( KTagServiceInformationLogo, "Logo" );
_LIT8( KTagServiceInformationLogoUri, "MediaUri" );
_LIT8( KTagServiceInformationServiceURL, "ServiceURL" );
_LIT8( KTagServiceDescription, "ServiceDescription" );
_LIT8( KTagServiceLanguage, "ServiceLanguage" );

// Program data
_LIT8( KTagProgramInformationTable, "ProgramInformationTable" );
_LIT8( KTagProgramInformation, "ProgramInformation" );
	_LIT8( KAttrProgramId, "programId" );
_LIT8( KTagBasicDescription, "BasicDescription" );
_LIT8( KTagProgramTitle, "Title" );
_LIT8( KTagProgramShortTitle, "ShortTitle" );
_LIT8( KTagProgramKeyword, "Keyword" );
_LIT8( KTagProgramSynopsis, "Synopsis" );

// Schedule data
_LIT8( KTagProgramLocationTable, "ProgramLocationTable" );
_LIT8( KTagSchedule, "Schedule" );
	_LIT8( KAttrServiceIdRef, "serviceIDRef" );
_LIT8( KTagScheduleEvent, "ScheduleEvent" );
_LIT8( KTagScheduleProgram, "Program" );
	_LIT8( KAttrScheduldeProgramId, "crid" );
_LIT8( KTagScheduleStartTime, "PublishedStartTime" );
_LIT8( KTagScheduleEndTime, "PublishedEndTime" );

// XML constants to be used in document fetched over http
// Channel
_LIT8( KTagChannel, "channel" );
_LIT8( KTagChannelId, "channelId" );
_LIT8( KTagChannelName, "channelName" );
_LIT8( KTagChannelLogoPath, "channelLogoPath" );
_LIT8( KTagChannelDescription, "channelDescription" );
_LIT8( KTagChannelURI, "channelUri" );
_LIT8( KTagChannelSDP, "channelSdp" );

_LIT8( KTagProgram, "program" );
_LIT8( KTagProgramId, "programId" );
_LIT8( KTagProgramChannelId, "programChannelId" );
_LIT8( KTagProgramStartTime, "programStartTime" );
_LIT8( KTagProgramEndTime, "programEndTime" );
_LIT8( KTagProgramURI, "uri" );
_LIT8( KTagProgramSDP, "sdp" );
_LIT8( KTagProgramGenre, "genre" );
_LIT8( KTagProgramName, "programName" );
_LIT8( KTagProgramDescription, "programDescription" );
_LIT8( KTagProgramLanguage, "programLanguage" );
_LIT8( KTagProgramParentRating, "parentRating" );

// Service discovery definitions
_LIT8( KTagServiceType, "Type" );
_LIT8( KTagServiceAddress, "HTTP" );
	_LIT8( KAttrServiceLocation, "Location" );
	
_LIT8( KServiceTypeLive, "Live" );
_LIT8( KServiceTypeCoD, "CoD" );

// User agent definition (probably needs to be changed in the future :)
_LIT8( KUserAgent, "Firefight wolf 0.01");

_LIT8( KAccept, "*/*");

_LIT8( KMimeType, "text/xml" );

_LIT( KConnecting, "Connecting");

_LIT( KHeaderReceived, "Header received.");

_LIT( KTransactionSuccessful, "Transaction Successful");

_LIT( KTransactionCancelled, "Transaction cancelled" );

_LIT( KTransactionFailed, "Transaction Failed");

_LIT( KTransactionComplete, "Transaction Complete");

_LIT( KBodyPartReceived, "%d bytes received... ");

_LIT( KBodyReceived,"Body received");


_LIT( KErrorStr, "Error: %d");

_LIT( KUnrecognisedEvent, "Unrecognised event: %d");

_LIT( KRunError, "MHFRunError: " );

// Path where fetched xml data is saved by plugin http client
_LIT( KIptvLiveXMLFilePath, "thomson.xml");

_LIT( KLiveTvEPGDatabaseName, "livetvepg.db" );

_LIT( KLiveTvIconFilePath, "ltvicon" );

#endif  // __LIVETVCONSTANTS_H__

// End of File

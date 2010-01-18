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
* Description:    Classes to provide carriers of channel and program data to*
*/



// USER INCLUDES
#include "CIptvEpgLatestEpgAvailable.h"
#include "IptvLiveLogger.h"

// ----------------------------------------------------------------------------
// CIptvEpgLatestEpgAvailable::CIptvEpgLatestEpgAvailable()
//
// Constructor
// ----------------------------------------------------------------------------
EXPORT_C CIptvEpgLatestEpgAvailable* CIptvEpgLatestEpgAvailable::NewL()
	{
	CIptvEpgLatestEpgAvailable* self = new (ELeave) CIptvEpgLatestEpgAvailable();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
CIptvEpgLatestEpgAvailable::CIptvEpgLatestEpgAvailable()
	{
	}

CIptvEpgLatestEpgAvailable::~CIptvEpgLatestEpgAvailable()
	{
	}

void CIptvEpgLatestEpgAvailable::ConstructL()
	{
	}

// Getter methods


// ----------------------------------------------------------------------------
// CIptvEpgLatestEpgAvailable::GetServiceId()
//
// Get program's service provider id.
// ----------------------------------------------------------------------------
EXPORT_C TUint32 CIptvEpgLatestEpgAvailable::GetServiceId() const
	{
	return iServiceId;
	}
	
// ----------------------------------------------------------------------------
// CIptvEpgLatestEpgAvailable::GetStartTime()
//
// Get program start time.
// ----------------------------------------------------------------------------
EXPORT_C TTime CIptvEpgLatestEpgAvailable::GetStartTime() const
	{
	return iStartTime;
	}
	
// ----------------------------------------------------------------------------
// CIptvEpgLatestEpgAvailable::SetServiceId()
//
// Set program's service provider id.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgLatestEpgAvailable::SetServiceId( const TUint32& aServiceId )
	{
	iServiceId = aServiceId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgLatestEpgAvailable::SetStartTime()
//
// Set program start time.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgLatestEpgAvailable::SetStartTime( const TTime& aStartTime )
	{
	iStartTime = aStartTime;
	}




// end of file

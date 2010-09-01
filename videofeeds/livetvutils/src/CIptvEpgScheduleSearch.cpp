/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class to provide carrier of program data to search view.*
*/



// USER INCLUDES
#include "CIptvEpgScheduleSearch.h"

// ----------------------------------------------------------------------------
// CIptvEpgScheduleSearch::CIptvEpgProgram()
//
// Constructor
// ----------------------------------------------------------------------------
//
EXPORT_C CIptvEpgScheduleSearch* CIptvEpgScheduleSearch::NewL()
	{
	CIptvEpgScheduleSearch* self = new (ELeave) CIptvEpgScheduleSearch();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CIptvEpgScheduleSearch::CIptvEpgScheduleSearch()
// ----------------------------------------------------------------------------
//
CIptvEpgScheduleSearch::CIptvEpgScheduleSearch() : iChannelId( 0 ),
	iProgramId( 0 ), iStartTime( 0 ), iEndTime( 0 )
	{
	}

// ----------------------------------------------------------------------------
// CIptvEpgScheduleSearch::~CIptvEpgScheduleSearch()
// ----------------------------------------------------------------------------
//
CIptvEpgScheduleSearch::~CIptvEpgScheduleSearch()
	{
	delete iProgramName;
	}

// ----------------------------------------------------------------------------
// CIptvEpgScheduleSearch::ConstructL()
// ----------------------------------------------------------------------------
//
void CIptvEpgScheduleSearch::ConstructL()
	{
	}

// ----------------------------------------------------------------------------
// CIptvEpgScheduleSearch::LinearOrderOfSchedulesByTime()
//
// Method that returns -1 if s1 is earlier than s2, 0 if equal and 1 if 
// s1 is later than s2
// ----------------------------------------------------------------------------
//
TInt CIptvEpgScheduleSearch::LinearOrderOfSchedulesByTime(
		const CIptvEpgScheduleSearch &s1, const CIptvEpgScheduleSearch &s2 )
	{
	return ( s1.iStartTime < s2.iStartTime ? -1 : ( s1.iStartTime == s2.iStartTime ? 0 : 1 ) );
	}

// ----------------------------------------------------------------------------
// CIptvEpgScheduleSearch::LinearOrderOfSchedulesByName()
//
// Method that returns -1 if s1 is earlier than s2, 0 if equal and 1 if 
// s1 is later than s2
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvEpgScheduleSearch::LinearOrderOfSchedulesByName(
		const CIptvEpgScheduleSearch &s1, const CIptvEpgScheduleSearch &s2 )
	{
	if ( s1.iProgramName && s2.iProgramName )
		{
		TInt result( s1.iProgramName->Des().Compare( s2.iProgramName->Des() ) );
		if ( result < 0 || result > 0 )
			{
			return result;
			}
		else
			{
			// names are equal, compare start times
			if ( s1.iStartTime < s2.iStartTime )
				{
				result = -1;
				}
			else if ( s1.iStartTime == s2.iStartTime )
				{
				result = 0;
				}
			else
				{
				result = 1;
				}
			}
		return result;
		}
	else
		{
		return 0;
		}
	
	}


// End of file

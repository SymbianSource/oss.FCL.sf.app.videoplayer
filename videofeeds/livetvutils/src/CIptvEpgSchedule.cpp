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
#include "CIptvEpgSchedule.h"
#include "CIptvEpgProgram.h"
#include "IptvLiveLogger.h"

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::CIptvEpgProgram()
//
// Constructor
// ----------------------------------------------------------------------------
EXPORT_C CIptvEpgSchedule* CIptvEpgSchedule::NewL()
	{
	CIptvEpgSchedule* self = new (ELeave) CIptvEpgSchedule();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
CIptvEpgSchedule::CIptvEpgSchedule() : iProgram( NULL )
	{
	}

CIptvEpgSchedule::~CIptvEpgSchedule()
	{
	delete iProgram;
	iProgram = NULL;
	}

void CIptvEpgSchedule::ConstructL()
	{
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::LinearOrderOfSchedulesByTime()
//
// Method that returns -1 if s1 is earlier than s2, 0 if equal and 1 if 
// s1 is later than s2
// ----------------------------------------------------------------------------
TInt CIptvEpgSchedule::LinearOrderOfSchedulesByTime(const CIptvEpgSchedule &s1, const CIptvEpgSchedule &s2) 
	{
	return ( s1.iStartTime < s2.iStartTime ? -1 : ( s1.iStartTime == s2.iStartTime ? 0 : 1 ) ) ;
	}

// Getter methods

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetProgramId()
//
// Get program id.
// ----------------------------------------------------------------------------
EXPORT_C TInt64 CIptvEpgSchedule::GetProgramId() 
	{
	return iProgramId;
	}
	
// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetChannelId()
//
// Get program's channel id.
// ----------------------------------------------------------------------------
EXPORT_C TInt64 CIptvEpgSchedule::GetChannelId()
	{
	return iChannelId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetServiceId()
//
// Get program's service provider id.
// ----------------------------------------------------------------------------
EXPORT_C TUint32 CIptvEpgSchedule::GetServiceId()
	{
	return iServiceId;
	}
	
// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetStartTime()
//
// Get program start time.
// ----------------------------------------------------------------------------
EXPORT_C TTime CIptvEpgSchedule::GetStartTime()
	{
	return iStartTime;
	}
	
// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetEndTime()
//
// Get program end time.
// ----------------------------------------------------------------------------

EXPORT_C TTime CIptvEpgSchedule::GetEndTime()
	{
	return iEndTime;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetURI()
//
// Get program URI.
// ----------------------------------------------------------------------------
EXPORT_C TPtrC CIptvEpgSchedule::GetURI()
	{
	TPtrC retval( KNullDesC().Ptr(), 0 );
	if ( iProgram ) 
		{
		retval.Set( iProgram->ProgramURI() );
		}
	return retval;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetSDP()
//
// Get program SDP.
// ----------------------------------------------------------------------------
EXPORT_C TPtrC CIptvEpgSchedule::GetSDP()
	{
	TPtrC retval( KNullDesC().Ptr(), 0 );
	if ( iProgram ) 
		{
		retval.Set( iProgram->ProgramSDP() );
		}
	return retval;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetGenre()
//
// Get program genre.
// ----------------------------------------------------------------------------
EXPORT_C TPtrC CIptvEpgSchedule::GetGenre()
	{
	TPtrC retval( KNullDesC().Ptr(), 0 );
	if ( iProgram ) 
		{
		retval.Set( iProgram->ProgramGenre() );
		}
	return retval;	
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetName()
//
// Get program name.
// ----------------------------------------------------------------------------
EXPORT_C TPtrC CIptvEpgSchedule::GetName()
	{
	TPtrC retval( KNullDesC().Ptr(), 0 );
	if ( iProgram ) 
		{
		retval.Set( iProgram->ProgramName() );
		}
	return retval;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetDescription()
//
// Get program description.
// ----------------------------------------------------------------------------
EXPORT_C TPtrC CIptvEpgSchedule::GetDescription()
	{
	TPtrC retval( KNullDesC().Ptr(), 0 );
	if ( iProgram ) 
		{
		retval.Set( iProgram->ProgramDescription() );
		}
	return retval;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetLanguage()
//
// Get program language.
// ----------------------------------------------------------------------------
EXPORT_C TPtrC CIptvEpgSchedule::GetLanguage()
	{
	TPtrC retval( KNullDesC().Ptr(), 0 );
	if ( iProgram ) 
		{
		retval.Set( iProgram->ProgramLanguage() );
		}
	return retval;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::GetParentalRating()
//
// Get program parental rating.
// ----------------------------------------------------------------------------
EXPORT_C TPtrC CIptvEpgSchedule::GetParentalRating()
	{
	TPtrC retval( KNullDesC().Ptr(), 0 );
	if ( iProgram ) 
		{
		retval.Set( iProgram->ProgramParentalRating() );
		}
	return retval;
	}

// Setter methods

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetProgramId()
//
// Set program id.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetProgramId( const TInt64& aProgramId )
	{
	iProgramId = aProgramId;	
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetChannelId()
//
// Set program's channel id.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetChannelId( const TInt64& aChannelId )
	{
	iChannelId = aChannelId;
	}
	
// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetServiceId()
//
// Set program's service provider id.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetServiceId( const TUint32& aServiceId )
	{
	iServiceId = aServiceId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetStartTime()
//
// Set program start time.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetStartTime( const TTime& aStartTime )
	{
	iStartTime = aStartTime;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetEndTime()
//
// Set program end time.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetEndTime( const TTime& aEndTime )
	{
	iEndTime = aEndTime;
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetURI()
//
// Set program URI.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetURIL( const TDesC& aURI )
	{
	if ( iProgram ) 
		{
		iProgram->SetProgramURI( aURI.AllocL() );
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetSDP()
//
// Set program SDP.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetSDPL( const TDesC& aSDP )
	{
	if ( iProgram ) 
		{
		iProgram->SetProgramSDP( aSDP.AllocL() );
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetGenre()
//
// Set program genre.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetGenreL( const TDesC& aGenre )
	{
	if ( iProgram ) 
		{
		iProgram->SetProgramGenre( aGenre.AllocL() );
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetName()
//
// Set program name.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetNameL( const TDesC& aName )
	{
	if ( iProgram ) 
		{
		iProgram->SetProgramName( aName.AllocL() );
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetDescription()
//
// Set program description.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetDescriptionL( const TDesC& aDescription )
	{
	if ( iProgram ) 
		{
		iProgram->SetProgramDescription( aDescription.AllocL() );
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetLanguage()
//
// Set program language.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetLanguageL( const TDesC& aLanguage )
	{
	if ( iProgram )
		{
		iProgram->SetProgramLanguage( aLanguage.AllocL() );
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgSchedule::SetParentalRating()
//
// Set program parental rating.
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgSchedule::SetParentalRatingL( const TDesC& aParentalRating )
	{
	if ( iProgram ) 
		{
		iProgram->SetProgramParentalRating( aParentalRating.AllocL() );
		}
	}


// end of file

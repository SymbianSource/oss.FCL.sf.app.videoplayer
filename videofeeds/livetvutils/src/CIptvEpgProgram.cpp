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
#include "CIptvEpgProgram.h"
#include "IptvLiveLogger.h"

// ----------------------------------------------------------------------------
// CIptvEpgProgram::NewL()
//
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CIptvEpgProgram* CIptvEpgProgram::NewL()
	{
	CIptvEpgProgram* self = new ( ELeave ) CIptvEpgProgram();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CIptvEpgProgram::~CIptvEpgProgram()
//
// Destructor
// ----------------------------------------------------------------------------
CIptvEpgProgram::~CIptvEpgProgram()
	{
	delete iURI;
	delete iSDP;
	delete iGenre;
	delete iName;
	delete iDescription;
	delete iLanguage;
	delete iParentalRating;
	}
	
// ----------------------------------------------------------------------------
// CIptvEpgProgram::CIptvEpgProgram()
//
// Constructor
// ----------------------------------------------------------------------------
CIptvEpgProgram::CIptvEpgProgram() : iServiceId( 0 ), iChannelId( 0 ), 
	iProgramId( 0 )
	{
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ConstructL()
//
// Symbian second phase constructor
// ----------------------------------------------------------------------------
void CIptvEpgProgram::ConstructL()
	{
	iURI = HBufC::NewL( 0 );
	iSDP = HBufC::NewL( 0 );
	iGenre = HBufC::NewL( 0 );
	iName = HBufC::NewL( 0 );
	iDescription = HBufC::NewL( 0 );
	iLanguage = HBufC::NewL( 0 );
	iParentalRating = HBufC::NewL( 0 );
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::LinearOrderOfProgramsById()
//
// Method that returns -1 if p1 is less than p2, 0 if equal and 1 if 
// p1 is more than p2
// ----------------------------------------------------------------------------
TInt CIptvEpgProgram::LinearOrderOfProgramsById( const CIptvEpgProgram &p1, 
													const CIptvEpgProgram &p2)
	{
	return ( p1.iProgramId < p2.iProgramId ? -1 : ( p1.iProgramId == p2.iProgramId ? 0 : 1 ) );
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::LinearOrderOfProgramsByName()
//
// Method that returns -1 if p1 is less than p2, 0 if equal and 1 if 
// p1 is more than p2
// ----------------------------------------------------------------------------
TInt CIptvEpgProgram::LinearOrderOfProgramsByName( const CIptvEpgProgram &p1, 
												   const CIptvEpgProgram &p2 )
	{
	return p1.ProgramName().Compare( p2.ProgramName() );
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetServiceId()
//
// Setter for iServiceId
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetServiceId( TUint32 aServiceId )
	{
	iServiceId = aServiceId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetChannelId()
//
// Setter for iChannelId
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetChannelId( TInt64 aChannelId )
	{
	iChannelId = aChannelId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetProgramId()
//
// Setter for iProgramId
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetProgramId( TInt64 aProgramId )
	{
	iProgramId = aProgramId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetProgramURI()
//
// Setter for iURI
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetProgramURI( HBufC* aURI )
	{
	if( aURI )
		{
		delete iURI;
		iURI = NULL;
		iURI = aURI;
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetProgramSDP()
//
// Setter for iSDP
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetProgramSDP( HBufC* aSDP )
	{
	if( aSDP )
		{
		delete iSDP;
		iSDP = NULL;
		iSDP = aSDP;
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetProgramGenre()
//
// Setter for iGenre
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetProgramGenre( HBufC* aGenre )
	{
	if( aGenre )
		{
		delete iGenre;
		iGenre = NULL;
		iGenre = aGenre;
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetProgramName()
//
// Setter for iName
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetProgramName( HBufC* aName )
	{
	if( aName )
		{
		delete iName;
		iName = NULL;
		iName = aName;
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetProgramDescription()
//
// Setter for iDescription
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetProgramDescription( HBufC* aDescription )
	{
	if( aDescription )
		{
		delete iDescription;
		iDescription = NULL;
		iDescription = aDescription;
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetProgramLanguage()
//
// Setter for iLanguage
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetProgramLanguage( HBufC* aLanguage )
	{
	if( aLanguage )
		{
		delete iLanguage;
		iLanguage = NULL;
		iLanguage = aLanguage;
		}
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::SetProgramParentalRating()
//
// Setter for iParentalRating
// ----------------------------------------------------------------------------
EXPORT_C void CIptvEpgProgram::SetProgramParentalRating( HBufC* aParentalRating )
	{
	if( aParentalRating )
		{
		delete iParentalRating;
		iParentalRating = NULL;
		iParentalRating = aParentalRating;
		}
	}

// Getters for the instance data
// ----------------------------------------------------------------------------
// CIptvEpgProgram::ServiceId()
//
// Getter for iServiceId
// ----------------------------------------------------------------------------
EXPORT_C TUint32 CIptvEpgProgram::ServiceId() const
	{
	return iServiceId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ChannelId()
//
// Getter for iChannelId
// ----------------------------------------------------------------------------
EXPORT_C TInt64 CIptvEpgProgram::ChannelId() const
	{
	return iChannelId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ProgramId()
//
// Getter for iProgramId
// ----------------------------------------------------------------------------
EXPORT_C TInt64 CIptvEpgProgram::ProgramId() const
	{
	return iProgramId;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ProgramURI()
//
// Getter for iURI
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CIptvEpgProgram::ProgramURI() const
	{
	return *iURI;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ProgramSDP()
//
// Getter for iSDP
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CIptvEpgProgram::ProgramSDP() const
	{
	return *iSDP;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ProgramGenre()
//
// Getter for iGenre
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CIptvEpgProgram::ProgramGenre() const
	{
	return *iGenre;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ProgramName()
//
// Getter for iName
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CIptvEpgProgram::ProgramName() const
	{
	return *iName;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ProgramDescription()
//
// Getter for iDescription
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CIptvEpgProgram::ProgramDescription() const
	{
	return *iDescription;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ProgramLanguage()
//
// Getter for iLanguage
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CIptvEpgProgram::ProgramLanguage() const
	{
	return *iLanguage;
	}

// ----------------------------------------------------------------------------
// CIptvEpgProgram::ProgramParentalRating()
//
// Getter for iParentalRating
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CIptvEpgProgram::ProgramParentalRating() const
	{
	return *iParentalRating;
	}

// End of file

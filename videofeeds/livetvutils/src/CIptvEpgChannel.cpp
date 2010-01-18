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
#include "CIptvEpgChannel.h"
#include "IptvLiveLogger.h"

// ----------------------------------------------------------------------------
// CIptvEpgChannel::NewL()
//
// Creates instance of CIptvEpgChannel.
// ----------------------------------------------------------------------------
EXPORT_C CIptvEpgChannel* CIptvEpgChannel::NewL()
	{
	LIVE_TV_TRACE1( _L("CIptvEpgChannel"));
	CIptvEpgChannel* self = new ( ELeave ) CIptvEpgChannel();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CIptvEpgChannel::~CIptvEpgChannel()
//
// Destructor
// ----------------------------------------------------------------------------
CIptvEpgChannel::~CIptvEpgChannel()
	{
	delete iChannelName;
	delete iLogoPath;
	delete iDescription;
	delete iURI;
	delete iSDP;
	delete iBitmap;
	}

// ----------------------------------------------------------------------------
// CIptvEpgChannel::CIptvEpgChannel()
//
// Constructor
// ----------------------------------------------------------------------------
CIptvEpgChannel::CIptvEpgChannel() : iOrder( 0 ), iBitmap( NULL )
	{
	}

// ----------------------------------------------------------------------------
// CIptvEpgChannel::ConstructL()
//
// Symbian second phase constructor
// ----------------------------------------------------------------------------
void CIptvEpgChannel::ConstructL()
	{
	iChannelName = HBufC::NewL( 0 );
	iLogoPath = HBufC::NewL( 0 );
	iDescription = HBufC::NewL( 0 );
	iURI = HBufC::NewL( 0 );
	iSDP = HBufC::NewL( 0 );
	}
	
// Setters

EXPORT_C void CIptvEpgChannel::SetChannelId( TInt64 aChannelId )
	{
	iChannelId = aChannelId;
	}

EXPORT_C void CIptvEpgChannel::SetServiceId( TUint32 aServiceId )
	{
	iServiceProviderId = aServiceId;
	}

EXPORT_C void CIptvEpgChannel::SetChannelName( HBufC* aChannelName )
	{
	if( aChannelName )
		{
		delete iChannelName;
		iChannelName = NULL;
		}
	iChannelName = aChannelName;
	}

EXPORT_C void CIptvEpgChannel::SetChannelLogoPath( HBufC* aLogoPath )
	{
	if( aLogoPath )
		{
		delete iLogoPath;
		iLogoPath = NULL;
		}
	iLogoPath = aLogoPath;
	}

EXPORT_C void CIptvEpgChannel::SetChannelDescription( HBufC* aDescription )
	{
	if( aDescription )
		{
		delete iDescription;
		iDescription = NULL;
		}
	iDescription = aDescription;
	}

EXPORT_C void CIptvEpgChannel::SetChannelURI( HBufC* aURI )
	{
	if( aURI )
		{
		delete iURI;
		iURI = NULL;
		}
	iURI = aURI;
	}

EXPORT_C void CIptvEpgChannel::SetChannelSDP( HBufC* aSDP )
	{
	if( aSDP )
		{
		delete iSDP;
		iSDP = NULL;
		}
	iSDP = aSDP;
	}

EXPORT_C void CIptvEpgChannel::SetChannelOrder( const TUint32 aOrder )
	{
	iOrder = aOrder;
	}

EXPORT_C void CIptvEpgChannel::SetChannelIconL( const CFbsBitmap& aBitmap )
	{
	delete iBitmap;
	iBitmap = NULL;
	// Create duplicate of aBitmap
	iBitmap = new ( ELeave ) CFbsBitmap;
	TInt err( iBitmap->Duplicate( aBitmap.Handle() ) );
	if ( err != KErrNone ) 
		{
		LIVE_TV_TRACE2(_L("CIptvEpgChannel::SetChannelIconL() FAILED: %d"), err);
		}
	User::LeaveIfError( err );
	}


// Getters

EXPORT_C TUint32 CIptvEpgChannel::ServiceId() const
	{
	return iServiceProviderId;
	}

EXPORT_C TInt64 CIptvEpgChannel::ChannelId() const
	{
	return iChannelId;
	}

EXPORT_C TDesC& CIptvEpgChannel::ChannelName() const
	{
	return *iChannelName;
	}

EXPORT_C TDesC& CIptvEpgChannel::ChannelLogoPath() const
	{
	return *iLogoPath;
	}

EXPORT_C TDesC& CIptvEpgChannel::ChannelDescription() const
	{
	return *iDescription;
	}

EXPORT_C TDesC& CIptvEpgChannel::ChannelURI() const
	{
	return *iURI;
	}

EXPORT_C TDesC& CIptvEpgChannel::ChannelSDP() const
	{
	return *iSDP;
	}

EXPORT_C TUint32 CIptvEpgChannel::ChannelOrder() const
	{
	return iOrder;
	}

EXPORT_C CFbsBitmap* CIptvEpgChannel::ChannelIcon()
	{
	return iBitmap;
	}
// ----------------------------------------------------------------------------
// CIptvEpgChannel::LinearOrderByOrderNum
// 
// ----------------------------------------------------------------------------
TInt CIptvEpgChannel::LinearOrderByOrderNum( const CIptvEpgChannel& aFirst, 
									 const CIptvEpgChannel& aSecond )
	{
	if ( aFirst.ChannelOrder() > 0 && aFirst.ChannelOrder() < KMaxTUint32 && 
		 aSecond.ChannelOrder() > 0 && aSecond.ChannelOrder() < KMaxTUint32 ) 
		{
		// ordered by iOrder 
		return ( aFirst.ChannelOrder() < aSecond.ChannelOrder() ? -1 : 
			 ( aFirst.ChannelOrder() == aSecond.ChannelOrder() ? 0 : 1 ) );
		}
	else if ( aFirst.ChannelOrder() == KMaxTUint32 && 
			  aSecond.ChannelOrder() == KMaxTUint32 )
		{
		// inactive channels are ordered by their name
		return aFirst.ChannelName().CompareC( aSecond.ChannelName() );	
		}
	else if ( aFirst.ChannelOrder() == 0 || aSecond.ChannelOrder() == 0 )
		{
		// order not set yet
		return ( aFirst.ChannelOrder() > aSecond.ChannelOrder() ? -1 : 
			 ( aFirst.ChannelOrder() == aSecond.ChannelOrder() ? 0 : 1 ) ) ;
		}
	else 
		{
		// ordered by iOrder 
		return ( aFirst.ChannelOrder() < aSecond.ChannelOrder() ? -1 : 
			 ( aFirst.ChannelOrder() == aSecond.ChannelOrder() ? 0 : 1 ) ) ;
		}
	}

//  End of File 

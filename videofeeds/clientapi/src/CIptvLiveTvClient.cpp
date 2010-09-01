/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implements LiveTv Client API.*
*/





// INCLUDE FILES
#include <e32svr.h>
#include <s32mem.h>
#include "IptvLiveLogger.h"

#include "IptvClientServerCommon.h"
#include "CIptvLiveTvClient.h"
#include "IptvServer.pan"

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvLiveTvClient::CIptvLiveTvClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CIptvLiveTvClient::CIptvLiveTvClient() : 
	iMsgPtr( (unsigned char*)0, 0 )
    {
    }

// -----------------------------------------------------------------------------
// CIptvLiveTvClient::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvLiveTvClient::ConstructL()
    {
    LIVE_TV_TRACE1( _L("CIptvLiveTvClient::ConstructL"));
    TInt error( iSession.Connect() );
    LIVE_TV_TRACE2( _L("iSession.Connect() returned %d"), error );
    if( error != KErrNone )
    	{
    	User::Leave( error );
    	}
    }

// -----------------------------------------------------------------------------
// CIptvLiveTvClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvLiveTvClient* CIptvLiveTvClient::NewL()
    {
	CIptvLiveTvClient* self = 
						new ( ELeave ) CIptvLiveTvClient();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// CIptvLiveTvClient::~CIptvLiveTvClient
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C CIptvLiveTvClient::~CIptvLiveTvClient()
    {
    iSession.Close();
    if ( iMsg ) 
    	{
    	delete iMsg;
    	}
    }
    
// -----------------------------------------------------------------------------
// CIptvLiveTvClient::UpdateEPG, asynchronous
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvLiveTvClient::UpdateEPG( TUint32 /*aServiceProviderId*/,
                                            TRequestStatus& aStatus )
    {
    LIVE_TV_TRACE1( _L("CIptvLiveTvClient::UpdateEPG()" ) );
    iSession.SendRequest( EIptvEngineLiveTvUpdateEPGReq, aStatus );
    }
    
// -----------------------------------------------------------------------------
// CIptvLiveTvClient::UpdateChannelOrder, synchronous
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLiveTvClient::UpdateChannelOrderL( const TUint32 aServiceId, 
													 const TInt64 aChannelId,
									 				 const TUint32 aOrder ) 
	{
	LIVE_TV_TRACE4(_L("CIptvLiveTvClient::UpdateChannelOrder() in, aChannelId: %Li, aServiceId: %u, aOrder:%u"), aChannelId, aServiceId, aOrder );
    delete iMsg;
    iMsg = NULL;
    
    iMsg = HBufC8::NewL( KIptvChannelOrderReqSize );
	iMsgPtr.Set( iMsg->Des() );
   	RDesWriteStream stream;
	CleanupClosePushL( stream );
	stream.Open( iMsgPtr );
   	stream.WriteUint32L( aServiceId );
	stream << aChannelId; // Can leave too. Does this work with TInt64?
    stream.WriteUint32L( aOrder );
	CleanupStack::PopAndDestroy( &stream ); // Closes stream	
	
	return iSession.SendRequest( EIptvEngineLiveTvUpdateChannelReq, iMsgPtr );
	}

// -----------------------------------------------------------------------------
// CIptvLiveTvClient::AddServiceIdL, asynchronous
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvLiveTvClient::AddServiceIdL( TUint32 aServiceId, 
												TRequestStatus& aStatus )
	{
    delete iMsg;
    iMsg = NULL;
    
    iMsg = HBufC8::NewL( KIptvSizeRequestSize );
	iMsgPtr.Set( iMsg->Des() );
    RDesWriteStream stream;
	CleanupClosePushL( stream );
	stream.Open( iMsgPtr );
	stream.WriteUint32L( aServiceId );
	CleanupStack::PopAndDestroy( &stream ); // closes stream

	iSession.SendRequest( EIptvEngineLiveTvContentServiceId, iMsgPtr, aStatus );			
	}

// -----------------------------------------------------------------------------
// CIptvLiveTvClient::SetIapL, synchronous
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CIptvLiveTvClient::SetIapL( TUint32 aIap )
	{
	TBuf8<KIptvSizeRequestSize> buf; 
	
    RDesWriteStream stream;
	CleanupClosePushL( stream );
    stream.Open( buf );
    stream.WriteUint32L( aIap );
    CleanupStack::PopAndDestroy( &stream ); // closes stream
	
    return iSession.SendRequest( EIptvEngineLiveTvSetIap, buf );
	}

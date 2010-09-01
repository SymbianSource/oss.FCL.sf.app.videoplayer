/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Streaming handler*
*/


// Version : %version: 8 %

#include <eikon.hrh>
#include <avkon.hrh>
#include <mpxmessage.h>
#include <mpxmessage2.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxplaybackutility.h>
#include <mpxcollectionmessage.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>

#include "IptvDebug.h"
#include "vcxnsstreaminghandler.h"
#include "vcxnscontent.h"
#include "vcxnsuiengine.h"
#include "vcxnscontentprovider.h"

// Get spec for this.
const TInt KMinIntervalFromBegin    = 1000; // 1 sec
const TInt KMinIntervalFromEnd      = 5000; // 5 sec

// -----------------------------------------------------------------------------
//   CVcxNsStreamingHandler::CVcxNsStreamingHandler
// -----------------------------------------------------------------------------
//
CVcxNsStreamingHandler::CVcxNsStreamingHandler( CVcxNsUiEngine& aUiEngine ) :
    iUiEngine ( aUiEngine )
    {
    }

// -----------------------------------------------------------------------------
//   CVcxNsStreamingHandler::NewL
// -----------------------------------------------------------------------------
//
CVcxNsStreamingHandler* CVcxNsStreamingHandler::NewL( CVcxNsUiEngine& aUiEngine )
    {
    CVcxNsStreamingHandler* self = new( ELeave) CVcxNsStreamingHandler( aUiEngine );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
//   CVcxNsStreamingHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::ConstructL");
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::ConstructL");
    }

// -----------------------------------------------------------------------------
//   CVcxNsStreamingHandler::~CVcxNsStreamingHandler
// -----------------------------------------------------------------------------
//
CVcxNsStreamingHandler::~CVcxNsStreamingHandler()
    {
    HandleCloseStream();
    
    if ( iPlaybackUtility )
        {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        }
    }

// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::HandlePlaybackMessage");
    
    if ( aError == KErrNone && aMessage )
        {
        TRAP_IGNORE( DoHandlePlaybackMessageL( *aMessage ) );
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::HandlePlaybackMessage");
    }

// ---------------------------------------------------------------------------
// Handle playback message.
// ---------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::DoHandlePlaybackMessageL( const CMPXMessage& aMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::DoHandlePlaybackMessageL");

    TMPXMessageId id( *aMessage.Value< TMPXMessageId >( KMPXMessageGeneralId ) );

    IPTVLOGSTRING2_LOW_LEVEL("CVcxNsStreamingHandler::DoHandlePlaybackMessageL TMPXMessageId = 0x%08x", id );

    if ( KMPXMessageGeneral == id )
        {
        TInt event = *aMessage.Value<TInt>( KMPXMessageGeneralEvent );
        
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsStreamingHandler::DoHandlePlaybackMessageL: event = %d", event );
        
        switch ( event )
        {
            case TMPXPlaybackMessage::EPropertyChanged:
                {
                HandlePropertyChangedL( aMessage );
                }
                break;
            case TMPXPlaybackMessage::EStateChanged:
                {
                HandleStateChangedL( aMessage );
                }
                break;
            default:
                break;
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::DoHandlePlaybackMessageL");
    }

// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::HandleStateChangedL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::HandleStateChangedL( const CMPXMessage& aMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::HandleStateChangedL");
    
    if ( iContent && 
         aMessage.IsSupported( KMPXMessageGeneralType ) )
        {
        iCurrentState = *aMessage.Value<TInt>( KMPXMessageGeneralType );
        
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsStreamingHandler::HandleStateChangedL: state = %d", iCurrentState );
       
        switch ( iCurrentState )
            {
            case EPbStateInitialised:
                {
                if ( iLastPosPending )
                    {
                    SeekToLastPosL();
                    }
                }
                break;
            case EPbStateStopped:
            case EPbStateShuttingDown:
                {
                HandleCloseStream();
                }
                break;
            case EPbStatePlaying:
                {
                GetDurationL();
                }
                break;
            default:
                break;
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::HandleStateChangedL");
    }
    
// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::HandlePropertyChangedL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::HandlePropertyChangedL( const CMPXMessage& aMessage )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::HandlePropertyChangedL");
    
    if( iContent &&
        aMessage.IsSupported( KMPXMessageGeneralType ) &&
        aMessage.IsSupported( KMPXMessageGeneralData ) )
        {
        TInt property = *aMessage.Value<TInt>( KMPXMessageGeneralType );
        TInt data = *aMessage.Value<TInt>( KMPXMessageGeneralData );
        
        if( property == EPbPropertyPosition )
            {
            IPTVLOGSTRING2_LOW_LEVEL("CVcxNsStreamingHandler::HandlePropertyChangedL(): pos = %d", data );

            iContent->SetLastPlaybackPosition( TReal32 ( data ),
                                               CVcxNsContent::EVcxContentTypeStream );
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::HandlePropertyChangedL");
    }

// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::HandleOpenStreamL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::HandleOpenStreamL( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::HandleOpenStreamL");
    
    HandleCloseStream();
    
    if ( !iPlaybackUtility )
        {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeDefault );
        iPlaybackUtility->AddObserverL( *this );    
        }
    
    iContent = CVcxNsContent::NewL( *aContent );
    
    TInt prevPos = iContent->GetLastPlaybackPosition( CVcxNsContent::EVcxContentTypeStream );
    
    if ( prevPos > KMinIntervalFromBegin )
        {
        iLastPosPending = ETrue;
        iResumePos = prevPos;
        }
    else
        {
        iLastPosPending = EFalse;
        iResumePos = 0;
        }

    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::HandleOpenStreamL");
    }

// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::HandleCloseStream
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::HandleCloseStream()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::HandleCloseStream");
    
    TRAP_IGNORE( StoreLastPosL() );

    delete iContent;
    iContent = NULL;
    
    iLastPosPending = EFalse;
    iResumePos      = 0;
    iDuration       = 0;
    iCurrentState   = EPbStateNotInitialised;
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::HandleCloseStream");
    }

// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::SeekToLastPosL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::SeekToLastPosL()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::SeekToLastPosL");
    
    iLastPosPending = EFalse;
    
    if ( iResumePos != 0 )
        {
        iPlaybackUtility->SetL( EPbPropertyPosition, iResumePos );
        iResumePos = 0;
        }

    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::SeekToLastPosL");
    }

// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::StoreLastPosL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::StoreLastPosL()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::StoreLastPosL");
    
    if( iContent && iUiEngine.ContentProvider() )
        {
        TInt pos = iContent->GetLastPlaybackPosition( CVcxNsContent::EVcxContentTypeStream );
        
        if ( pos <= iDuration - KMinIntervalFromEnd && pos >= KMinIntervalFromBegin )
            {
            iUiEngine.ContentProvider()->StoreLastPlayPosL( iContent, 
                                                            CVcxNsContent::EVcxContentTypeStream,
                                                            TReal32( pos ) );
            }
        else
            {
            iUiEngine.ContentProvider()->StoreLastPlayPosL( iContent, 
                                                            CVcxNsContent::EVcxContentTypeStream,
                                                            TReal32 ( 0 ) );
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::StoreLastPosL");
    }


// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::GetDurationL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::GetDurationL()
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::GetDurationL");
    
    iPlaybackUtility->PropertyL( *this, EPbPropertyDuration );
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::GetDurationL");
    }

// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::HandlePropertyL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::HandlePropertyL( TMPXPlaybackProperty aProperty,
                                              TInt aValue, 
                                              TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL(">>>CVcxNsStreamingHandler::HandlePropertyL");
    
    if ( aError == KErrNone &&
         aProperty == EPbPropertyDuration )
        {
        iDuration = aValue;
        }
    
    IPTVLOGSTRING_LOW_LEVEL("<<<CVcxNsStreamingHandler::HandlePropertyL");
    }

// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::HandleSubPlayerNamesL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::HandleSubPlayerNamesL( TUid /*aPlayer*/,
                                                    const MDesCArray* /*aSubPlayers*/,
                                                    TBool /*aComplete*/,
                                                    TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsStreamingHandler::HandleSubPlayerNamesL");
    }

// -------------------------------------------------------------------------------------------------
//   CVcxNsStreamingHandler::HandleMediaL
// -------------------------------------------------------------------------------------------------
//
void CVcxNsStreamingHandler::HandleMediaL( const CMPXMedia& /*aProperties*/,
                                           TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsStreamingHandler::HandleMediaL");
    }

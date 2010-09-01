/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Content update progress handling*
*/




// INCLUDE FILES
#include <centralrepository.h>
#include "CIptvUtil.h"
#include "CIptvTimer.h"
#include "IptvDebug.h"
#include "vcxnsupdateprogresshandler.h"
#include "vcxnsserviceprovider.h"
#include "vcxnsuiengine.h"

//CONSTANTS 
const TInt KVcxNsProgCompletedNoThumbnails = 10;
const TInt KVcxNsProgStartedNoThumbnails = 5;
const TInt KVcxNsProgStartedThumbnails = 3;
const TInt KVcxNsThumbProgressPercentVal = 3; 
const TInt KVcxNsProgressUpdateStarted = 5;
const TInt KVcxNsProgressUpdateCompletedNoThumb = 70;
const TInt KVcxNsProgressUpdateCompleted = 40;
const TInt KVcxNsProgressUpdateSucceeded = 99;
const TInt KVcxNsProgressTimerDefault = 200000;  // 0,2 sec

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxNsUpdateProgressHandler::CVcxNsUpdateProgressHandler()
// -----------------------------------------------------------------------------
//
CVcxNsUpdateProgressHandler::CVcxNsUpdateProgressHandler( 
    CVcxNsUiEngine& aUiEngine ) 
:   iUiEngine( aUiEngine ),
    iThumbnails ( EFalse ),
    iPreviousProgress ( 0 ),
    iProgressPerThumbnail ( 0 ),
    iThumbnailsInPercent ( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsUpdateProgressHandler::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsUpdateProgressHandler* CVcxNsUpdateProgressHandler::NewL( 
    CVcxNsUiEngine& aUiEngine )
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "UI Engine ## CVcxNsUpdateProgressHandler::NewL()");
    return new (ELeave) CVcxNsUpdateProgressHandler( aUiEngine );
    }

// -----------------------------------------------------------------------------
// CVcxNsUpdateProgressHandler::~CVcxNsUpdateProgressHandler()
// -----------------------------------------------------------------------------
//
CVcxNsUpdateProgressHandler::~CVcxNsUpdateProgressHandler()
    {
    if ( iProgressTimer && iProgressTimer->IsActive() )
        {
        iProgressTimer->Cancel();
        }
    
    delete iProgressTimer;
    }

// -----------------------------------------------------------------------------
// CVcxNsUpdateProgressHandler::RegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxNsUpdateProgressHandler::RegisterObserver(
    MVcxNsUpdateProgressObserver* aObserver )
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "UI Engine ## CVcxNsUpdateProgressHandler::RegisterObserver()");
    iProgressObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CVcxNsUpdateProgressHandler::DeRegisterObserver()
// -----------------------------------------------------------------------------
//
void CVcxNsUpdateProgressHandler::DeRegisterObserver(
    MVcxNsUpdateProgressObserver* aObserver )
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "UI Engine ## CVcxNsUpdateProgressHandler::DeRegisterObserver()");

    if ( aObserver == iProgressObserver )
        {
        iProgressObserver = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsUpdateProgressHandler::GetProgress
// -----------------------------------------------------------------------------
//
TInt CVcxNsUpdateProgressHandler::GetProgress( TUint32 aServiceId )
    {
    if ( aServiceId == iCurrentServiceId )
        {
        IPTVLOGSTRING2_HIGH_LEVEL(
            "UI Engine ## CVcxNsUpdateProgressHandler::GetProgress( %d )", 
            iPreviousProgress );
        return iPreviousProgress;
        }
   
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// Handle service update status
// -----------------------------------------------------------------------------
//
void CVcxNsUpdateProgressHandler::HandleUpdateStatusChangeL( 
    TUint32 aServiceId,
    TInt aUpdateStatusMsg,
    TInt aInfo )
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "UI Engine ## CVcxNsUpdateProgressHandler::HandleUpdateStatusChangeL()");

    TInt progress( iPreviousProgress );
    iCurrentServiceId = aServiceId;
    iCurrentStatus = aUpdateStatusMsg; 
    
    if ( iProgressTimer && iProgressTimer->IsActive() )
        {
        iProgressTimer->Cancel();
        }
    
    switch ( aUpdateStatusMsg )
        {
        case KIptvVodUpdateStarted:
            {
            //Update started: progress 5% 
            progress = KVcxNsProgressUpdateStarted;
            TRAP_IGNORE( iThumbnails = ThumbnailsAllowedL() );
            StartProgressUpdateTimerL( );
            }
            break;
        case KIptvContentUpdateCompleted:
            {
            if ( iThumbnails )
                {
                //Update completed, thumbnails: progress 40%
                if ( progress < KVcxNsProgressUpdateCompleted )
                    {
                    progress = KVcxNsProgressUpdateCompleted;
                    }
                }
            else 
                {
                if ( progress < KVcxNsProgressUpdateCompletedNoThumb )
                    {
                    //Update completed, no thumbnails: progress 70%
                    progress = KVcxNsProgressUpdateCompletedNoThumb;
                    }
                StartProgressUpdateTimerL();
                }            
            }
            break;
        case KIptvThumbnailCount:
            {
            if ( aInfo > 0 && iThumbnails )
                {
                TInt thumbnailPercentage = 
                    ( KVcxNsProgressUpdateSucceeded - KVcxNsProgressUpdateCompleted ); 
                
                iProgressPerThumbnail = ( thumbnailPercentage / aInfo );
                
                //If there's more thumbnails than percents left:
                if ( iProgressPerThumbnail == 0 )
                    {
                    iThumbnailsInPercent = ( aInfo / thumbnailPercentage );
                    }
                }
            }
            break;
        case KIptvServiceThumbnailDownloaded:
        case KIptvContentThumbnailDownloaded:
            {
            if ( iProgressPerThumbnail > 0 )
                {
                progress += iProgressPerThumbnail;
                }
            else
                {
                iThumbnailCounter++;
                if ( iThumbnailCounter == 
                       ( iThumbnailsInPercent * KVcxNsThumbProgressPercentVal ) )
                    {
                    //When downloaded enough for increasing progress,
                    //zero thumbnail counter 
                    progress += KVcxNsThumbProgressPercentVal;
                    iThumbnailCounter = 0;
                    }
                }
            }
            break;
        case KIptvErrorEpgUpdateSuccessed:
            //Update succeeded: progress 99%
            progress = KVcxNsProgressUpdateSucceeded;
            break;
        
        case KIptvErrorEpgUpdateFailed:
        case KIptvErrorEpgUpdateStopped:    
            {
            progress = 0;
            iPreviousProgress = 0;
            iProgressPerThumbnail = 0;
            iThumbnailsInPercent = 0;
            iThumbnailCounter = 0;
            }
            break;
        default:
            break;
        }
    
    if ( progress > 0 && progress > iPreviousProgress )
        {
        //Refresh only if any progress
        RefreshProgressL( progress );
        }
    }

// --------------------------------------------------------------------------
// Timer expired
// --------------------------------------------------------------------------
//
void CVcxNsUpdateProgressHandler::TimerExpired( CIptvTimer* /*aTimer*/ )
    {
    TInt progress ( iPreviousProgress );
    TInt timeout ( KVcxNsProgressTimerDefault );
    
    if ( iThumbnails &&
            progress < KVcxNsProgressUpdateCompleted &&
            iCurrentStatus == KIptvVodUpdateStarted )
        {
        progress += KVcxNsProgStartedThumbnails; 
        }
    else if ( !iThumbnails && 
              progress < KVcxNsProgressUpdateCompletedNoThumb &&
              iCurrentStatus == KIptvVodUpdateStarted )
        {
        progress += KVcxNsProgStartedNoThumbnails;
        }
    else if ( !iThumbnails && 
              iCurrentStatus == KIptvContentUpdateCompleted )
        {
        progress += KVcxNsProgCompletedNoThumbnails;
        }
    
    if ( progress > iPreviousProgress )
        {
        TRAP_IGNORE( RefreshProgressL( progress ) );
        }

    if ( iProgressTimer )
        {
        if ( iProgressTimer ->IsActive() )
            {
            iProgressTimer->Cancel();
            }
        iProgressTimer->After( timeout );
        }
    }

// --------------------------------------------------------------------------
// Cancel progress handler timer
// --------------------------------------------------------------------------
//
void CVcxNsUpdateProgressHandler::Cancel()
    {
    if ( iProgressTimer && iProgressTimer->IsActive() )
        {
        IPTVLOGSTRING_HIGH_LEVEL(
            "UI Engine ## CVcxNsUpdateProgressHandler::Cancel()");
        iProgressTimer->Cancel();
        }
    }

// --------------------------------------------------------------------------
// Check if thumbnails are allowed
// --------------------------------------------------------------------------
//
TBool CVcxNsUpdateProgressHandler::ThumbnailsAllowedL() const
    {
    TBool thumbnails( EFalse );
    CRepository* cenRep = CRepository::NewL( KIptvCenRepUid );
    if ( cenRep )
        {
        TInt value;
        TInt error = cenRep->Get( KIptvCenRepVoDThumbnailsKey, value );
        if ( error == KErrNone && value == 1 )
            {
            thumbnails = ETrue;
            }
        }
    
    delete cenRep;
    return thumbnails;
    }

// --------------------------------------------------------------------------
// Check progress
// --------------------------------------------------------------------------
//
void CVcxNsUpdateProgressHandler::StartProgressUpdateTimerL( )
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "UI Engine ## CVcxNsUpdateProgressHandler::StartProgressUpdateTimerL()");
    
    if ( iProgressTimer && iProgressTimer->IsActive())
        {
        iProgressTimer->Cancel();
        }
    
    if ( !iProgressTimer )
        {
        iProgressTimer = CIptvTimer::NewL( CActive::EPriorityUserInput, *this );
        }

    iProgressTimer->After( KVcxNsProgressTimerDefault );
    }

// --------------------------------------------------------------------------
// Refresh progress
// --------------------------------------------------------------------------
//
void CVcxNsUpdateProgressHandler::RefreshProgressL( TInt aProgress )
    {
    IPTVLOGSTRING2_HIGH_LEVEL(
        "UI Engine ## CVcxNsUpdateProgressHandler::RefreshProgressL( %d )", 
        aProgress );

    if ( aProgress > KVcxNsProgressUpdateSucceeded )
        {
        aProgress = KVcxNsProgressUpdateSucceeded;
        }

    iPreviousProgress = aProgress;

    CVcxNsServiceProvider* serviceProvider = iUiEngine.ServiceProvider();
    if ( serviceProvider && aProgress > 0 )
        {
        TInt index = serviceProvider->ServiceIndex( iCurrentServiceId );
        if ( iProgressObserver )
            {
            iProgressObserver->HandleFeedUpdateProgressL( index, aProgress );
            }
        }
    }

/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "VCXMyVideosTestDlWatcher.h"
#include "MVCXMyVideosCollectionPluginTesterObserver.h"
#include "VCXMyVideosTestCommon.h"
#include "VCXTestStatsKeeper.h"

#include <BAUTILS.H>
#include <HttpDownloadMgrCommon.h>

#include "VCXTestLog.h"

// CONSTANTS
_LIT(KEVcxMyVideosDlStateNone, "EVcxMyVideosDlStateNone");
_LIT(KEVcxMyVideosDlStateDownloading, "EVcxMyVideosDlStateDownloading");
_LIT(KEVcxMyVideosDlStateFailed, "EVcxMyVideosDlStateFailed");
_LIT(KEVcxMyVideosDlStatePaused, "EVcxMyVideosDlStatePaused");
_LIT(KEVcxMyVideosDlStateDownloaded, "EVcxMyVideosDlStateDownloaded");

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::NewL
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDownload* CVCXMyVideosTestDownload::NewL()
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDownload::NewL");
    CVCXMyVideosTestDownload* self = new (ELeave) CVCXMyVideosTestDownload();
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CVCXMyVideosTestDownload::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDownload::CVCXMyVideosTestDownload
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDownload::CVCXMyVideosTestDownload()
    {
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDownload::~CVCXMyVideosTestDownload
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDownload::~CVCXMyVideosTestDownload( )
    {
    delete iTitle;
    iTitle = NULL;
    delete iUrl;
    iUrl = NULL;
    delete iPath;
    iPath = NULL;
    delete iUserName;
    iUserName = NULL;
    delete iPassword;
    iPassword = NULL;
    delete iPath;
    iPath = NULL;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDownload::ConstructL( )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDownload::ConstructL");
    VCXLOGLO1("<<<CVCXMyVideosTestDownload::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::NewL
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDlWatcher* CVCXMyVideosTestDlWatcher::NewL(
            MVCXMyVideosCollectionPluginTesterObserver* aObserver, CVCXTestStatsKeeper* aStatsKeeper )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::NewL");
    CVCXMyVideosTestDlWatcher* self = new (ELeave) CVCXMyVideosTestDlWatcher( aStatsKeeper );
    CleanupStack::PushL(self);
    self->ConstructL( aObserver );
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::CVCXMyVideosTestDlWatcher
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDlWatcher::CVCXMyVideosTestDlWatcher( CVCXTestStatsKeeper* aStatsKeeper )
  : iStats( aStatsKeeper )
    {

    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::~CVCXMyVideosTestDlWatcher
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDlWatcher::~CVCXMyVideosTestDlWatcher( )
    {
    iDownloads.ResetAndDestroy();
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::ConstructL( MVCXMyVideosCollectionPluginTesterObserver* aObserver )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::ConstructL");
    iObserver = aObserver;
    User::LeaveIfError( iFs.Connect() );
    iDlCheckDisabled = ETrue;
    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::CreateDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::CreateDownloadL( TInt aIapId, TInt aServiceId, TInt aContentId, const TDesC& aUrl, TBool aSync, const TDesC& aUserName, const TDesC& aPassword )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::CreateDownloadL");

    VCXLOGLO3("CVCXMyVideosTestDlWatcher:: aServiceId: %d, aContentId: %d", aServiceId, aContentId);
    PrintUrl( _L("CVCXMyVideosTestDlWatcher:: aUrl:"), aUrl );

    CVCXMyVideosTestDownload* dl = CVCXMyVideosTestDownload::NewL();
    iDownloads.Append( dl );

    CleanupDownloadsL( aServiceId, aContentId, aUrl );
    
    dl->iState = EVcxMyVideosDlStateNone;
    dl->iSyncCall = aSync;
    dl->iUrl = aUrl.AllocL();
    dl->iIapId = aIapId;
    dl->iUserName = aUserName.AllocL();
    dl->iPassword = aPassword.AllocL();
    dl->iServiceId = aServiceId;
    dl->iContentId = aContentId;

    iDlCheckDisabled = EFalse;
    
    TRAP_IGNORE( iStats->ActionStartL( aUrl, _L("Download "), ETrue ) );

    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::CreateDownloadL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::StartDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::StartDownloadL( const TDesC& aUrl, const TDesC& aPath, TInt aDownloadId, TInt aMpxId,
        const TDesC& aTitle, TVcxMyVideosDownloadState aState, TInt aProgress )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::StartDownloadL");

    VCXLOGLO3("CVCXMyVideosTestDlWatcher:: aMpxId: %d, aDownloadId: %d", aMpxId, aDownloadId);
    PrintUrl( _L("aUrl:"), aUrl );

    CVCXMyVideosTestDownload* dl = GetDownload( aUrl );
    if( !dl )
        {
        CreateDownloadL( -1, -1, -1, aUrl, ETrue, _L(""), _L("") );
        dl = GetDownload( aUrl );
        if( !dl )
            {
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestErrorDownloadNotFound, KVCXMYVideosTestErrorDownloadNotFound );
            VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::StartDownloadL");
            return;
            }
        }

    dl->iMpxId = aMpxId;
    if( dl->iTitle )
        {
        delete dl->iTitle;
        dl->iTitle = NULL;
        }
    dl->iTitle = aTitle.AllocL();
    
    if( dl->iPath )
        {
        delete dl->iPath;
        }    
    dl->iPath = aPath.AllocL();
    
    dl->iMpxId = aMpxId;

    UpdateDownloadStateL( aMpxId, aDownloadId, aState, aProgress, KErrNone, KErrNone );
    
    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::StartDownloadL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::UpdateDownloadStateL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::UpdateDownloadStateL( TInt aMpxId, TInt aDownloadId,
        TVcxMyVideosDownloadState aState, TInt aProgress, TInt aDownloadError, TInt aGlobalError )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::UpdateDownloadStateL");
    
    CVCXMyVideosTestDownload* dl = GetDownloadByMpxId( aMpxId );
    if( !dl )
        {
        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestErrorDownloadNotFound, KVCXMYVideosTestErrorDownloadNotFound );
        VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::UpdateDownloadStateL");
        return;
        }
    
    TBool finishedDownload = dl && aState == EVcxMyVideosDlStateDownloaded && aDownloadId != 0
                            && dl->iProgress >= 100 && dl->iState == EVcxMyVideosDlStateDownloaded;
    
    if( finishedDownload )
        {
        VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::UpdateDownloadStateL");
        return;
        }
    
    VCXLOGLO3("CVCXMyVideosTestDlWatcher:: aDownloadError: %d, aGlobalError: %d", aDownloadError, aGlobalError);
    VCXLOGLO4("CVCXMyVideosTestDlWatcher:: aMpxId: %d, aState: %d, aProgress: %d", aMpxId, aState, aProgress);

    // This is main error and will be informed to client always.
    TInt error ( aGlobalError );

    // For other download manager related errors the global error code should be better
    if( aDownloadError == EInternal
            | aDownloadError == EContentFileIntegrity
            | aDownloadError == EDiskFull
            | aDownloadError == EConnectionFailed
            | aDownloadError == EMoveFailed
            | aDownloadError == EDestFileWriteFailed
            | aDownloadError == EMMCRemoved
            | aDownloadError == EBadUrl
            | aDownloadError == EWrongDestFilename
            | aDownloadError == EDestFileInUse
            | aDownloadError == EHttpUnhandled
            | aDownloadError == EHttpAuthenticationFailed
            | aDownloadError == EProxyAuthenticationFailed
            | aDownloadError == EObjectNotFound
            | aDownloadError == EPartialContentModified
            | aDownloadError == EContentExpired
            | aDownloadError == EHttpRestartFailed )
        {
        error = aDownloadError;
        }

    // Check for DL existance
    dl->iExists = ETrue;

    // Download was failed at previous update, inform client now.
    if( dl->iGlobalError == KVCXMYVideosTestMessageDlFailed && !dl->iInformed )
        {
        if( error != KErrNone )
            {
            VCXLOGLO1("CVCXMyVideosTestDlWatcher::UpdateDownloadStateL - DL failed at last update, we have error now.");
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlFailed, error );
            dl->iInformed = ETrue;
            TRAPD( actionErr, iStats->ActionEndL( *dl->iUrl, error ) );
            if( actionErr == KErrAbort )
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
            }
        else
            {
            VCXLOGLO1("CVCXMyVideosTestDlWatcher::UpdateDownloadStateL - DL failed at last update, still no error.");
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlFailed, dl->iGlobalError );
            dl->iInformed = ETrue;
            TRAPD( actionErr, iStats->ActionEndL( *dl->iUrl, dl->iGlobalError ) );
            if( actionErr == KErrAbort )
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
            }
        }

    // Inform the new state
    if( dl->iState != aState )
        {
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: New state: %d", aState);
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: Old state: %d", dl->iState);

        // Unusual case, signal error to script.
        if( dl->iState != EVcxMyVideosDlStateNone && aState == EVcxMyVideosDlStateNone && !dl->iCanceled )
            {
            VCXLOGLO1("CVCXMyVideosTestDlWatcher:: DL state changed to EVcxMyVideosDlStateNone, reporting as error.");
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlGeneral, KVCXMYVideosTestMessageDlGeneral );
            }

        if( dl->iState == EVcxMyVideosDlStatePaused && dl->iWaitingResume )
            {
            dl->iWaitingResume = EFalse;
            }

        dl->iInformed = EFalse;
        dl->iState = aState;
        dl->iDownloadError = aDownloadError;
        dl->iGlobalError = aGlobalError;
        
        TBuf<50> stateBuff;
        TPtrC stateStr = GetStateDesc( static_cast<TVcxMyVideosDownloadState>( dl->iState ) );
        stateBuff.Format( _L("State: %S"), &stateStr );
        TRAP_IGNORE( iStats->ActionProgressL( *dl->iUrl, stateBuff ) );
        }

    UpdateDownloadProgressL( aMpxId, aDownloadId, aProgress );

    dl->iDownloadId = aDownloadId;

    // Handle states which are informed to observer.
    if( !dl ->iInformed )
    switch( dl->iState )
        {
        case EVcxMyVideosDlStateNone:
            {

            }
            break;

        case EVcxMyVideosDlStateDownloading:
            {
            dl->iInformed = ETrue;
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlRunning, error );
            }
            break;

        case EVcxMyVideosDlStateFailed:
            {
            if( error != KErrNone )
                {
                VCXLOGLO1("CVCXMyVideosTestDlWatcher::UpdateDownloadStateL - DL failed, we have error.");
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlFailed, error );
                TRAPD( actionErr, iStats->ActionEndL( *dl->iUrl, error ) );
                if( actionErr == KErrAbort )
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                
                dl->iInformed = ETrue;
                }
            else
                {
                VCXLOGLO1("CVCXMyVideosTestDlWatcher::UpdateDownloadStateL - DL failed, no error yet.");
                //iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlFailed, KVCXMYVideosTestMessageDlFailed );
                // This will be informed at next update to client.
                dl->iGlobalError = KVCXMYVideosTestMessageDlFailed;
                }
            }
            break;

        case EVcxMyVideosDlStatePaused:
            {
            dl->iInformed = ETrue;
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlPaused, error );
            // Unexpected pause state, stop action.
            if( !dl->iWaitingPause )
                {
                TRAPD( actionErr, iStats->ActionEndL( *dl->iUrl, KErrGeneral ) );
                if( actionErr == KErrAbort )
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                }
            dl->iWaitingPause = EFalse;
            }
            break;

        case EVcxMyVideosDlStateDownloaded:
            {
            if( !BaflUtils::FileExists( iFs, *dl->iPath ) )
                {
                VCXLOGLO1("CVCXMyVideosTestDlWatcher:: DL finished without errors but file doesn't exist!");
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestErrorFileNotFound, KVCXMYVideosTestErrorFileNotFound );
                TRAPD( actionErr, iStats->ActionEndL( *dl->iUrl, KVCXMYVideosTestErrorFileNotFound ) );
                if( actionErr == KErrAbort )
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                dl->iInformed = ETrue;
                }
            else
                {
                dl->iInformed = ETrue;
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlSucceeded, error );
                TRAPD( actionErr, iStats->ActionEndL( *dl->iUrl, error ) );
                if( actionErr == KErrAbort )
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                }
            }
            break;

        default:
            break;
        }

    if( error != KErrNone && !dl->iInformed )
        {
        dl->iInformed = ETrue;
        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlGeneral, error );
        }

    VCXLOGLO1("<<<<CVCXMyVideosTestDlWatcher::UpdateDownloadStateL");
    }


// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::UpdateDownloadProgressL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::UpdateDownloadProgressL( TInt aMpxId, TInt aDownloadId, TInt aProgress )
    {
    CVCXMyVideosTestDownload* dl = GetDownloadByMpxId( aMpxId );
    if( dl )
        {
        if( ( aProgress > dl->iProgress + KVCXTestProgressUpdateInterval ) || ( aProgress == 100 && dl->iProgress != 100 ) )
            {
            dl->iProgress = aProgress;
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlProgressed, KErrNone );
            TBuf<50> progressBuff;
            progressBuff.Format( _L("Progress: %d"), aProgress );
            TRAP_IGNORE( iStats->ActionProgressL( *dl->iUrl, progressBuff ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::CancelDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::CancelDownloadL( TInt aServiceId, TInt aContentId, const TDesC& aUrl )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::CancelDownloadL");
    CVCXMyVideosTestDownload* dl = GetDownload( aServiceId, aContentId, aUrl );
    if( dl )
        {
        dl->iCanceled = ETrue;
        }
    else
        {
        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestErrorDownloadNotFound, KVCXMYVideosTestErrorDownloadNotFound );
        }
    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::CancelDownloadL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::SetDownloadResumedFlagL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::SetDownloadResumedFlagL( TInt aMpxId, TInt /* aDownloadId */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::SetDownloadResumedFlagL");

    CVCXMyVideosTestDownload* dl = GetDownloadByMpxId( aMpxId );
    if( dl )
        {
        dl->iWaitingResume = ETrue;
        dl->iWaitingPause = EFalse;
        dl->iInformed = EFalse;
        dl->iDownloadError = KErrNone;
        dl->iGlobalError = KErrNone;
        }

    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::SetDownloadResumedFlagL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::SetDownloadPausedFlagL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::SetDownloadPausedFlagL( TInt aMpxId, TInt /* aDownloadId */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::SetDownloadPausedFlagL");

    CVCXMyVideosTestDownload* dl = GetDownloadByMpxId( aMpxId );
    if( dl )
        {
        dl->iWaitingResume = EFalse;
        dl->iWaitingPause = ETrue;
        dl->iInformed = EFalse;
        dl->iDownloadError = KErrNone;
        dl->iGlobalError = KErrNone;
        }

    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::SetDownloadPausedFlagL");
    }
// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::GetDownloadByMpxId
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDownload* CVCXMyVideosTestDlWatcher::GetDownloadByMpxId( TInt aMpxId )
    {
    for( TInt i=0; i<iDownloads.Count(); i++ )
        {
        if( iDownloads[i]->iMpxId == aMpxId )
            {
            return iDownloads[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::GetDownloadByDownloadId
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDownload* CVCXMyVideosTestDlWatcher::GetDownloadByDownloadId( TInt aDownloadId )
    {
    for( TInt i=0; i<iDownloads.Count(); i++ )
        {
        if( iDownloads[i]->iDownloadId == aDownloadId )
            {
            return iDownloads[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::GetDownload
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDownload* CVCXMyVideosTestDlWatcher::GetDownload( TInt aServiceId, TInt aContentId, const TDesC& aUrl )
    {
    for( TInt i=0; i<iDownloads.Count(); i++ )
        {
        if( iDownloads[i]->iServiceId == aServiceId &&
            iDownloads[i]->iContentId == aContentId &&
            iDownloads[i]->iUrl->Des() == aUrl )
            {
            return iDownloads[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::GetDownloadByIndex
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDownload* CVCXMyVideosTestDlWatcher::GetDownloadByIndex( TInt aIndex )
    {
    if( aIndex >= iDownloads.Count() ) return NULL;
    return iDownloads[aIndex];
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::GetDownloadCount
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosTestDlWatcher::GetDownloadCount()
    {
    return iDownloads.Count();
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::GetFailedDownloadCount
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosTestDlWatcher::GetFailedDownloadCount()
    {
    TInt count( 0 );

    for( TInt i=0; i<iDownloads.Count(); i++ )
        {
        if( iDownloads[i]->iState == EVcxMyVideosDlStateFailed )
            {
            count++;
            }
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::CleanupDownloadsL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::CleanupDownloadsL( TInt aServiceId, TInt aContentId, const TDesC& aUrl )
    {
    if( aUrl.Length() <= 0 ) return;

    // Remove duplicates which are finished.
    for( TInt i=iDownloads.Count()-1; i>=0; i-- )
        {
        if( iDownloads[i]->iUrl &&
            iDownloads[i]->iServiceId == aServiceId &&
            iDownloads[i]->iContentId == aContentId &&
            iDownloads[i]->iUrl->Des() == aUrl &&
            ( iDownloads[i]->iState == EVcxMyVideosDlStateDownloaded ||
              iDownloads[i]->iState == EVcxMyVideosDlStateFailed ) )
            {
            CVCXMyVideosTestDownload* dl = iDownloads[i];
            iDownloads.Remove( i );
            delete dl;
            }
        }
    
    // Keep only 10 newest downloads.
    if( iDownloads.Count() > 10 )
        {
        TInt tooMany = iDownloads.Count() - 10;
        for( TInt i = tooMany - 1; i >= 0; i-- )
            {
            CVCXMyVideosTestDownload* dl = iDownloads[i];
            iDownloads.Remove( i );
            delete dl;
            }
        }
    
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::GetDownload
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDownload* CVCXMyVideosTestDlWatcher::GetDownload( const TDesC& aUrl )
    {
    CVCXMyVideosTestDownload* dl( NULL );

    for( TInt i=0; i<iDownloads.Count(); i++ )
        {
        if( iDownloads[i]->iUrl->Des() == aUrl )
            {
            // Take first with same URL.
            if( dl == NULL )
                {
                dl = iDownloads[i];
                }
            else
                {
                // If there's more dl items with same URL then prefer one with dl state none.
                if( iDownloads[i]->iState == EVcxMyVideosDlStateNone )
                    {
                    dl = iDownloads[i];
                    }
                }
            }
        }
    return dl;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::PrintDownloads
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::PrintDownloads( )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::PrintDownloads");

    for( TInt i=0; i<iDownloads.Count(); i++ )
        {
        CVCXMyVideosTestDownload* dl = iDownloads[i];
        VCXLOGLO1("--------------------------------------------------------------------------------");

        if( dl->iTitle )
            {
            TPtr titlePtr = dl->iTitle->Des();
            VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iTitle: %S", &titlePtr);
            }
        if( dl->iPath )
            {
            TPtr pathPtr = dl->iPath->Des();
            VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iPath: %S", &pathPtr);
            }
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iMpxId: %d", dl->iMpxId);
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iDownloadId: %d", dl->iDownloadId);
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iContentId: %d", dl->iContentId);
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iServiceId: %d", dl->iServiceId);
        PrintUrl( _L("CVCXMyVideosTestDlWatcher:: iUrl:"), *dl->iUrl );
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iProgress: %d", dl->iProgress);
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iDownloadError: %d", dl->iDownloadError);
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iGlobalError: %d", dl->iGlobalError);
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iInformed: %d", dl->iInformed);
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: iWaitingResume: %d", dl->iWaitingResume);

        TPtrC stateDesc = GetStateDesc( static_cast<TVcxMyVideosDownloadState>( dl->iState ) );
        
        VCXLOGLO2("CVCXMyVideosTestDlWatcher:: State: %S", &stateDesc);
        }
    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::PrintDownloads");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::PrintUrl
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::PrintUrl( const TDesC& aPre, const TDesC& aUrl )
    {
    if( aUrl.Length() > 80 )
        {
        TPtrC left( aUrl.Left( 38 ) );
        TPtrC right( aUrl.Right( 38 ) );
        VCXLOGLO4("%S %S..%S", &aPre, &left, &right );
        }
    else
        {
        VCXLOGLO3("%S %S", &aPre, &aUrl );
        }
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::PrepareCheck
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::PrepareCheck( )
    {
    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::PrepareCheck");

    for( TInt i=0; i<iDownloads.Count(); i++ )
        {
        iDownloads[i]->iExists = EFalse;
        }

    iDlCheckActive = ETrue;
    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::PrepareCheck");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::FinishCheckL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::FinishCheckL( )
    {
    if( iDownloads.Count() <= 0) return;

    VCXLOGLO1(">>>CVCXMyVideosTestDlWatcher::FinishCheckL");

    // Check that all downloads exist.
    for( TInt i = iDownloads.Count()-1; i>=0; i-- )
        {
        if( !iDownloads[i]->iExists )
            {
            if( iDownloads[i]->iCanceled )
                {
                TRAPD( actionErr, iStats->ActionEndL( *iDownloads[i]->iUrl, KErrNone ) );
                if( actionErr == KErrAbort )
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                VCXLOGLO2("CVCXMyVideosTestDlWatcher:: Dl is canceled, mpxId: %d.", iDownloads[i]->iMpxId);
                CVCXMyVideosTestDownload* dl = iDownloads[i];
                iDownloads.Remove( i );
                delete dl;
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlCanceled, KErrNone );
                }
            else
                {
                if( iDownloads[i]->iState != EVcxMyVideosDlStateNone &&
                    iDownloads[i]->iState != EVcxMyVideosDlStateDownloaded &&
                    !iDlCheckDisabled )
                    {
                    TRAP_IGNORE( iStats->ActionTraceL( _L("Download item was not found.") ) );
                    TRAPD( actionErr, iStats->ActionEndL( *iDownloads[i]->iUrl, KErrNone ) );
                    if( actionErr == KErrAbort )
                        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                    
                    VCXLOGLO2("CVCXMyVideosTestDlWatcher:: Dl doesn't exist anymore, mpxId: %d. Error!", iDownloads[i]->iMpxId);
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDlFailed, KErrCorrupt );
                    CVCXMyVideosTestDownload* dl = iDownloads[i];
                    iDownloads.Remove( i );
                    delete dl;                    
                    }
                }
            }
        }

    iDlCheckActive = EFalse;
    VCXLOGLO1("<<<CVCXMyVideosTestDlWatcher::FinishCheckL");
    }
// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::DisableDownloadCheck
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::DisableDownloadCheck( )
    {
    iDlCheckDisabled = ETrue;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::Reset
// -----------------------------------------------------------------------------
//
void CVCXMyVideosTestDlWatcher::Reset( )
    {
    iDownloads.ResetAndDestroy();
    }


// -----------------------------------------------------------------------------
// CVCXMyVideosTestDlWatcher::GetStateDesc
// -----------------------------------------------------------------------------
//
const TDesC& CVCXMyVideosTestDlWatcher::GetStateDesc( TVcxMyVideosDownloadState aState )
    {
    switch( aState )
        {
        default:
        case EVcxMyVideosDlStateNone:
            {
            return KEVcxMyVideosDlStateNone;
            }

        case EVcxMyVideosDlStateDownloading:
            {
            return KEVcxMyVideosDlStateDownloading;
            }

        case EVcxMyVideosDlStateFailed:
            {
            return KEVcxMyVideosDlStateFailed;
            }

        case EVcxMyVideosDlStatePaused:
            {
            return KEVcxMyVideosDlStatePaused;
            }

        case EVcxMyVideosDlStateDownloaded:
            {
            return KEVcxMyVideosDlStateDownloaded;
            }
        }
    }

//  End of File

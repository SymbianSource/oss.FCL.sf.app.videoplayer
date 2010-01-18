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
* Description: 
*
*/




#include "IptvDebug.h"
#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <mpxmediageneraldefs.h>

#include "vcxnsdownloadupdater.h"
#include "vcxnscontent.h"

const TInt KUpdateDelay = 3000000; // 3 seconds

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxNsDownloadUpdater::CVcxNsDownloadUpdater( MVcxNsDownloadUpdaterObserver* aObserver ):
    iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsDownloadUpdater::ConstructL()
    {
    iTimer = CPeriodic::NewL( EPriorityNormal );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CVcxNsDownloadUpdater* CVcxNsDownloadUpdater::NewL( MVcxNsDownloadUpdaterObserver* aObserver )
    {
    CVcxNsDownloadUpdater* self = new( ELeave ) CVcxNsDownloadUpdater( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxNsDownloadUpdater::~CVcxNsDownloadUpdater()
    {
    if( iTimer )
        {
        iTimer->Cancel();
        }
    
    delete iTimer;
    
    iDownloadArray.Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsDownloadUpdater::AddDownload( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::AddDownload IN");
    
    if ( !aContent )
        {
        return;
        }
    
    CMPXMedia* media = aContent->GetMpxMedia();
    
    if( !media )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::AddDownload OUT, media == NULL");
        return;
        }

    TInt index( KErrNotFound );
    
    if( !FindDownload( aContent, &index ) )
        {
        TDownloadItem item;
        item.iContent = aContent;
        
        TUint32 status = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
        TInt8 progress = media->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );

        item.iProgress = progress;
        item.iState = status;
        
        iDownloadArray.Append( item );
        }
    
    if( !iTimer->IsActive() )
        {
        TCallBack callback = TCallBack( Callback, this );
        const TTimeIntervalMicroSeconds32 KUpdateInterval( KUpdateDelay );
        iTimer->Start( KUpdateInterval, KUpdateInterval, callback );
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::AddDownload OUT");
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CVcxNsDownloadUpdater::RemoveDownload( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::RemoveDownload IN");

    if ( !aContent )
        {
        return;
        }
    
    TInt index( KErrNotFound );
    
    if( FindDownload( aContent, &index ) )
        {
        iDownloadArray.Remove( index );
        }
    
    if( iDownloadArray.Count() < 1 )
        {
        iTimer->Cancel();        
        } 

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::RemoveDownload OUT");
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CVcxNsDownloadUpdater::RemoveAllDownloads()
    {
    iDownloadArray.Reset();
    
    iTimer->Cancel(); 
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CVcxNsDownloadUpdater::Callback( TAny *aPtr )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::Callback IN");
    
    CVcxNsDownloadUpdater* ptr = static_cast<CVcxNsDownloadUpdater*>( aPtr );
    
    TInt err ( KErrNotFound );
    
    if ( ptr )
        {
        TRAP( err, ptr->UpdateDownloadsL() ); 
        }

#ifdef _DEBUG
    if( err )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsDownloadUpdater::Callback leaved with %d", err);
        }
#endif // _DEBUG
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::Callback OUT");
    
    return 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsDownloadUpdater::UpdateDownloadsL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::UpdateDownloadsL IN");
    
    for ( TInt i = 0; i < iDownloadArray.Count(); i++ )
        {
        UpdateDownloadL( iDownloadArray[i] );
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::UpdateDownloadsL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsDownloadUpdater::UpdateDownloadL( TDownloadItem& aDownload )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::UpdateDownloadL IN");
    
    CMPXMedia* media = aDownload.iContent->GetMpxMedia();
    
    if ( !media )
        {
        return;
        }
    
    TUint32 status = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
    TInt8 progress = media->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );
    TUint32 mpxId  = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1;

    if ( progress == 0 && aDownload.iProgress > 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::UpdateDownloadL: Resume started from beginning.");
        iObserver->ResumeStartedFromBeginningL( mpxId );
        }
    if ( progress != aDownload.iProgress || status != aDownload.iState )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsDownloadUpdater:: dl state    = %d", status );
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsDownloadUpdater:: dl progress = %d", progress );
        
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::UpdateDownloadL: updating video");
        aDownload.iProgress = progress;
        aDownload.iState = status;        

        iObserver->UpdateVideoObject( mpxId );
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::UpdateDownloadL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CVcxNsDownloadUpdater::FindDownload( CVcxNsContent* aContent, TInt* aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::FindDownload IN");
    
    for ( TInt i = iDownloadArray.Count()-1; i >= 0; i-- )
        {
        if ( iDownloadArray[i].iContent == aContent )
            {
            *aIndex = i;
            
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::FindDownload OUT: found");
            
            return ETrue;
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsDownloadUpdater::FindDownload OUT: NOT found");
    
    return EFalse;
    }

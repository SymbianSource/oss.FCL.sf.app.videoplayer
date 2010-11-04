/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    IAD update handling.
*
*/


// Version : %version: 1 %

#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include <featmgr.h>
#include <mpxviewutility.h>

#include "mpxvideoplayeriadupdate.h"
#include "mpxvideoplayerconstants.h"
#include "mpxvideo_debug.h"

const TUid KIadParamUid = { 0x200159B2 }; // Uid of VideoPlayer.sis
_LIT( KIadParamExec, "mpxvideoplayer.exe" );


// --------------------------------------------------------------------------
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CMpxVideoPlayerIadUpdate* CMpxVideoPlayerIadUpdate::NewL( MMPXViewUtility& aViewUtility )
    {
    CMpxVideoPlayerIadUpdate* self = new( ELeave ) CMpxVideoPlayerIadUpdate( aViewUtility );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// --------------------------------------------------------------------------
//
CMpxVideoPlayerIadUpdate::CMpxVideoPlayerIadUpdate( MMPXViewUtility& aViewUtility )
  : iUpdate( NULL ), iParameters( NULL ), iViewUtility( aViewUtility )
    {
    // None
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerIadUpdate::ConstructL()
    {
    MPX_ENTER_EXIT( _L("CMpxVideoPlayerIadUpdate::ConstructL()") );

    if( FeatureManager::FeatureSupported( KFeatureIdIAUpdate ) )
        {
        iUpdate = CIAUpdate::NewL( *this );
        iParameters = CIAUpdateParameters::NewL();
        }
    else
        {
        MPX_DEBUG(_L("CMpxVideoPlayerIadUpdate::ConstructL(), Feature not supported"));
        }
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CMpxVideoPlayerIadUpdate::~CMpxVideoPlayerIadUpdate()
    {
    Delete();
    }

// --------------------------------------------------------------------------
// Starts update process.
// --------------------------------------------------------------------------
//
void CMpxVideoPlayerIadUpdate::StartL()
    {
    MPX_ENTER_EXIT( _L("CMpxVideoPlayerIadUpdate::StartL()") );

    if( iUpdate && iParameters )
        {
        iParameters->SetUid( KIadParamUid );
        // We want Videos to be started after update is finished
        iParameters->SetCommandLineExecutableL( KIadParamExec );
        iParameters->SetShowProgress( EFalse );
    
        // Check the updates
        iUpdate->CheckUpdates( *iParameters );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//  
void CMpxVideoPlayerIadUpdate::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    MPX_ENTER_EXIT( _L("CMpxVideoPlayerIadUpdate::CheckUpdatesComplete()"), 
        _L("aErrorCode: %d, aAvailableUpdates: %d"), aErrorCode, aAvailableUpdates );

    if ( aErrorCode == KErrNone )
        {
        if ( aAvailableUpdates > 0 && iViewUtility.ActiveViewType() == KUidMyVideosViewType )
            {
            // There were some updates available and video list is active.
            iUpdate->UpdateQuery();
            }
        else
            {
            // No updates available or playback ongoing.
            Delete();
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//  
void CMpxVideoPlayerIadUpdate::UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResult )
    {
    MPX_ENTER_EXIT( _L("CMpxVideoPlayerIadUpdate::UpdateComplete()"), 
        _L("aErrorCode: %d, SuccessCount: %d"), aErrorCode, aResult->SuccessCount() );

    delete aResult; // Ownership was transferred, so this must be deleted by the client

    // We do not need the client-server session anymore
    Delete();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//  
void CMpxVideoPlayerIadUpdate::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )
    {
    MPX_ENTER_EXIT( _L("CMpxVideoPlayerIadUpdate::UpdateQueryComplete()"),
        _L("aErrorCode: %d, aUpdateNow: %d"), aErrorCode, aUpdateNow );

    if ( aErrorCode == KErrNone )
        {
        if ( aUpdateNow )
            {
            // User choosed to update now, so let's launch the IAUpdate UI.
            iUpdate->ShowUpdates( *iParameters );
            }
        else
            {
            // The answer was 'Later'. 
            Delete();
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//  
void CMpxVideoPlayerIadUpdate::Delete()
    {
    if ( iUpdate )
        {
        delete iUpdate;
        iUpdate = NULL;
        }
    if ( iParameters )
        {
        delete iParameters; 
        iParameters = NULL;
        }
    }


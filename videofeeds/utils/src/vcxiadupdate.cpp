/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    IAD update handling.*
*/




// INCLUDE FILES
#include "vcxiadupdate.h"
#include "IptvDebug.h"
#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include <featmgr.h>

// CONSTANTS
const TUid KIadParamUid = { 0x102750D5 }; // Uid of VideoCenter.sis
//_LIT( KIadParamSearch, "some search criteria" );
#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
_LIT( KIadParamExec, "vcxnsappui.exe" );
#else
_LIT( KIadParamExec, "iptvappui.exe" );
#endif // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
//_LIT8( KIadParamArg, " -x -x" );

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CVcxIadUpdate::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CVcxIadUpdate* CVcxIadUpdate::NewL()
    {
    CVcxIadUpdate* self = new( ELeave ) CVcxIadUpdate();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CVcxIadUpdate::CVcxIadUpdate
// C++ default constructor can NOT contain any code, that might leave.
// --------------------------------------------------------------------------
//
CVcxIadUpdate::CVcxIadUpdate()
  : iUpdate( NULL ), iParameters( NULL )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CVcxIadUpdate::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVcxIadUpdate::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxIadUpdate::ConstructL()" );

    if( FeatureManager::FeatureSupported( KFeatureIdIAUpdate ) )
        {
        iUpdate = CIAUpdate::NewL( *this );
        iParameters = CIAUpdateParameters::NewL();
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "CVcxIadUpdate::ConstructL(), Feature not supported" );
        }
    }

// --------------------------------------------------------------------------
// CVcxIadUpdate::~CVcxIadUpdate
// Destructor
// --------------------------------------------------------------------------
//
CVcxIadUpdate::~CVcxIadUpdate()
    {
    Delete();
    }

// --------------------------------------------------------------------------
// CVcxIadUpdate::StartL
// Starts update process.
// --------------------------------------------------------------------------
//
EXPORT_C void CVcxIadUpdate::StartL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxIadUpdate::StartL()" );

    if( iUpdate && iParameters )
        {
        iParameters->SetUid( KIadParamUid );
        //iParameters->SetSearchCriteriaL( KIAParamSearch );
        // We want VC to be started after update is finished
        iParameters->SetCommandLineExecutableL( KIadParamExec );
        //iParameters->SetCommandLineArgumentsL( KIAParamArg );
        iParameters->SetShowProgress( EFalse );
    
        // Check the updates
        iUpdate->CheckUpdates( *iParameters );
        }
    }

// -----------------------------------------------------------------------------
// CVcxIadUpdate::CheckUpdatesComplete
// -----------------------------------------------------------------------------
//  
void CVcxIadUpdate::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    IPTVLOGSTRING3_LOW_LEVEL( 
        "CVcxIadUpdate::CheckUpdatesComplete(), aErrorCode: %d, aAvailableUpdates: %d",
        aErrorCode, aAvailableUpdates );

    if ( aErrorCode == KErrNone )
        {
        if ( aAvailableUpdates > 0 )
            {
            // There were some updates available.
            iUpdate->UpdateQuery();         
            }
        else
            {
            // No updates available.
            Delete();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxIadUpdate::UpdateComplete
// -----------------------------------------------------------------------------
//  
void CVcxIadUpdate::UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResult )
    {
    IPTVLOGSTRING3_LOW_LEVEL( 
        "CVcxIadUpdate::UpdateComplete(), aErrorCode: %d, SuccessCount: %d",
        aErrorCode, aResult->SuccessCount() );

    if ( aErrorCode == KErrNone )
        {
        // The update process that the user started from IAUpdate UI is now completed.
        // If the client application itself was updated in the update process, this callback
        // is never called, since the client is not running anymore. 
        TInt successCount = aResult->SuccessCount();        
        }
    
    delete aResult; // Ownership was transferred, so this must be deleted by the client

    // We do not need the client-server session anymore
    Delete();
    }

// -----------------------------------------------------------------------------
// CVcxIadUpdate::UpdateQueryComplete
// -----------------------------------------------------------------------------
//  
void CVcxIadUpdate::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )
    {
    IPTVLOGSTRING3_LOW_LEVEL( 
        "CVcxIadUpdate::UpdateComplete(), aErrorCode: %d, aUpdateNow: %d",
        aErrorCode, aUpdateNow );

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
// CVcxIadUpdate::Delete
// -----------------------------------------------------------------------------
//  
void CVcxIadUpdate::Delete()
    {
    delete iUpdate; iUpdate = NULL;
    delete iParameters; iParameters = NULL;                
    }

//  End of File

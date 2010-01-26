/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   App settings handles storing and loading of application settings*
*/



// INCLUDE FILES
#include <e32def.h>
#include <bldvariant.hrh>
#include <bautils.h>
#include <centralrepository.h>
#include <commdb.h>
#include <sysutil.h>
#include <mmtsy_names.h>
#include <secui.h>
#include <secuisecurityhandler.h>

#include "videosettingsdrivemonitor.h"

#include "videoplayersettingsengine.pan"
#include "videoplayersettingsengine.h"
#include "videoplayercenrepkeys.h"
#include "mpxlog.h"

// CONSTANTS
const TInt KTimeBeforeRetryingServerConnection = 50000;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::CVcxNsSettings()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsEngine::CVcxNsSettingsEngine( )
 :  iPhoneConnected( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::ConstructL
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsEngine::ConstructL()
    {
    iCenRep = CRepository::NewL( KIptvCenRepUid );
    iAlrCenRep = CRepository::NewL( KIptvAlrCenRepUid );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CVcxNsSettingsEngine* CVcxNsSettingsEngine::NewL( )
    {
    MPX_FUNC("IptvUiEngine ## CIptvAppSettings::NewL()");
    
    CVcxNsSettingsEngine* self = new (ELeave) CVcxNsSettingsEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::~CIptvAppSettings()
// -----------------------------------------------------------------------------
//
CVcxNsSettingsEngine::~CVcxNsSettingsEngine()
    {
    MPX_FUNC("IptvUiEngine ## CIptvAppSettings::~CIptvAppSettings()");

    if ( iPhoneConnected )
	    {
	    // close phone
	    if ( iPhone.SubSessionHandle() )
	        {
	        iPhone.Close();
	        }

	    //close ETel connection
	    if ( iServer.Handle() )
	        {
	        iServer.UnloadPhoneModule( KMmTsyModuleName );
	        iServer.Close();
	        }
    	}

    delete iCenRep;
    delete iAlrCenRep;

    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::GetUsedMemory()
// -----------------------------------------------------------------------------
//
EXPORT_C void CVcxNsSettingsEngine::GetUsedMemoryL( TInt& aUsedMemory )
    {
    MPX_FUNC("CVcxNsSettingsEngine::GetUsedMemoryL() start");

    CVideoSettingsDriveMonitor* driveMonitor = CVideoSettingsDriveMonitor::NewL();
    CleanupStack::PushL( driveMonitor ); // 1->
                
    driveMonitor->GetUsedMemoryL( aUsedMemory );

    CleanupStack::PopAndDestroy( driveMonitor ); // <-1    
    }


// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::SetUsedMemory()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::SetUsedMemory( TInt aUsedMemory )
    {
    return SetCenRepIntKey( KIptvCenRepUid, 
                            KIptvCenRepPreferredMemoryKey, 
                            aUsedMemory );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::GetParentControl()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::GetParentControl( TInt& aParentControl )
    {
    return GetCenRepIntKey( KIptvCenRepUid, 
                            KIptvCenRepParentControlKey,
                            aParentControl );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::SetParentControl()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::SetParentControl( TInt aParentControl )
    {
    return SetCenRepIntKey( KIptvCenRepUid, 
                            KIptvCenRepParentControlKey, 
                            aParentControl );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::GetVideoOnDemandThumbnails()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::GetVideoOnDemandThumbnails( TInt& aVodThumbnails )
    {
    return GetCenRepIntKey( KIptvCenRepUid, 
                            KIptvCenRepVoDThumbnailsKey, 
                            aVodThumbnails ); 
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::SetVideoOnDemandThumbnails()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::SetVideoOnDemandThumbnails( TInt aVodThumbnails )
    {
    return SetCenRepIntKey( KIptvCenRepUid, KIptvCenRepVoDThumbnailsKey, aVodThumbnails );
    }


// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::GetUsedSnapId()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::GetUsedSnapId( TInt& aUsedSnapId )
    {
    return GetCenRepIntKey( KIptvAlrCenRepUid, KIptvCenRepUsedSnapIdKey, aUsedSnapId );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::SetUsedSnapId()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::SetUsedSnapId( TInt aUsedSnapId )
    {
    return SetCenRepIntKey( KIptvAlrCenRepUid, KIptvCenRepUsedSnapIdKey, aUsedSnapId );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::GetAllowUseGprs()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::GetAllowUseGprs( TInt& aAllowUseGprs )
    {
    return GetCenRepIntKey( KIptvAlrCenRepUid, KIptvCenRepAllowUseGprsKey, aAllowUseGprs );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::SetAllowUseGprs()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::SetAllowUseGprs( TInt aAllowUseGprs )
    {
    return SetCenRepIntKey( KIptvAlrCenRepUid, KIptvCenRepAllowUseGprsKey, aAllowUseGprs );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::GetAllowRoaming()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::GetAllowRoaming( TInt& aAllowRoaming )
    {
    return GetCenRepIntKey( KIptvAlrCenRepUid, KIptvCenRepAllowRoamingKey, aAllowRoaming );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::SetAllowRoaming()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::SetAllowRoaming( TInt aAllowRoaming )
    {
    return SetCenRepIntKey( KIptvAlrCenRepUid, KIptvCenRepAllowRoamingKey, aAllowRoaming );
    }

    
// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::CheckLockCodeL()
// -----------------------------------------------------------------------------
//   
EXPORT_C TBool CVcxNsSettingsEngine::CheckLockCodeL()
    {
    TBool lockCodeAccepted = EFalse ;
    
    if ( !iPhoneConnected )
        {
        // Connect to ETel server
        // All server connections are retried because occasional
        // fails on connections are possible, at least on some servers.
        TInt err = iServer.Connect();
        if ( err != KErrNone )
            {
            User::After( KTimeBeforeRetryingServerConnection );
            TInt err = iServer.Connect();
            }
        User::LeaveIfError( err );
        User::LeaveIfError( iServer.SetExtendedErrorGranularity(
            RTelServer::EErrorExtended ) );

        // Load TSY.
        err = iServer.LoadPhoneModule( KMmTsyModuleName );
        if ( KErrAlreadyExists != err )
            {
            // May also return KErrAlreadyExists if something else
            // has already loaded the TSY module. And that is
            // not an error.
            User::LeaveIfError( err );
            }

        // Open phones
        User::LeaveIfError( iPhone.Open( iServer, KMmTsyPhoneName ) );
        iPhoneConnected = ETrue;
        }

#ifndef __WINS__
    // Initialize sec ui.
    CSecurityHandler* handler = new( ELeave ) CSecurityHandler( iPhone );
    CleanupStack::PushL( handler );
    TSecUi::InitializeLibL();

    lockCodeAccepted = handler->AskSecCodeL();

	// Uninitialize security ui.
	CleanupStack::PopAndDestroy( handler );
	TSecUi::UnInitializeLib();
#else
        lockCodeAccepted = ETrue;
#endif

    return lockCodeAccepted;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::GetCenRepIntKey()
// -----------------------------------------------------------------------------
//            
TBool CVcxNsSettingsEngine::GetCenRepIntKey( TUid aCategory, TInt aKey, TInt& aValue )
    {
    TInt error = KErrNotFound;

    if ( aCategory == KIptvCenRepUid && iCenRep )
        {
        error = iCenRep->Get( aKey, aValue );
        }
    else if ( aCategory == KIptvAlrCenRepUid && iAlrCenRep )
        {
        error = iAlrCenRep->Get( aKey, aValue );
        }
    
    return (error == KErrNone) ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::SetCenRepIntKey()
// -----------------------------------------------------------------------------
//
TBool CVcxNsSettingsEngine::SetCenRepIntKey( TUid aCategory, TInt aKey, TInt aValue )
    {
    TInt error = KErrNotFound;

    if ( aCategory == KIptvCenRepUid && iCenRep )
        {
        error = iCenRep->Set( aKey, aValue );
        }

    else if ( aCategory == KIptvAlrCenRepUid && iAlrCenRep )
        {
        error = iAlrCenRep->Set( aKey, aValue );
        }

    return (error == KErrNone) ? ETrue : EFalse;    
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::GetPlayerVolume()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::GetPlayerVolume( TInt& aVolume )
    {
    return GetCenRepIntKey( KIptvCenRepUid, 
                            KIptvCenRepVideoPlayerVolumeKey, 
                            aVolume );
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngine::SetPlayerVolume()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVcxNsSettingsEngine::SetPlayerVolume( TInt aVolume )
    {
    return SetCenRepIntKey( KIptvCenRepUid, 
                            KIptvCenRepVideoPlayerVolumeKey, 
                            aVolume );
    }

// End of File
